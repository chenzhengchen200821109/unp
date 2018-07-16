#include "unp.h"
#include <setjmp.h>

static jmp_buf env;

void sig_alrm(int signo)
{
    int val = 1;

    longjmp(env, val);
}

int getnameinfo_timeo(const struct sockaddr *sa, socklen_t salen, char *host, size_t hostlen, char *serv, size_t servlen, int flags, unsigned int seconds)
{
    int ret;

    signal(SIGALRM, sig_alrm);
    alarm(seconds);

    while(setjmp(env) == 0) {
        ret = getnameinfo(sa, salen, host, hostlen, serv, servlen, flags);
        if (ret == 0) {
            alarm(0);
            return 0; // succeed
        }
    }
    if (flags & NI_NAMEREQD == 0) {
        inet_ntop(AF_INET, (SA *)sa, host, hostlen);
        return 0;
    }
    return -1;
}
