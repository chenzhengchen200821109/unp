#include "unp.h"
#include <time.h>

extern int daemon_init(const char *, int);
extern int tcp_listen(const char *, const char *, socklen_t *);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t addrlen, len;
    struct sockaddr *cliaddr;
    char buff[MAXLINE];
    time_t ticks;

    if (argc < 2 || argc > 3)
        err_quit("usage: daytimetcpserv2 [ <host> ] <service or port>");

    daemon_init(argv[0], 0);

    if (argc == 2)
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    else
        listenfd = tcp_listen(argv[1], argv[2], &addrlen);

    cliaddr = malloc(addrlen);

    for ( ; ; ) {
        len = addrlen;
        connfd = accept(listenfd, cliaddr, &len);
        err_msg("connection from %s", sock_ntop(cliaddr, len));

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }
}
