#include "unp.h"

void str_cli(FILE *fp, int sockfd);

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_un servaddr;

	sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXSTR_PATH);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd); /* do it all */

	exit(0);
}

void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		writen(sockfd, sendline, strlen(sendline));

		if (readline(sockfd, recvline, MAXLINE) == 0)
				err_quit("str_cli: server terminated prematurely");
		fputs(recvline, stdout);
	}
}

