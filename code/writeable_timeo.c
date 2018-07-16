#include "unp.h"

int writeable_timeo(int fd, int sec)
{
    fd_set wset;
    struct timeval tv;

    FD_ZERO(&wset);
    FD_SET(fd, &wset);

    tv.tv_sec = sec;
    tv.tv_usec = 0;

    return (select(fd+1, NULL, &wset, NULL, &tv));
}
