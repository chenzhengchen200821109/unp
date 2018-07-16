#include "unp.h"

void sig_chld(int); /* */
void str_echo(int);

int main(int argc, char **argv)
{
    int listenfd, connfd, udpfd, nready, maxfdpl;
    char mesg[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0); /* create listening TCP socket */

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    udpfd = socket(AF_INET, SOCK_DGRAM, 0); /* create UDP socket */

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(udpfd, (SA *)&servaddr, sizeof(servaddr));

    signal(SIGCHLD, sig_chld); /* must call waitpid() */

    FD_ZERO(&rset);
    maxfdpl = max(listenfd, udpfd) + 1;
    for ( ; ; ) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        if ((nready = select(maxfdpl, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue; /* back to for() */
            else
                err_sys("select error");
        }

        if (FD_ISSET(listenfd, &rset)) {
            len = sizeof(cliaddr);
            connfd = accept(listenfd, (SA *)&cliaddr, &len);
            if ((childpid = fork()) == 0) { /* child process */
                close(listenfd); /* close listening socket */
                str_echo(connfd); /* process the request */
                exit(0);
            }
            close(connfd); /* parent closes connected socket */
        }

        if (FD_ISSET(udpfd, &rset)) {
            len = sizeof(cliaddr);
            n = recvfrom(udpfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);
            sendto(udpfd, mesg, n, 0, (SA *)&cliaddr, len);
        }
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);

    return;
}

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
        writen(sockfd, buf, n);

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("str_echo: read error");
}
