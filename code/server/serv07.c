#include "unpthread.h"
#include "pthread07.h"

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
    int i;
    void sig_init(int), thread_make(int);

    if (argc == 3)
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    else if (argc == 4)
        listenfd = tcp_listen(argv[1], argv[2], &addrlen);
    else
        err_quit("usage: serv07 [ <host> ] <port#> <#threads> ");
    nthreads = atoi(argv[argc - 1]);
    tptr = calloc(nthreads, sizeof(Thread));
    for (i = 0; i < nthreadss; i++) 
        thread_make(i); /* only main thread returns */
    signal(SIGINT, sig_int);
    for ( ; ; )
        pause(); /* everything done by threads */
}
