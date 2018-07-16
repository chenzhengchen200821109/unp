#include "unp.h"

void str_echo(int);

int main(int argc, char *argv[])
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_un cliaddr, servaddr;
	void sig_chld(int);

	listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);

	unlink(UNIXSTR_PATH);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);

	bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	Signal(SIGCHLD, sig_chld);

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
			str_echo(connfd); /* process request */
			exit(0);
		}

		close(connfd); /* parent closes connected socket */
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

