#include "unp.h"
#include <sys/param.h> /* ALIGN macro for CMSG NXTHDR() macro */

ssize_t recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp, SA *sa, socklen_t *salenptr, struct unp_in_pktinfo *pktp)
{
	struct msghdr msg;
	struct iovec iov[1];
	ssize_t n;

#ifdef HAVE_MSGHDR_MSG_CONTROL
	struct cmsghdr *cmptr;
	union {
		struct cms ghdr cm;
		char control[CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(struct unp_in_pktinfo))];
	} control_un;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	msg.msg_flags = 0;
#else
	bzero(&msg, sizeof(msg)); /* make certain msg_accrightslen = 0 */
#endif
	msg.msg_name = sa;
	msg.msg_namelen = *salenptr;
	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ((n = recvmsg(fd, &msg, *flagsp)) < 0)
			return (n);
	*salenptr = msg.msg_namelen; /* pass back results */
	if (pktp)
			bzero(pktp, sizeof(struct unp_in_pktinfo)); /* 0.0.0.0, i/f = 0 */

#ifdef HAVE_MSGHDR_MSG_CONTROL
	*flagsp = 0; /* pass back results */
	return (n);
#else
	*flagsp = msg.msg_flags; /* pass back results */
	if (msg.msg_controllen < sizeof(struct cmsghdr) || (msg.msg_flags & CMSG_CTRUNC) || pktp == NULL)
			return (n);

	for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL; cmptr = CMSG_NXTHDR(&msg, cmptr)) {
#ifdef IP_RECVDSTADDR
			if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVDSTADDR) {
				memcpy(&pktp->ipi_addr, CMSG_DATA(cmptr), sizeof(struct in_addr));
				continue;
			}
#endif
#ifdef IP_RECVIF
			if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVIF) {
				struct sockaddr_dl *sdl;
				sdl = (struct sockaddr_dl *)CMSG_DATA(cmptr);
				pktp->ipi_ifindex = sdl->sdl_index;
				continue;
			}
#endif
			err_quit("unknown ancillary data, len = %d, level = %d, type = %d", cmptr->cmsg_len, cmptr->cmsg_level, cmptr->cmsg_type);
	}
	return (n);
#endif /* HAVE_MSGHDR_MSG_CONTROL */
}