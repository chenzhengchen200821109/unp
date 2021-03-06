#include "unp.h"

int main(int argc, char **argv)
{
    //int counter; /* count the number of succussful read() */
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
    //socklen_t len;
    //struct sockaddr_storage ss;

	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");
    //if (argc != 3)
    //    err_quit("usage: daytimetcpcli <hostname/IPaddress> <service/port#>");
	//if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	if ((sockfd = socket(9999, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13); /* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
			err_quit("inet_pton error for %s", argv[1]);

	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");
    //sockfd = tcp_connect(argv[1], argv[2]);

    //len = sizeof(ss);
    //getpeername(sockfd, (SA *)&ss, &len);
    //printf("connected to %s\n", sock_ntop_host((SA *)&ss, len));

    //counter = 0;
	//while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
    //    counter++;
    //	recvline[n] = 0; /* null terminate */
    //	if (fputs(recvline, stdout) == EOF)
	//		err_sys("fputs error");
    //}
    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0; /* null terminate */
        if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
    }
	if (n < 0)
		err_sys("read error");

    //printf("The number of succussful read() call is %d\n", counter);

	exit(0);
}
