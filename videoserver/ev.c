#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "ev.h"

typedef struct ev_api_state {
    int epfd;
    struct epoll_event *events;
} ev_api_state;

static int ev_api_create(ev_event_loop *event_loop) {
    ev_api_state *state = malloc(sizeof(ev_api_state));

    if (!state) return -1;
    state->events = malloc(sizeof(struct epoll_event)*event_loop->setsize);
    if (!state->events) {
        free(state);
        return -1;
    }
    state->epfd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    if (state->epfd == -1) {
        free(state->events);
        free(state);
        return -1;
    }
    event_loop->apidata = state;
    return 0;
}

static int ev_api_resize(ev_event_loop *event_loop, int setsize) {
    ev_api_state *state = event_loop->apidata;

    state->events = realloc(state->events, sizeof(struct epoll_event)*setsize);
    return 0;
}

static void ev_api_free(ev_event_loop *event_loop) {
    ev_api_state *state = event_loop->apidata;

    close(state->epfd);
    free(state->events);
    free(state);
}

static int ev_api_add_event(ev_event_loop *event_loop, int fd, int mask) {
    ev_api_state *state = event_loop->apidata;
    struct epoll_event ee;
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = event_loop->events[fd].mask == EV_NONE ?
            EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= event_loop->events[fd].mask; /* Merge old events */
    if (mask & EV_READABLE) ee.events |= EPOLLIN;
    if (mask & EV_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = fd;
    if (epoll_ctl(state->epfd,op,fd,&ee) == -1) return -1;
    return 0;
}

static void ev_api_del_event(ev_event_loop *event_loop, int fd, int delmask) {
    ev_api_state *state = event_loop->apidata;
    struct epoll_event ee;
    int mask = event_loop->events[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & EV_READABLE) ee.events |= EPOLLIN;
    if (mask & EV_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = fd;
    if (mask != EV_NONE) {
        epoll_ctl(state->epfd,EPOLL_CTL_MOD,fd,&ee);
    } else {
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for
         * EPOLL_CTL_DEL. */
        epoll_ctl(state->epfd,EPOLL_CTL_DEL,fd,&ee);
    }
}

static int ev_api_poll(ev_event_loop *event_loop, struct timeval *tvp) {
    ev_api_state *state = event_loop->apidata;
    int retval, numevents = 0;

    retval = epoll_wait(state->epfd,state->events,event_loop->setsize,
            tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);
    if (retval > 0) {
        int j;

        numevents = retval;
        for (j = 0; j < numevents; j++) {
            int mask = 0;
            struct epoll_event *e = state->events+j;

            if (e->events & EPOLLIN) mask |= EV_READABLE;
            if (e->events & EPOLLOUT) mask |= EV_WRITABLE;
            if (e->events & EPOLLERR) mask |= EV_WRITABLE;
            if (e->events & EPOLLHUP) mask |= EV_WRITABLE;
            event_loop->fired[j].fd = e->data.fd;
            event_loop->fired[j].mask = mask;
        }
    }
    return numevents;
}

static char *ev_api_name(void) {
    return "epoll";
}


ev_event_loop *ev_create_event_loop(int setsize) {
    ev_event_loop *event_loop;
    int i;

    if ((event_loop = malloc(sizeof(*event_loop))) == NULL) goto err;
    event_loop->events = malloc(sizeof(ev_file_event)*setsize);
    event_loop->fired = malloc(sizeof(ev_fired_event)*setsize);
    if (event_loop->events == NULL || event_loop->fired == NULL) goto err;
    event_loop->setsize = setsize;
    event_loop->last_time = time(NULL);
    event_loop->time_event_head = NULL;
    event_loop->time_event_next_id = 0;
    event_loop->stop = 0;
    event_loop->maxfd = -1;
    event_loop->beforesleep = NULL;
    if (ev_api_create(event_loop) == -1) goto err;
    /* Events with mask == EV_NONE are not set. So let's initialize the
     * vector with it. */
    for (i = 0; i < setsize; i++)
        event_loop->events[i].mask = EV_NONE;
    return event_loop;

err:
    if (event_loop) {
        free(event_loop->events);
        free(event_loop->fired);
        free(event_loop);
    }
    return NULL;
}

/* Return the current set size. */
int ev_get_setsize(ev_event_loop *event_loop) {
    return event_loop->setsize;
}

/* Resize the maximum set size of the event loop.
 * If the requested set size is smaller than the current set size, but
 * there is already a file descriptor in use that is >= the requested
 * set size minus one, EV_ERR is returned and the operation is not
 * performed at all.
 *
 * Otherwise EV_OK is returned and the operation is successful. */
int ev_resize_setsize(ev_event_loop *event_loop, int setsize) {
    int i;

    if (setsize == event_loop->setsize) return EV_OK;
    if (event_loop->maxfd >= setsize) return EV_ERR;
    if (ev_api_resize(event_loop,setsize) == -1) return EV_ERR;

    event_loop->events = realloc(event_loop->events,sizeof(ev_file_event)*setsize);
    event_loop->fired = realloc(event_loop->fired,sizeof(ev_fired_event)*setsize);
    event_loop->setsize = setsize;

    /* Make sure that if we created new slots, they are initialized with
     * an EV_NONE mask. */
    for (i = event_loop->maxfd+1; i < setsize; i++)
        event_loop->events[i].mask = EV_NONE;
    return EV_OK;
}

void ev_delete_event_loop(ev_event_loop *event_loop) {
    ev_api_free(event_loop);
    free(event_loop->events);
    free(event_loop->fired);
    free(event_loop);
}

void ev_stop(ev_event_loop *event_loop) {
    event_loop->stop = 1;
}

int ev_create_file_event(ev_event_loop *event_loop, int fd, int mask,
        ev_file_proc *proc, void *client_data)
{
    if (fd >= event_loop->setsize) {
        errno = ERANGE;
        return EV_ERR;
    }
    ev_file_event *fe = &event_loop->events[fd];

    if (ev_api_add_event(event_loop, fd, mask) == -1)
        return EV_ERR;
    fe->mask |= mask;
    if (mask & EV_READABLE) fe->rfile_proc = proc;
    if (mask & EV_WRITABLE) fe->wfile_proc = proc;
    fe->client_data = client_data;
    if (fd > event_loop->maxfd)
        event_loop->maxfd = fd;
    return EV_OK;
}

void ev_delete_file_event(ev_event_loop *event_loop, int fd, int mask)
{
    if (fd >= event_loop->setsize) return;
    ev_file_event *fe = &event_loop->events[fd];

    if (fe->mask == EV_NONE) return;
    fe->mask = fe->mask & (~mask);
    if (fd == event_loop->maxfd && fe->mask == EV_NONE) {
        /* Update the max fd */
        int j;

        for (j = event_loop->maxfd-1; j >= 0; j--)
            if (event_loop->events[j].mask != EV_NONE) break;
        event_loop->maxfd = j;
    }
    ev_api_del_event(event_loop, fd, mask);
}

int ev_get_file_events(ev_event_loop *event_loop, int fd) {
    if (fd >= event_loop->setsize) return 0;
    ev_file_event *fe = &event_loop->events[fd];

    return fe->mask;
}

static void ev_get_time(long *seconds, long *milliseconds)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec/1000;
}

static void ev_add_milliseconds_to_now(long long milliseconds, long *sec, long *ms) {
    long cur_sec, cur_ms, when_sec, when_ms;

    ev_get_time(&cur_sec, &cur_ms);
    when_sec = cur_sec + milliseconds/1000;
    when_ms = cur_ms + milliseconds%1000;
    if (when_ms >= 1000) {
        when_sec ++;
        when_ms -= 1000;
    }
    *sec = when_sec;
    *ms = when_ms;
}

long long ev_create_time_event(ev_event_loop *event_loop, long long milliseconds,
        ev_time_proc *proc, void *client_data,
        ev_event_finalizer_proc *finalizer_proc)
{
    long long id = event_loop->time_event_next_id++;
    ev_time_event *te;

    te = malloc(sizeof(*te));
    if (te == NULL) return EV_ERR;
    te->id = id;
    ev_add_milliseconds_to_now(milliseconds,&te->when_sec,&te->when_ms);
    te->time_proc = proc;
    te->finalizer_proc = finalizer_proc;
    te->client_data = client_data;
    te->next = event_loop->time_event_head;
    event_loop->time_event_head = te;
    return id;
}

int ev_delete_time_event(ev_event_loop *event_loop, long long id)
{
    ev_time_event *te, *prev = NULL;

    te = event_loop->time_event_head;
    while(te) {
        if (te->id == id) {
            if (prev == NULL)
                event_loop->time_event_head = te->next;
            else
                prev->next = te->next;
            if (te->finalizer_proc)
                te->finalizer_proc(event_loop, te->client_data);
            free(te);
            return EV_OK;
        }
        prev = te;
        te = te->next;
    }
    return EV_ERR; /* NO event with the specified ID found */
}

/* Search the first timer to fire.
 * This operation is useful to know how many time the select can be
 * put in sleep without to delay any event.
 * If there are no timers NULL is returned.
 *
 * Note that's O(N) since time events are unsorted.
 * Possible optimizations (not needed by Redis so far, but...):
 * 1) Insert the event in order, so that the nearest is just the head.
 *    Much better but still insertion or deletion of timers is O(N).
 * 2) Use a skiplist to have this operation as O(1) and insertion as O(log(N)).
 */
static ev_time_event *ev_search_nearest_timer(ev_event_loop *event_loop)
{
    ev_time_event *te = event_loop->time_event_head;
    ev_time_event *nearest = NULL;

    while(te) {
        if (!nearest || te->when_sec < nearest->when_sec ||
                (te->when_sec == nearest->when_sec &&
                 te->when_ms < nearest->when_ms))
            nearest = te;
        te = te->next;
    }
    return nearest;
}

/* Process time events */
static int process_time_events(ev_event_loop *event_loop) {
    int processed = 0;
    ev_time_event *te;
    long long max_id;
    time_t now = time(NULL);

    /* If the system clock is moved to the future, and then set back to the
     * right value, time events may be delayed in a random way. Often this
     * means that scheduled operations will not be performed soon enough.
     *
     * Here we try to detect system clock skews, and force all the time
     * events to be processed ASAP when this happens: the idea is that
     * processing events earlier is less dangerous than delaying them
     * indefinitely, and practice suggests it is. */
    if (now < event_loop->last_time) {
        te = event_loop->time_event_head;
        while(te) {
            te->when_sec = 0;
            te = te->next;
        }
    }
    event_loop->last_time = now;

    te = event_loop->time_event_head;
    max_id = event_loop->time_event_next_id-1;
    while(te) {
        long now_sec, now_ms;
        long long id;

        if (te->id > max_id) {
            te = te->next;
            continue;
        }
        ev_get_time(&now_sec, &now_ms);
        if (now_sec > te->when_sec ||
            (now_sec == te->when_sec && now_ms >= te->when_ms))
        {
            int retval;

            id = te->id;
            retval = te->time_proc(event_loop, id, te->client_data);
            processed++;
            /* After an event is processed our time event list may
             * no longer be the same, so we restart from head.
             * Still we make sure to don't process events registered
             * by event handlers itself in order to don't loop forever.
             * To do so we saved the max ID we want to handle.
             *
             * FUTURE OPTIMIZATIONS:
             * Note that this is NOT great algorithmically. Redis uses
             * a single time event so it's not a problem but the right
             * way to do this is to add the new elements on head, and
             * to flag deleted elements in a special way for later
             * deletion (putting references to the nodes to delete into
             * another linked list). */
            if (retval != EV_NOMORE) {
                ev_add_milliseconds_to_now(retval,&te->when_sec,&te->when_ms);
            } else {
                ev_delete_time_event(event_loop, id);
            }
            te = event_loop->time_event_head;
        } else {
            te = te->next;
        }
    }
    return processed;
}

/* Process every pending time event, then every pending file event
 * (that may be registered by time event callbacks just processed).
 * Without special flags the function sleeps until some file event
 * fires, or when the next time event occurs (if any).
 *
 * If flags is 0, the function does nothing and returns.
 * if flags has EV_ALL_EVENTS set, all the kind of events are processed.
 * if flags has EV_FILE_EVENTS set, file events are processed.
 * if flags has EV_TIME_EVENTS set, time events are processed.
 * if flags has EV_DONT_WAIT set the function returns ASAP until all
 * the events that's possible to process without to wait are processed.
 *
 * The function returns the number of events processed. */
int ev_process_events(ev_event_loop *event_loop, int flags)
{
    int processed = 0, numevents;

    /* Nothing to do? return ASAP */
    if (!(flags & EV_TIME_EVENTS) && !(flags & EV_FILE_EVENTS)) return 0;

    /* Note that we want call select() even if there are no
     * file events to process as long as we want to process time
     * events, in order to sleep until the next time event is ready
     * to fire. */
    if (event_loop->maxfd != -1 ||
        ((flags & EV_TIME_EVENTS) && !(flags & EV_DONT_WAIT))) {
        int j;
        ev_time_event *shortest = NULL;
        struct timeval tv, *tvp;

        if (flags & EV_TIME_EVENTS && !(flags & EV_DONT_WAIT))
            shortest = ev_search_nearest_timer(event_loop);
        if (shortest) {
            long now_sec, now_ms;

            /* Calculate the time missing for the nearest
             * timer to fire. */
            ev_get_time(&now_sec, &now_ms);
            tvp = &tv;
            tvp->tv_sec = shortest->when_sec - now_sec;
            if (shortest->when_ms < now_ms) {
                tvp->tv_usec = ((shortest->when_ms+1000) - now_ms)*1000;
                tvp->tv_sec --;
            } else {
                tvp->tv_usec = (shortest->when_ms - now_ms)*1000;
            }
            if (tvp->tv_sec < 0) tvp->tv_sec = 0;
            if (tvp->tv_usec < 0) tvp->tv_usec = 0;
        } else {
            /* If we have to check for events but need to return
             * ASAP because of EV_DONT_WAIT we need to set the timeout
             * to zero */
            if (flags & EV_DONT_WAIT) {
                tv.tv_sec = tv.tv_usec = 0;
                tvp = &tv;
            } else {
                /* Otherwise we can block */
                tvp = NULL; /* wait forever */
            }
        }

        numevents = ev_api_poll(event_loop, tvp);
        for (j = 0; j < numevents; j++) {
            ev_file_event *fe = &event_loop->events[event_loop->fired[j].fd];
            int mask = event_loop->fired[j].mask;
            int fd = event_loop->fired[j].fd;
            int rfired = 0;

	    /* note the fe->mask & mask & ... code: maybe an already processed
             * event removed an element that fired and we still didn't
             * processed, so we check if the event is still valid. */
            if (fe->mask & mask & EV_READABLE) {
                rfired = 1;
                fe->rfile_proc(event_loop,fd,fe->client_data,mask);
            }
            if (fe->mask & mask & EV_WRITABLE) {
                if (!rfired || fe->wfile_proc != fe->rfile_proc)
                    fe->wfile_proc(event_loop,fd,fe->client_data,mask);
            }
            processed++;
        }
    }
    /* Check time events */
    if (flags & EV_TIME_EVENTS)
        processed += process_time_events(event_loop);

    return processed; /* return the number of processed file/time events */
}

/* Wait for milliseconds until the given file descriptor becomes
 * writable/readable/exception */
int ev_wait(int fd, int mask, long long milliseconds) {
    struct pollfd pfd;
    int retmask = 0, retval;

    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fd;
    if (mask & EV_READABLE) pfd.events |= POLLIN;
    if (mask & EV_WRITABLE) pfd.events |= POLLOUT;

    if ((retval = poll(&pfd, 1, milliseconds))== 1) {
        if (pfd.revents & POLLIN) retmask |= EV_READABLE;
        if (pfd.revents & POLLOUT) retmask |= EV_WRITABLE;
	if (pfd.revents & POLLERR) retmask |= EV_WRITABLE;
        if (pfd.revents & POLLHUP) retmask |= EV_WRITABLE;
        return retmask;
    } else {
        return retval;
    }
}

void ev_main(ev_event_loop *event_loop) {
    event_loop->stop = 0;
    while (!event_loop->stop) {
        if (event_loop->beforesleep != NULL)
            event_loop->beforesleep(event_loop);
        ev_process_events(event_loop, EV_ALL_EVENTS);
    }
}

char *ev_get_api_name(void) {
    return ev_api_name();
}

void ev_set_before_sleep_proc(ev_event_loop *event_loop, ev_before_sleep_proc *beforesleep) {
    event_loop->beforesleep = beforesleep;
}
