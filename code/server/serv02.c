#include "unp.h"

static int nchildren;
static pid_t* pids;

void
child_main(int i, int listenfd, int addrlen)
{
    int connfd;
    void web_child(int);
    socklen_t clilen;
    struct sockaddr* cliaddr;

    cliaddr = malloc(addrlen);
    printf("child %ld starting\n", (long) getpid());
    for ( ; ; ) {
        clilen = addrlen;
        connfd = accept(listenfd, cliaddr, &clilen);
        web_child(connfd); /* process the request */
        close(connfd);
    }
}

void
sig_int(int signo)
{
    int i;
    void pr_cpu_time(void);

    /* terminate all children */
    for (i = 0; i < nchildren; i++)
        kill(pids[i], SIGTERM);
    while (wait(NULL) > 0) /* wait for all children */
        ;
    if (errno != ECHILD)
        err_sys("wait error");

    pr_cpu_time();
    exit(0);
}

pid_t
child_make(int i, int listenfd, int addrlen)
{
    pid_t pid;
    void child_main(int, int, int);

    if ((pid = fork()) > 0)
        return (pid); /* parentt */
    child_main(i, listenfd, addrlen); /* never returns */
}

int
main(int argc, char** argv)
{
    int listenfd, i;
    socklen_t addrlen;
    void sig_int(int);
    pid_t child_make(int, int, int);

    if (argc == 3)
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 4)
        listenfd = tcp_listen(argv[1], argv[2], &addrlen);
    else
        err_quit("usage: serv02 [ <host> ] <port#> <#children>");
    nchildren = atoi(argv[argc - 1]);
    pids = calloc(nchildren, sizeof(pid_t));
    for (i = 0; i < nchildren; i++)
        pids[i] = child_make(i, listenfd, addrlen); /* parent returns */
    signal(SIGINT, sig_int);

    for( ; ; )
        pause(); /* everything done by children */
}
