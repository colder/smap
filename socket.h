#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int socket_connect(const char *host, in_port_t port);
void socket_close(int fd);
int socket_read_line(int fd, char *dst, int max_len);
int socket_nwrite(int fd, const char *msg, int length);

#define socket_write(fd, msg) \
	socket_nwrite(fd, msg, strlen(msg))

#endif /* SOCKET_H */
