#include "web01.h"
#include <pthread.h>

#define F_JOINED 8 /* main has thread_join'ed */

pthread_mutex_t ndone_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ndone_cond = PTHREAD_COND_INITIALIZER;

void* tcp_connect(const char*, char*);

void
write_get_cmd(struct file* fptr)
{
    int n;
    char line[MAXLINE];

    n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);
    writen(fptr->f_fd, line, n);
    printf("wrote %d bytes for %s\n", n, fptr->f_name);
    fptr->f_flags = F_READING; /* clears F_CONNECTING */
    FD_SET(fptr->f_fd, &rset); /* will read server's reply */
}

void* do_get_read(void* vptr)
{
    int fd, n;
    char line[MAXLINE];
    struct file* fptr;

    fptr = tcp_connect(fptr->f_host, SERV);
    fptr->f_fd = fd;
    printf("do_get_read for %s, fd %d, thread %d\n", fptr->f_name, fd, (int)fptr->f_tid);
    write_get_cmd(fptr); /* write() the GET command */

    /* Read server's reply */
    for ( ; ; ) {
        if ((n = read(fd, line, MAXLINE)) == 0)
            break; /* server closed connection */
        printf("read %d bytes from %s\n", n, fptr->f_name);
    }
    printf("end-of-file on %s\n", fptr->f_name);
    close(fd);
    fptr->f_flags = F_DONE; /* clears F_READING */

    pthread_mutex_lock(&ndone_mutex);
    ndone++;
    pthread_cond_signal(&ndone_cond);
    pthread_mutex_unlock(&ndone_mutex);

    return (fptr); /* terminate thread */
}

int main(int argc, char *argv[])
{
    int i, n, maxnconn;
    pthread_t tid;
    struct file *fptr;
    
    if(argc < 5)
        err_quit("usage: web <#conns> <hostname> <homepage> <file1> ...");
    maxnconn = atoi(argv[1]);

    nfiles = min(argc - 4, MAXFILES);
    for (i = 0; i < nfiles; i++) {
        file[i].f_name = argv[i + 4];
        file[i].f_host = argv[2];
        file[i].f_flags = 0;
    }
    printf("nfiles = %d\n", nfiles);

    home_page(argv[2], argv[3]);
    nlefttoread = nlefttoconn = nfiles;
    nconn = 0;

    while (nlefttoread > 0) {
        while (nconn < maxnconn && nlefttoconn > 0) {
            // find a file to read
            for (i = 0; i < nfiles; i++)
                if (file[i].f_flags == 0)
                    break;
            if (i == nfiles)
                err_quit("nlefttoconn = %d but nothing found", nlefttoconn);
            file[i].f_flags = F_CONNECTING;
            pthread_create(&tid, NULL, &do_get_read, &file[i]);
            file[i].f_tid = tid;
            nconn++;
            nlefttoconn--;
        }
        /* Wait for thread to terminate */
        pthread_mutex_lock(&ndone_mutex);
        while (ndone == 0)
            pthread_cond_wait(&ndone_cond, &ndone_mutex);
        for (i = 0; i < nfiles; i++) {
            if (file[i].f_flags & F_DONE) {
                pthread_join(file[i].f_tid, (void **)&fptr);
                if (&file[i] != fptr)
                    err_quit("file[i] != fptr");
                fptr->f_flags = F_JOINED; /* clears F_DONE */
                ndone--;
                nconn--;
                nlefttoread--;
                printf("thread %d for %s done\n", (int)fptr->f_tid, fptr->f_name);
            }
        }
        pthread_mutex_unlock(&ndone_mutex);
    }
    exit(0);
}
