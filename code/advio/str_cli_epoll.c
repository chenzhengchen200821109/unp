#include <sys/epoll.h>
#include "unp.h"

#define MAX_EVENTS 10

void str_cli(FILE* fp, int sockfd)
{
	int stdineof;
    char buf[MAXLINE];
    int n;
    int wfd;
    struct epoll_event ev, events[2];
    int epollfd;
    int i;
    int nfds;

    /* epoll_create */
    if ((epollfd = epoll_create(1)) == -1)
        err_sys("epoll_create error");
    /* epoll_ctl */
    events[0].events = EPOLLIN;
    events[0].data.fd = fileno(fp);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fileno(fp), &events[0]) == -1)
        err_sys("epoll_ctl error");
    events[1].events = EPOLLIN;
    events[1].data.fd = sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &events[1]) == -1)
        err_sys("epoll_ctl error");

	for ( ; ; ) {
        if ((nfds = epoll_wait(epollfd, events, 2, -1)) == -1)
            err_sys("epoll_wait error");
        
        for (i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd) { /* socket is readable */
                if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                    if (stdineof == 1) {
                        close(epollfd); /* close epollfd for kernel to release resources */
                        return; /* normal termination */
                    }
                    else
                        err_quit("str_cli: server terminated prematurely");
                }
                write(fileno(stdout), buf, n);
            }

            if (events[i].data.fd = fileno(fp)) { /* input is readable */
                n = read(fileno(fp), buf, MAXLINE);
                if (n > 0)
                    writen(sockfd, buf, n);
                if (n == 0) {
                    stdineof == 1;
                    shutdown(sockfd, SHUT_WR); /* send FIN */
                    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fileno(fp), events) == -1)
                        err_sys("epoll_ctl error");
                    continue;
                }
            }
        }
	}
}
