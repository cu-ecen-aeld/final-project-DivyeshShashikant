ifeq ($(CC),)
	CC = ${CROSS_COMPILE}gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror 
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

default: server

all : server

server: server.o
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

clean: 
	-rm -f *.o server
	
