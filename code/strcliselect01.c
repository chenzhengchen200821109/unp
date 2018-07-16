#include "unp.h"

void str_cli(FILE* fp, int sockfd);

int main(int argc, char **argv) 
{
	int i, sockfd[5];
	struct sockaddr_in servaddr;

	if (argc != 2)
			err_quit("usage: strcliselect01 <IPaddress>");

	for (i = 0; i < 5; i++) {
		sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(SERV_PORT);
		inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

		connect(sockfd[i], (SA *)&servaddr, sizeof(servaddr));
	}

	str_cli(stdin, sockfd[0]); /* do it all */

	exit(0);
}

void str_cli(FILE* fp, int sockfd)
{
	int maxfdpl;
	fd_set rset;
	char sendline[MAXLINE], recvline[MAXLINE];

	FD_ZERO(&rset);
	for ( ; ; ) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdpl = max(fileno(fp), sockfd);
		select(maxfdpl + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) { /* socket is readable */
			if (readline(sockfd, recvline, MAXLINE) == 0)
					err_quit("str_cli: server terminated prematurely");
			fputs(recvline, stdout);
		}

		if (FD_ISSET(fileno(fp), &rset)) { /* input is readable */
			if (fgets(sendline, MAXLINE, fp) == NULL)
					return; /* all done */
			writen(sockfd, sendline, strlen(sendline));
		}
	}
}
