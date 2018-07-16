#ifndef __unp_h
#define __unp_h

#include <sys/select.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/un.h>

/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many kernels still #define
 * it as 5, while actually supporting many more
 */
#define LISTENQ 1024 /* 2nd argument to listen() */
#define SERV_PORT 5122
#define SERV_PORT_STR "9877"
#define UNIXSTR_PATH "/tmp/unix.str" /* Unix domain stream */
#define UNIXDG_PATH "/tmp/unix.dg" /* unix domain stream */

/*
 * We need the newer CMSG_LEN() and CMSG_SPACE() macros, buf few implementations support them
 * today. These two macros really need an ALIGN() macro, but each implementation does this
 * differently.
 */
#ifndef CMSG_LEN
#define CMSG_LEN(size) (sizeof(struct cmsghdr) + (size))
#endif
#ifndef CMSG_SPACE
#define CMSG_SPACE(size) (sizeof(struct cmsghdr) + (size))
#endif

/* Miscellaneous constants */
#define MAXLINE 4096 /* max text line length */
#define BUFFSIZE 8192 /* buffer size for reads and writes */

/* Following shortens all the typecasts of pointer arguments: */
#define SA struct sockaddr

#define min(a, b) ((a) < (b) ? (a) : (b));
#define max(a, b) ((a) > (b) ? (a) : (b));

/* return destination IPv4 address and interface index for UDP */
struct unp_in_pktinfo {
	struct in_addr ipi_addr; /* destination IPv4 address */
	int ipi_ifindex; /* received interface index */
};

/* signal handler */
typedef void Sigfunc(int);
Sigfunc *Signal(int signo, Sigfunc *func);

typedef void (*sig_t)(int);
sig_t signal(int signo, sig_t func);

/* some convenient wrapper functions, which are self-explaination */ 
char *sock_ntop(const struct sockaddr *sa, socklen_t addrlen);
int sock_bind_wild(int sockfd, int family);
int sock_cmp_addr(const struct sockaddr *sockaddr1, const struct sockaddr *sockaddr2, socklen_t addrlen);
int sock_cmp_port(const struct sockaddr *sockaddr1, const struct sockaddr *sockaddr2, socklen_t addrlen);
int sock_get_port(const struct sockaddr *sockaddr, socklen_t addrlen);
char *sock_ntop_host(const struct sockaddr *sockaddr, socklen_t addrlen);
void sock_set_addr(const struct sockaddr *sockaddr, socklen_t addrlen, void *ptr);
void sock_set_port(const struct sockaddr *sockaddr, socklen_t addrlen, void *ptr);
void sock_set_wild(struct sockaddr *sockaddr, socklen_t addrlen);

ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);
ssize_t readline(int filedes, void *buff, ssize_t maxlen);

/* error handlers */
void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

#endif
