#include "unp.h"
#include <time.h>

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t len;
	char buff[MAXLINE];
	time_t ticks;
    struct sockaddr_storage cliaddr;

    if (argc == 2)
        listenfd = tcp_listen(NULL, argv[1], &cliaddr);
    else if (argc == 3)
        listenfd = tcp_listen(argv[1], argv[2], &cliaddr);
    else
        err_quit("usage: daytimetcpsrv2 [ <host> ] <service or port>");

	for ( ; ; ) {
        len = sizeof(cliaddr);
		connfd = accept(listenfd, (SA *)&cliaddr, &len);
		printf("connection from %s, port %d\n", inet_ntop((SA *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		write(connfd, buff, strlen(buff));

		close(connfd);
	}
	
}
