dated: tcp_serv.o sock.o date_serv.o
	gcc tcp_serv.o sock.o date_serv.o -o dated

datefd: tcp_serv.o sock.o date_fork_serv.o
	gcc tcp_serv.o sock.o date_fork_serv.o -o datefd

datenfd: tcp_serv.o sock.o date_fork_non_wait_serv.o
	gcc tcp_serv.o sock.o date_fork_non_wait_serv.o -o datenfd

udpd: sock.o udp_serv.o
	gcc sock.o udp_serv.o -o udpd

tcp_serv.o: tcp_serv.c
	gcc -c tcp_serv.c

sock.o: sock.c
	gcc -c sock.c

date_serv.o: date_serv.c
	gcc -c date_serv.c

date_fork_serv.o: date_fork_serv.c
	gcc -c date_fork_serv.c

udp_serv.o: udp_serv.c
	gcc -c udp_serv.c

date_fork_non_wait_serv.o:
	gcc -c date_fork_non_wait_serv.c
