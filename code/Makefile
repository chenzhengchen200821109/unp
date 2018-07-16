# Learning how to write a makefile
#

targets = daytimetcpcli daytimetcpsrv daytimetcpsrv1 tcpcli01 tcpserv01 tcpserv02 tcpcli04 tcpserv03 \
		 strcliselect01 udpserv01 udpcli01 unixstrcli01 unixstrserv01
src = error.c unp.c unp.h

cc = gcc-6

all: $(targets)
.PHONY: all

unixstrcli01:
	$(cc) -o unixstrcli01 unixstrcli01.c $(src)

unixstrserv01:
	$(cc) -o unixstrserv01 unixstrserv01.c $(src)

udpserv01:
	$(cc) -o udpserv01 udpserv01.c $(src)

udpcli01:
	$(cc) -o udpcli01 udpcli01.c $(src)

strcliselect01:
	$(cc) -o strcliselect01 strcliselect01.c $(src)

tcpserv03:
	$(cc) -o tcpserv03 tcpserv03.c $(src)

tcpcli04:
	$(cc) -o tcpcli04 tcpcli04.c $(src)

tcpserv02:
	$(cc) -o tcpserv02 tcpserv02.c $(src)

tcpcli01:
	$(cc) -o tcpcli01 tcpcli01.c $(src)

tcpserv01:
	$(cc) -o tcpserv01 tcpserv01.c $(src)

daytimetcpcli:
	$(cc) -o daytimetcpcli daytimetcpcli.c $(src)

daytimetcpsrv:
	$(cc) -o daytimetcpsrv daytimetcpsrv.c $(src)

daytimetcpsrv1:
	$(cc) -o daytimetcpsrv1 daytimetcpsrv1.c $(src)

.PHONY: clean
clean:
	rm -rf $(targets) 

