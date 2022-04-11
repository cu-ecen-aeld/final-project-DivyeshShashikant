ifeq ($(CC),)
	CC = ${CROSS_COMPILE}gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror 
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

default: tmp102

all : tmp102

tmp102: tmp102.o
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

clean: 
	-rm -f *.o tmp102
	
