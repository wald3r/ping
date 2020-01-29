CFLAGS = -Wall -O
CC=gcc
LIB = 


all: pingserver pingclient1 pingclient2 pingclient3

pingserver: pingserver.c
	$(CC) $(CFLAGS) pingserver.c -o pingserver $(LIB)

pingclient1: pingclient1.c
	$(CC) $(CFLAGS) pingclient1.c -o pingclient1 $(LIB)

pingclient2: pingclient2.c
	$(CC) $(CFLAGS) pingclient2.c -o pingclient2 $(LIB)

pingclient3: pingclient3.c
	$(CC) $(CFLAGS) pingclient3.c -o pingclient3 $(LIB)

.PHONY: clean
clean:
	$(RM) pingserver pingclient1 pingclient2 pingclient3

