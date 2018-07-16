#include "unp.h"

void str_echo(int sockfd)
{
	char line[MAXLINE];
	FILE *fpin, *fpout;

	fpin = fdopen(sockfd, "r");
	fpout = fdopen(sockfd, "w");

	while (fgets(line, MAXLINE, fpin) != NULL)
			fputs(line, fpout);
}
