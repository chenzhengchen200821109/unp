#include "unp.h"

extern void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
    struct linger optval;
    socklen_t optlen;

	if (argc != 2)
			err_quit("usage: tcpcli <IPaddress>");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd); /* do it all */

    //getsockopt(sockfd, SOL_SOCKET, SO_LINGER, &optval, &optlen);
    //optval.l_onoff = 1;
    //optval.l_linger = 0;
    //setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &optval, optlen);

	exit(0);
}

