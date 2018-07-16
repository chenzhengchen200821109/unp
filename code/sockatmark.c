#include "unp.h"

int sockatmark_v(int fd)
{
		int flag;

		if (ioctl(fd, SIOCATMARK, &flag) < 0)
				return (-1);
		return (flag != 0);
}
