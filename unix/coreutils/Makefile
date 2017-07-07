head: head.o reader.o
	gcc head.o reader.o -o headx

tail: tail.o reader.o
	gcc tail.o reader.o -o tailx

head.o: head.c
	gcc -c head.c

tail.o: tail.c
	gcc -c tail.c

reader.o: reader.c
	gcc -c reader.c

