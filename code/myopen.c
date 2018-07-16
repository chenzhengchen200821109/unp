#include "unp.h"

extern ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd);

int my_open(const char *pathname, int mode)
{
	int fd, sockfd[2], status;
	pid_t childpid;
	char c, argsockfd[10], argmode[10];

	socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

	if ((childpid = fork()) == 0) { /* child process */
		close(sockfd[0]);
		snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]);
		execl("./openfile", "openfile", argsockfd, pathname, argmode, (char *)NULL);
		err_sys("execl error");
	}

	/* parent process - wait for the child to terminate */
	close(sockfd[1]); /* close the end we don't use */

	waitpid(childpid, &status, 0);
	if (WIFEXITED(status) == 0) /* terminate normally or not */
			err_quit("child process did not terminate");
	if ((status = WEXITSTATUS(status)) == 0)
			read_fd(sockfd[0], &c, 1, &fd);
	else {
		errno = status; /* set errno value from child's status */
		fd = -1;
	}

	close(sockfd[0]);
	return(fd);
}
