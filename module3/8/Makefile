CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11

TARGET = sniffer

SOURCES = main.c sniffer.c

OBJECTS = $(SOURCES:.c=.o)


all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)


main.o: main.c sniffer.h
	$(CC) $(CFLAGS) -c main.c


sniffer.o: sniffer.c sniffer.h
	$(CC) $(CFLAGS) -c sniffer.c


clean:
	rm -f $(OBJECTS) $(TARGET)