head: head.o reader.o
	gcc head.o reader.o

tail: tail.o reader.o
	gcc tail.o reader.o

cp: cp.o reader.o
	gcc cp.o reader.o

pwc: pwc.o
	gcc pwc.o -lpthread

head.o: head.c
	gcc -c -std=c99 -Werror head.c

tail.o: tail.c
	gcc -c -std=c99 -Werror tail.c

cp.o: cp.c
	gcc -c -std=c99 -Werror cp.c

reader.o: reader.c
	gcc -c -std=c99 -Werror reader.c

pwc.o: pwc.c
	gcc -c -std=c99 -Werror pwc.c

clean:
	rm -f *.o

