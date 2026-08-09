CC      = gcc
CFLAGS  = -Wall -Wextra -std=gnu11
LDFLAGS = -lrt
TARGET  = chat
SRCS    = chat.c queue.c signal.c
OBJS    = $(SRCS:.c=.o)
DEPS    = chat.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) mychat