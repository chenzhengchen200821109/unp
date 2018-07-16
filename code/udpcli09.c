#include "unp.h"

int main(int argc, char **argv)
{
    int sockfd;
    socklen_t len;
    struct sockaddr_in cliaddr, servaddr;

    if (argc != 2)
        err_quit("usage: udpcli < IPaddress>");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

    len = sizeof(cliaddr);
    getsockname(sockfd, (SA *)&cliaddr, &len);
    printf("local address %s\n", sock_ntop((SA *)&cliaddr, len));

    exit(0);
}
