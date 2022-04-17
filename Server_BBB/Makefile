CC=${CROSS_COMPILE}gcc
CFLAGS=-g -Wall -Werror 

default: server

all : server

server: server.o
	${CC} ${CFLAGS} server.c -o server -lpthread

clean:
	-rm -f *.o server
