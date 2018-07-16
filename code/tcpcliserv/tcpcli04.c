#include "unp.h"

extern void str_cli(FILE *fp, int sockfd);

void sig_pipe(int signo) 
{
    printf("received SIGPIPE\n");    
}

int main(int argc, char **argv)
{
	int i, sockfd[5];
	struct sockaddr_in servaddr;

	if (argc != 2)
			err_quit("usage: tcpcli <IPaddress>");
	
    Signal(SIGPIPE, sig_pipe);
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

