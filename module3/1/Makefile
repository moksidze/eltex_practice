CC = gcc
CFLAGS = -Wall -Wextra

TARGET = copy

SRC = main.c parent.c child.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *.copy

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run