#include "client.h"
#include "socket.h"

int socket_connect(const char *host, in_port_t port){
        struct hostent *hp;
        struct sockaddr_in addr;
        int on = 1, sock;     

        if((hp = gethostbyname(host)) == NULL){
                herror("gethostbyname");
                exit(1);
        }
        bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
        addr.sin_port = htons(port);
        addr.sin_family = AF_INET;
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
        if(sock == -1){
                perror("setsockopt");
                exit(1);
        }
        if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
                perror("connect");
                exit(1);
        }
        return sock;
}

int socket_nwrite(int fd, const char *msg, int length) {
	debug_out(msg);
	write(fd, msg, length);
	return SUCCESS;
}

void socket_close(int fd) {
	shutdown(fd, SHUT_RDWR); 
	close(fd); 
}

int socket_read_line(int fd, char *dst, int max_len) {
	int n = 0;
	char c;
	bzero(dst, max_len);

	while(n < max_len && read(fd, &c, 1) != 0){
		dst[n] = c;
		n++;
		if (c == '\n') {
			break;
		}
	}
	dst[n] = '\0';

	debug_in(dst);

	return n;
}
