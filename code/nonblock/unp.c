#include "unp.h"

/* signal handler */
Sigfunc *Signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
			act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
	} else {
#ifdef SA_RESTART
			act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
			return(SIG_ERR);
	return(oact.sa_handler);
}

/* This implementation is incomplete */
char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	char portstr[8];
	static char str[128]; /* Unix domain is largest */

	switch(sa->sa_family) {
	case AF_INET: {
					struct sockaddr_in *sin = (struct sockaddr_in *)sa;
					if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
							return(NULL);
					if (ntohs(sin->sin_port) != 0) {
						snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
						strcat(str, portstr);
					}
					return(str);
				  }
	}
}

ssize_t readn(int fd, void *vptr, size_t n) /* read n bytes from a descriptor */
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
					nread = 0; /* and call read() again */
			else 
					return -1;
		} else if (nread == 0)
				break; /* EOF */
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft); /* return >= 0 */
}

ssize_t writen(int fd, const void *vptr, size_t n) /* write n bytes to a descriptor */
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
					nwritten = 0; /* and call write() again */
			else
					return -1; /* error */
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}

//ssize_t readline(int fd, void *vptr, ssize_t maxlen) /* PAINFULLY SLOW VERSION -- example only */
//{
//	ssize_t n, rc;
//	char c, *ptr;

//	ptr = (char *)vptr;
//	for (n = 1; n < maxlen; n++) {
//	again:
//			if ((rc = read(fd, &c, 1)) == 1) {
//				*ptr++ = c;
//				if (c == '\n')
//						break; /* newline is sorted, like fgets() */
//			} else if (rc == 0) {
//				*ptr = 0;
//				return(n - 1); /* EOF, n - 1 bytes were read */
//			} else {
//				if (errno == EINTR)
//						goto again;
//				return -1; /* error, errno set by read() */
//			}
//	}
//	*ptr = 0; /* null terminate like fgets() */
//	return(n);
//}

int sockfd_to_family(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t len;

	len = sizeof(ss);
	if (getsockname(sockfd, (SA *)&ss, &len) < 0)
			return -1;
	return (ss.ss_family);
}
