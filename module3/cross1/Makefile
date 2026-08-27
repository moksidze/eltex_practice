CC = gcc

CFLAGS = -Wall -Wextra -std=c11

all: server client

server: server.c common.c common.h
	$(CC) $(CFLAGS) server.c common.c -o server

client: client.c common.c common.h
	$(CC) $(CFLAGS) client.c common.c -o client

clean:
	rm -f server client