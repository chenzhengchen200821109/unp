#include "unp.h"

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];

    while (fgets(sendline, MAXLINE, fp) != NULL) {
        writen(sockfd, sendline, 1);
        sleep(1);
        writen(sockfd, sendline+1, strlen(sendline)-1);
        if (readline(sockfd, recvline, MAXLINE) == 0)
            err_quit("str_cli: server terminated prematurely");
        fputs(recvline, stdout);
    }
}
