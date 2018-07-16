#include "unp.h"
#include <sys/epoll.h>

#define IPADDRESS "127.0.0.1"
#define FDSIZE 1 /* ignored by epoll_create() but must be greater than 0 */
#define EPOLLEVENTS 100

static int tcp_listen();
static void do_epoll(int listenfd);
static void handle_events(int epollfd, struct epoll_event*, int num, int listenfd, char*);
static void handle_accept(int epollfd, int listenfd);
static void do_read(int epollfd, int fd, char* buf);
static void do_write(int epollfd, int fd, char* buf);

/* add an event */
static void add_event(int epollfd, int fd, int state);
/* modify an event */
static void modify_event(int epollfd, int fd, int state);
/* delete an event */
static void delete_event(int epollfd, int fd, int state);

int main(int argc, char* argv[])
{
    int listenfd;
    int port;

    listenfd = tcp_listen();
    do_epoll(listenfd);

    return 0;
}

static int 
tcp_listen()
{
    int listenfd;
    struct sockaddr_in servaddr;
    
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        err_quit("socket error");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT); /* echo server */
    if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) == -1)
        err_sys("bind error");
    listen(listenfd, LISTENQ);

    return listenfd;
}

static void 
do_epoll(int listenfd)
{
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int nfd;
    char buf[MAXLINE];
    
    memset(buf, 0, MAXLINE);
    epollfd = epoll_create(FDSIZE);
    add_event(epollfd, listenfd, EPOLLIN);
    for ( ; ; ) {
        if ((nfd = epoll_wait(epollfd, events, EPOLLEVENTS, -1)) == -1)
            err_sys("epoll_wait failed");
        handle_events(epollfd, events, nfd, listenfd, buf);
    }

    close(epollfd);
}

static void
handle_events(int epollfd, struct epoll_event* events, int nfd, int listenfd, char* buf)
{
    int i;
    int fd;

    for (i = 0; i < nfd; i++) {
        fd = events[i].data.fd;
        if ((fd == listenfd) && (events[i].events & EPOLLIN))
            handle_accept(epollfd, listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd, fd, buf);
        else if (events[i].events & EPOLLOUT)
            do_write(epollfd, fd, buf);
    }
}

static void handle_accept(int epollfd, int listenfd)
{
    int confd;
    struct sockaddr_in cliaddr;
    socklen_t clilen;

    clilen = sizeof(cliaddr);
    if ((confd = accept(listenfd, (SA *)&cliaddr, &clilen)) == -1)
        err_sys("accept failed");
    printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
    add_event(epollfd, confd, EPOLLIN);
}

static void 
do_read(int epollfd, int fd, char* buf)
{
    int nread = MAXLINE;

reread:
    if ((nread = read(fd, buf, MAXLINE)) == -1) {
        if (errno == EINTR)
            goto reread;
        else {
            err_sys("read error");
            close(fd);
            delete_event(epollfd, fd, EPOLLIN);
        }
    } else if (nread == 0) {
        fprintf(stderr, "client close.");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    } else {
        printf("read message is : %s\n", buf);
        modify_event(epollfd, fd, EPOLLOUT);
    }
}

static void 
do_write(int epollfd, int fd, char* buf)
{
    int nwrite;
    
rewrite:
    if ((nwrite = write(fd, buf, strlen(buf))) == -1) {
        if (errno == EINTR)
            goto rewrite;
        else {
        err_sys("write error");
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
        }
    } else {
        modify_event(epollfd, fd, EPOLLIN);
        memset(buf, 0, MAXLINE);
    }
}

static void 
add_event(int epollfd, int fd, int event)
{
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

static void 
delete_event(int epollfd, int fd, int event)
{
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

static void 
modify_event(int epollfd, int fd, int event)
{
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}
