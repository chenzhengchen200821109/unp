#include "unp.h"
#include <sys/time.h>

char *gf_time(void)
{
	struct timeval tv;
	static char str[30];
	char *ptr;

	if (gettimeofday(&tv, NULL) < 0)
			err_sys("gettimeofday error");

	ptr = ctime(&tv.tv_sec);
	strcpy(str, &ptr[11]);
	/* Fri Sep 13 00:00:00 1986\n\0 */
	snprintf(str + 8, sizeof(str) - 8, ".%06ld", (long)tv.tv_usec);

	return(str);
}
