#include "unp.h"

#define MAXFILES 20
#define SERV "80" /* port number or service name */

struct file {
    char *f_name; /* filename */
    char *f_host; /* hostname or IPv4/IPv6 address */
    int f_fd; /* descriptor */
    int f_flags; /* F_xxx below */
    pthread_t f_tid; /* thread ID */
} file[MAXFILES];

#define F_CONNECTING 1 /* connect() in progress */
#define F_READING 2 /* connect() complete; noew reading */
#define F_DONE 4 /* all done */
#define F_JOINED 8

#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"

/* globals */
int nconn, nfiles, nlefttoconn, nlefttoread;
fd_set rset, wset;
int ndone; /* number of terminated threads */

void home_page(const char *, const char *);
