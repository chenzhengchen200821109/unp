#include "unp.h"

int readable_timeo(int fd, int sec)
{
	fd_set rset;
	struct timeval tv;

	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

    /* time expires return 0, error return -1 */
	return (select(fd+1, &rset, NULL, NULL, &tv)); /* > 0 if descriptor is readable */
}
