CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g
OBJECTS = main.o helpers.o process.o handler.o

shell379: $(OBJECTS)
	$(CC) -o shell379 $(OBJECTS)

main.o: main.c helpers.h process.h handler.h
	$(CC) $(CFLAGS) -c main.c -o main.o

handler.o: handler.c handler.h process.h helpers.h
	$(CC) $(CFLAGS) -c handler.c -o handler.o

process.o: process.c process.h helpers.h
	$(CC) $(CFLAGS) -c process.c -o process.o

helpers.o: helpers.c helpers.h
	$(CC) $(CFLAGS) -c helpers.c -o helpers.o

clean:
	rm *.o
	rm shell379