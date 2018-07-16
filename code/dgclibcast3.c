#include "unp.h"

static void recvfrom_alarm(int);

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int n;
	const int on = 1;
	char sendline[MAXLINE], recvline[MAXLINE + 1];
	sigset_t sigset_alrm;
	socklen_t len;
	struct sockaddr *preply_addr;

	preply_addr = malloc(servlen);

	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

	sigemptyset(&sigset_alrm);
	sigaddset(&sigset_alrm, SIGALRM);
	Signal(SIGALRM, recvfrom_alarm);

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		alarm(5);
		for ( ; ; ) {
			len = servlen;
			sigprocmask(SIG_UNBLOCK, &sigset_alrm, NULL);
			n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
			sigprocmask(SIG_BLOCK, &sigset_alrm, NULL);
			if (n < 0) {
				if (errno == EINTR)
						break; /* waited long enough for replies */
				else
						err_sys("recvfrom error");
			} else {
				recvline[n] = 0; /* null terminate */
				printf("from %s: %s", sock_ntop_host(preply_addr, len), recvline);
			}
		}
	}
	free(preply_addr);
}

static void recvfrom_alarm(int signo)
{
	return; /* just interrupt the recvfrom() */
}
