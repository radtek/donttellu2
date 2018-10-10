#ifndef __EV_H__
#define __EV_H__
#include <time.h>
#include <stdlib.h>

#define EV_OK 0
#define EV_ERR -1

#define EV_NONE 0
#define EV_READABLE 1
#define EV_WRITABLE 2

#define EV_FILE_EVENTS 1
#define EV_TIME_EVENTS 2
#define EV_ALL_EVENTS (EV_FILE_EVENTS|EV_TIME_EVENTS)
#define EV_DONT_WAIT 4

#define EV_NOMORE -1

/* Macros */
#define EV_NOTUSED(V) ((void) V)

struct ev_event_loop;

/* Types and data structures */
typedef void ev_file_proc(struct ev_event_loop *event_loop, int fd, void *client_data, int mask);
typedef int ev_time_proc(struct ev_event_loop *event_loop, long long id, void *client_data);
typedef void ev_event_finalizer_proc(struct ev_event_loop *event_loop, void *client_data);
typedef void ev_before_sleep_proc(struct ev_event_loop *event_loop);

/* File event structure */
typedef struct ev_file_event {
    int mask; /* one of EV_(READABLE|WRITABLE) */
    ev_file_proc *rfile_proc;
    ev_file_proc *wfile_proc;
    void *client_data;
} ev_file_event;

/* Time event structure */
typedef struct ev_time_event {
    long long id; /* time event identifier. */
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */
    ev_time_proc *time_proc;
    ev_event_finalizer_proc *finalizer_proc;
    void *client_data;
    struct ev_time_event *next;
} ev_time_event;

/* A fired event */
typedef struct ev_fired_event {
    int fd;
    int mask;
} ev_fired_event;

/* State of an event based program */
typedef struct ev_event_loop {
    int maxfd;   /* highest file descriptor currently registered */
    int setsize; /* max number of file descriptors tracked */
    long long time_event_next_id;
    time_t last_time;     /* Used to detect system clock skew */
    ev_file_event *events; /* Registered events */
    ev_fired_event *fired; /* Fired events */
    ev_time_event *time_event_head;
    int stop;
    void *apidata; /* This is used for polling API specific data */
    ev_before_sleep_proc *beforesleep;
} ev_event_loop;

/* Prototypes */
ev_event_loop *ev_create_event_loop(int setsize);
void ev_delete_event_loop(ev_event_loop *event_loop);
void ev_stop(ev_event_loop *event_loop);
int ev_create_file_event(ev_event_loop *event_loop, int fd, int mask,
        ev_file_proc *proc, void *client_data);
void ev_delete_file_event(ev_event_loop *event_loop, int fd, int mask);
int ev_get_file_events(ev_event_loop *event_loop, int fd);
long long ev_create_time_event(ev_event_loop *event_loop, long long milliseconds,
        ev_time_proc *proc, void *client_data,
        ev_event_finalizer_proc *finalizer_proc);
int ev_delete_time_event(ev_event_loop *event_loop, long long id);
int ev_process_events(ev_event_loop *event_loop, int flags);
int ev_wait(int fd, int mask, long long milliseconds);
void ev_main(ev_event_loop *event_loop);
char *ev_get_api_name(void);
void ev_set_before_sleep_proc(ev_event_loop *event_loop, ev_before_sleep_proc *beforesleep);
int ev_get_setsize(ev_event_loop *event_loop);
int ev_resize_setsize(ev_event_loop *event_loop, int setsize);

#endif
