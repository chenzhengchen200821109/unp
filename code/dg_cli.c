#include "unp.h"
#define NDG 2000 /* datagrams to send */
#define DGLEN 1400 /* length of each datagram */

/*
 * recvfrom() will be blocked forever if no data is received
 */
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE];
    
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0; /* null terminate */
        fputs(recvline, stdout);
    }
}

/* void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int i;
    char sendline[DGLEN];
    
    for (i = 0; i < NDG; i++) {
        sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
    }
} */

