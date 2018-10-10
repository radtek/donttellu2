#ifndef ENET_H
#define ENET_H

#define ENET_OK 0
#define ENET_ERR -1
#define ENET_ERR_LEN 256

/* Flags used with certain functions. */
#define ENET_NONE 0
#define ENET_IP_ONLY (1<<0)

#if defined(__sun)
#define AF_LOCAL AF_UNIX
#endif

int enet_tcp_connect(char *err, char *addr, int port);
int enet_tcp_non_block_connect(char *err, char *addr, int port);
int enet_tcp_non_block_bind_connect(char *err, char *addr, int port, char *source_addr);
int enet_unix_connect(char *err, char *path);
int enet_unix_non_block_connect(char *err, char *path);
int enet_read(int fd, char *buf, int count);
int enet_resolve(char *err, char *host, char *ipbuf, size_t ipbuf_len);
int enet_resolve_iP(char *err, char *host, char *ipbuf, size_t ipbuf_len);
int enet_tcp_server(char *err, int port, char *bindaddr, int backlog);
int enet_tcp6Server(char *err, int port, char *bindaddr, int backlog);
int enet_unix_server(char *err, char *path, mode_t perm, int backlog);
int enet_tcp_accept(char *err, int serversock, char *ip, size_t ip_len, int *port);
int enet_unix_accept(char *err, int serversock);
int enet_write(int fd, char *buf, int count);
int enet_non_block(char *err, int fd);
int enet_enable_tcp_no_delay(char *err, int fd);
int enet_disable_tcp_no_delay(char *err, int fd);
int enet_tcp_keep_alive(char *err, int fd);
int enet_peer_to_string(int fd, char *ip, size_t ip_len, int *port);
int enet_keep_alive(char *err, int fd, int interval);
int enet_sock_name(int fd, char *ip, size_t ip_len, int *port);

#endif
