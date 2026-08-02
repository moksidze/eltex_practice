CC = gcc

CFLAGS = -Wall -Wextra -fPIC

LDFLAGS = -shared

LIBDIR = libs

TARGET = calculator

all: $(LIBDIR) $(LIBDIR)/add.so $(LIBDIR)/sub.so $(LIBDIR)/mul.so $(LIBDIR)/div.so $(TARGET)

add: $(LIBDIR)/add.so

sub: $(LIBDIR)/sub.so

mul: $(LIBDIR)/mul.so

div: $(LIBDIR)/div.so

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(LIBDIR)/add.so: add.c calc.h
	$(CC) $(CFLAGS) $(LDFLAGS) add.c -o $(LIBDIR)/add.so

$(LIBDIR)/sub.so: sub.c calc.h
	$(CC) $(CFLAGS) $(LDFLAGS) sub.c -o $(LIBDIR)/sub.so

$(LIBDIR)/mul.so: mul.c calc.h
	$(CC) $(CFLAGS) $(LDFLAGS) mul.c -o $(LIBDIR)/mul.so

$(LIBDIR)/div.so: div.c calc.h
	$(CC) $(CFLAGS) $(LDFLAGS) div.c -o $(LIBDIR)/div.so

$(TARGET): main.c
	$(CC) main.c -ldl -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf $(LIBDIR)
	rm -f $(TARGET)