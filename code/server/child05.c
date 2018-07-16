#include "unp.h"
#include "child.h"

pid_t
child_make(int i, int listenfd, int addrlen)
{
    int sockfd[2];
    pid_t pid;
    void child_main(int, int, int);

    socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

    if ((pid = fork()) > 0) { /* parent process */
        close(sockfd[1]);
        cptr[i].child_pid = pid;
        cptr[i].child_pipefd = sockfd[0];
        cptr[i].child_status = 0;
        return (pid);
    }

    dup2(sockfd[1], STDERR_FILENO); /* child's stream pipe to parent */
    close(sockfd[0]);
    close(sockfd[1]);
    close(listenfd); /* child does not need this open */
    child_main(i, listenfd, addrlen); /* never returns */
}
