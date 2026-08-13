CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L

SERVER = server
CLIENT = client

COMMON_OBJ = common.o
SERVER_OBJ = server.o common.o
CLIENT_OBJ = client.o common.o


all: $(SERVER) $(CLIENT)


$(SERVER): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ)


$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ)


server.o: server.c common.h
	$(CC) $(CFLAGS) -c server.c -o server.o


client.o: client.c common.h
	$(CC) $(CFLAGS) -c client.c -o client.o


common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c -o common.o


clean:
	rm -f *.o $(SERVER) $(CLIENT)