#include "unp.h"

void str_echo(int);

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	/* signal(SIGCHLD, sig_chld); */

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	signal(SIGCHLD, sig_chld);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (SA *)&cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
					continue; /* back to for() */
			else
					err_sys("accept error");
		}

		if ((childpid = fork()) == 0) { /* child process */
			close(listenfd); /* close listening socket */
			str_echo(connfd); /* process the request */
			exit(0);
		}
		close(connfd); /* parent closes connected socket */
	}


}

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

again:
	while ((n = read(sockfd, buf, MAXLINE))  > 0)
			writen(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
			goto again;
	else if (n < 0)
			err_sys("str_echo: read error");
}
