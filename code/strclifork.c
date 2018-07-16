#include "unp.h"

void str_cli(FILE *fp, int sockfd)
{
	pid_t pid;
	char sendline[MAXLINE], recvline[MAXLINE];

	if ((pid = fork()) == 0) { /* child: server -> stdout */
		while (readline(sockfd, recvline, MAXLINE) > 0)
				fputs(recvline, stdout);

		kill(getppid(), SIGTERM); /* in case parent still running */
		exit(0);
	}

	/* parent: stdin -> server */
	while (fgets(sendline, MAXLINE, fp) != NULL)
		writen(sockfd, sendline, MAXLINE, strlen(sendline));

	shutdown(sockfd, SHUT_WR); /* EOF on stdin, send FIN */
	pause();

	return;
}
