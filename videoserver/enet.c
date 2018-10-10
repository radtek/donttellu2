//#include "fmacros.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "enet.h"

static void enet_set_error(char *err, const char *fmt, ...)
{
    va_list ap;

    if (!err) return;
    va_start(ap, fmt);
    vsnprintf(err, ENET_ERR_LEN, fmt, ap);
    va_end(ap);
}

int enet_non_block(char *err, int fd)
{
    int flags;

    /* Set the socket non-blocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        enet_set_error(err, "fcntl(F_GETFL): %s", strerror(errno));
        return ENET_ERR;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        enet_set_error(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return ENET_ERR;
    }
    return ENET_OK;
}

/* Set TCP keep alive option to detect dead peers. The interval option
 * is only used for Linux as we are using Linux-specific APIs to set
 * the probe send time, interval, and count. */
int enet_keep_alive(char *err, int fd, int interval)
{
    int val = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1)
    {
        enet_set_error(err, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
        return ENET_ERR;
    }

#ifdef __linux__
    /* Default settings are more or less garbage, with the keepalive time
     * set to 7200 by default on Linux. Modify settings to make the feature
     * actually useful. */

    /* Send first probe after interval. */
    val = interval;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
        enet_set_error(err, "setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));
        return ENET_ERR;
    }

    /* Send next probes after the specified interval. Note that we set the
     * delay as interval / 3, as we send three probes before detecting
     * an error (see the next setsockopt call). */
    val = interval/3;
    if (val == 0) val = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
        enet_set_error(err, "setsockopt TCP_KEEPINTVL: %s\n", strerror(errno));
        return ENET_ERR;
    }

    /* Consider the socket in error state after three we send three ACK
     * probes without getting a reply. */
    val = 3;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
        enet_set_error(err, "setsockopt TCP_KEEPCNT: %s\n", strerror(errno));
        return ENET_ERR;
    }
#endif

    return ENET_OK;
}

static int enet_set_tcp_no_delay(char *err, int fd, int val)
{
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == -1)
    {
        enet_set_error(err, "setsockopt TCP_NODELAY: %s", strerror(errno));
        return ENET_ERR;
    }
    return ENET_OK;
}

int enet_enable_tcp_no_delay(char *err, int fd)
{
    return enet_set_tcp_no_delay(err, fd, 1);
}

int enet_disable_tcp_no_delay(char *err, int fd)
{
    return enet_set_tcp_no_delay(err, fd, 0);
}


int enet_set_send_buffer(char *err, int fd, int buffsize)
{
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buffsize, sizeof(buffsize)) == -1)
    {
        enet_set_error(err, "setsockopt SO_SNDBUF: %s", strerror(errno));
        return ENET_ERR;
    }
    return ENET_OK;
}

int enet_tcp_keep_alive(char *err, int fd)
{
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes)) == -1) {
        enet_set_error(err, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
        return ENET_ERR;
    }
    return ENET_OK;
}

/* enet_generic_resolve() is called by enet_resolve() and enet_resolve_iP() to
 * do the actual work. It resolves the hostname "host" and set the string
 * representation of the IP address into the buffer pointed by "ipbuf".
 *
 * If flags is set to ENET_IP_ONLY the function only resolves hostnames
 * that are actually already IPv4 or IPv6 addresses. This turns the function
 * into a validating / normalizing function. */
int enet_generic_resolve(char *err, char *host, char *ipbuf, size_t ipbuf_len,
                       int flags)
{
    struct addrinfo hints, *info;
    int rv;

    memset(&hints,0,sizeof(hints));
    if (flags & ENET_IP_ONLY) hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;  /* specify socktype to avoid dups */

    if ((rv = getaddrinfo(host, NULL, &hints, &info)) != 0) {
        enet_set_error(err, "%s", gai_strerror(rv));
        return ENET_ERR;
    }
    if (info->ai_family == AF_INET) {
        struct sockaddr_in *sa = (struct sockaddr_in *)info->ai_addr;
        inet_ntop(AF_INET, &(sa->sin_addr), ipbuf, ipbuf_len);
    } else {
        struct sockaddr_in6 *sa = (struct sockaddr_in6 *)info->ai_addr;
        inet_ntop(AF_INET6, &(sa->sin6_addr), ipbuf, ipbuf_len);
    }

    freeaddrinfo(info);
    return ENET_OK;
}

int enet_resolve(char *err, char *host, char *ipbuf, size_t ipbuf_len) {
    return enet_generic_resolve(err,host,ipbuf,ipbuf_len,ENET_NONE);
}

int enet_resolve_iP(char *err, char *host, char *ipbuf, size_t ipbuf_len) {
    return enet_generic_resolve(err,host,ipbuf,ipbuf_len,ENET_IP_ONLY);
}

static int enet_set_reuse_addr(char *err, int fd) {
    int yes = 1;
    /* Make sure connection-intensive things like the redis benckmark
     * will be able to close/open sockets a zillion of times */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        enet_set_error(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return ENET_ERR;
    }
    return ENET_OK;
}

static int enet_create_socket(char *err, int domain) {
    int s;
    if ((s = socket(domain, SOCK_STREAM, 0)) == -1) {
        enet_set_error(err, "creating socket: %s", strerror(errno));
        return ENET_ERR;
    }

    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (enet_set_reuse_addr(err,s) == ENET_ERR) {
        close(s);
        return ENET_ERR;
    }
    return s;
}

#define ENET_CONNECT_NONE 0
#define ENET_CONNECT_NONBLOCK 1
static int enet_tcp_generic_connect(char *err, char *addr, int port,
                                 char *source_addr, int flags)
{
    int s = ENET_ERR, rv;
    char portstr[6];  /* strlen("65535") + 1; */
    struct addrinfo hints, *servinfo, *bservinfo, *p, *b;

    snprintf(portstr,sizeof(portstr),"%d",port);
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(addr,portstr,&hints,&servinfo)) != 0) {
        enet_set_error(err, "%s", gai_strerror(rv));
        return ENET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Try to create the socket and to connect it.
         * If we fail in the socket() call, or on connect(), we retry with
         * the next entry in servinfo. */
        if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
            continue;
        if (enet_set_reuse_addr(err,s) == ENET_ERR) goto error;
        if (flags & ENET_CONNECT_NONBLOCK && enet_non_block(err,s) != ENET_OK)
            goto error;
        if (source_addr) {
            int bound = 0;
            /* Using getaddrinfo saves us from self-determining IPv4 vs IPv6 */
            if ((rv = getaddrinfo(source_addr, NULL, &hints, &bservinfo)) != 0) {
                enet_set_error(err, "%s", gai_strerror(rv));
                goto end;
            }
            for (b = bservinfo; b != NULL; b = b->ai_next) {
                if (bind(s,b->ai_addr,b->ai_addrlen) != -1) {
                    bound = 1;
                    break;
                }
            }
            if (!bound) {
                enet_set_error(err, "bind: %s", strerror(errno));
                goto end;
            }
        }
        if (connect(s,p->ai_addr,p->ai_addrlen) == -1) {
            /* If the socket is non-blocking, it is ok for connect() to
             * return an EINPROGRESS error here. */
            if (errno == EINPROGRESS && flags & ENET_CONNECT_NONBLOCK)
                goto end;
            close(s);
            s = ENET_ERR;
            continue;
        }

        /* If we ended an iteration of the for loop without errors, we
         * have a connected socket. Let's return to the caller. */
        goto end;
    }
    if (p == NULL)
        enet_set_error(err, "creating socket: %s", strerror(errno));

error:
    if (s != ENET_ERR) {
        close(s);
        s = ENET_ERR;
    }
end:
    freeaddrinfo(servinfo);
    return s;
}

int enet_tcp_connect(char *err, char *addr, int port)
{
    return enet_tcp_generic_connect(err,addr,port,NULL,ENET_CONNECT_NONE);
}

int enet_tcp_non_block_connect(char *err, char *addr, int port)
{
    return enet_tcp_generic_connect(err,addr,port,NULL,ENET_CONNECT_NONBLOCK);
}

int enet_tcp_non_block_bind_connect(char *err, char *addr, int port, char *source_addr)
{
    return enet_tcp_generic_connect(err,addr,port,source_addr,ENET_CONNECT_NONBLOCK);
}

int enet_unix_generic_connect(char *err, char *path, int flags)
{
    int s;
    struct sockaddr_un sa;

    if ((s = enet_create_socket(err,AF_LOCAL)) == ENET_ERR)
        return ENET_ERR;

    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path,path,sizeof(sa.sun_path)-1);
    if (flags & ENET_CONNECT_NONBLOCK) {
        if (enet_non_block(err,s) != ENET_OK)
            return ENET_ERR;
    }
    if (connect(s,(struct sockaddr*)&sa,sizeof(sa)) == -1) {
        if (errno == EINPROGRESS &&
            flags & ENET_CONNECT_NONBLOCK)
            return s;

        enet_set_error(err, "connect: %s", strerror(errno));
        close(s);
        return ENET_ERR;
    }
    return s;
}

int enet_unix_connect(char *err, char *path)
{
    return enet_unix_generic_connect(err,path,ENET_CONNECT_NONE);
}

int enet_unix_non_block_connect(char *err, char *path)
{
    return enet_unix_generic_connect(err,path,ENET_CONNECT_NONBLOCK);
}

/* Like read(2) but make sure 'count' is read before to return
 * (unless error or EOF condition is encountered) */
int enet_read(int fd, char *buf, int count)
{
    int nread, totlen = 0;
    while(totlen != count) {
        nread = read(fd,buf,count-totlen);
        if (nread == 0) return totlen;
        if (nread == -1) return -1;
        totlen += nread;
        buf += nread;
    }
    return totlen;
}

/* Like write(2) but make sure 'count' is read before to return
 * (unless error is encountered) */
int enet_write(int fd, char *buf, int count)
{
    int nwritten, totlen = 0;
    while(totlen != count) {
        nwritten = write(fd,buf,count-totlen);
        if (nwritten == 0) return totlen;
        if (nwritten == -1) return -1;
        totlen += nwritten;
        buf += nwritten;
    }
    return totlen;
}

static int enet_listen(char *err, int s, struct sockaddr *sa, socklen_t len, int backlog) {
    if (bind(s,sa,len) == -1) {
        enet_set_error(err, "bind: %s", strerror(errno));
        close(s);
        return ENET_ERR;
    }

    if (listen(s, backlog) == -1) {
        enet_set_error(err, "listen: %s", strerror(errno));
        close(s);
        return ENET_ERR;
    }
    return ENET_OK;
}

static int enet_v6Only(char *err, int s) {
    int yes = 1;
    if (setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,&yes,sizeof(yes)) == -1) {
        enet_set_error(err, "setsockopt: %s", strerror(errno));
        close(s);
        return ENET_ERR;
    }
    return ENET_OK;
}

static int _enet_tcp_server(char *err, int port, char *bindaddr, int af, int backlog)
{
    int s, rv;
    char _port[6];  /* strlen("65535") */
    struct addrinfo hints, *servinfo, *p;

    snprintf(_port,6,"%d",port);
    memset(&hints,0,sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;    /* No effect if bindaddr != NULL */

    if ((rv = getaddrinfo(bindaddr,_port,&hints,&servinfo)) != 0) {
        enet_set_error(err, "%s", gai_strerror(rv));
        return ENET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((s = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
            continue;

        if (af == AF_INET6 && enet_v6Only(err,s) == ENET_ERR) goto error;
        if (enet_set_reuse_addr(err,s) == ENET_ERR) goto error;
        if (enet_listen(err,s,p->ai_addr,p->ai_addrlen,backlog) == ENET_ERR) goto error;
        goto end;
    }
    if (p == NULL) {
        enet_set_error(err, "unable to bind socket");
        goto error;
    }

error:
    s = ENET_ERR;
end:
    freeaddrinfo(servinfo);
    return s;
}

int enet_tcp_server(char *err, int port, char *bindaddr, int backlog)
{
    return _enet_tcp_server(err, port, bindaddr, AF_INET, backlog);
}

int enet_tcp6Server(char *err, int port, char *bindaddr, int backlog)
{
    return _enet_tcp_server(err, port, bindaddr, AF_INET6, backlog);
}

int enet_unix_server(char *err, char *path, mode_t perm, int backlog)
{
    int s;
    struct sockaddr_un sa;

    if ((s = enet_create_socket(err,AF_LOCAL)) == ENET_ERR)
        return ENET_ERR;

    memset(&sa,0,sizeof(sa));
    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path,path,sizeof(sa.sun_path)-1);
    if (enet_listen(err,s,(struct sockaddr*)&sa,sizeof(sa),backlog) == ENET_ERR)
        return ENET_ERR;
    if (perm)
        chmod(sa.sun_path, perm);
    return s;
}

static int enet_generic_accept(char *err, int s, struct sockaddr *sa, socklen_t *len) {
    int fd;
    while(1) {
        fd = accept(s,sa,len);
        if (fd == -1) {
            if (errno == EINTR)
                continue;
            else {
                enet_set_error(err, "accept: %s", strerror(errno));
                return ENET_ERR;
            }
        }
        break;
    }
    return fd;
}

int enet_tcp_accept(char *err, int s, char *ip, size_t ip_len, int *port) {
    int fd;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    if ((fd = enet_generic_accept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return ENET_ERR;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return fd;
}

int enet_unix_accept(char *err, int s) {
    int fd;
    struct sockaddr_un sa;
    socklen_t salen = sizeof(sa);
    if ((fd = enet_generic_accept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return ENET_ERR;

    return fd;
}

int enet_peer_to_string(int fd, char *ip, size_t ip_len, int *port) {
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getpeername(fd,(struct sockaddr*)&sa,&salen) == -1) {
        if (port) *port = 0;
        ip[0] = '?';
        ip[1] = '\0';
        return -1;
    }
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return 0;
}

int enet_sock_name(int fd, char *ip, size_t ip_len, int *port) {
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getsockname(fd,(struct sockaddr*)&sa,&salen) == -1) {
        if (port) *port = 0;
        ip[0] = '?';
        ip[1] = '\0';
        return -1;
    }
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return 0;
}
