all: libs $(TARGET)

libs:
	mkdir -p plugins
	$(CC) $(CFLAGS) -shared add.c -o add.so
	$(CC) $(CFLAGS) -shared sub.c -o sub.so
	$(CC) $(CFLAGS) -shared mul.c -o mul.so
	$(CC) $(CFLAGS) -shared div.c -o div.so

$(TARGET): main.c
	$(CC) main.c -o $(TARGET) $(LDFLAGS)

run: calc.exe
	LD_LIBRARY_PATH=. ./calc.exe

clean:
	rm -f $(TARGET)
	rm -rf plugins