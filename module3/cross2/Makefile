CC = gcc

CFLAGS = -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L

TARGET = taxi

OBJ = main.o driver.o ipc.o timer.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lrt

main.o: main.c driver.h ipc.h
	$(CC) $(CFLAGS) -c main.c

driver.o: driver.c driver.h ipc.h timer.h
	$(CC) $(CFLAGS) -c driver.c

ipc.o: ipc.c ipc.h
	$(CC) $(CFLAGS) -c ipc.c

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c timer.c

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean