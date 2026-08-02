all: libbook.so program.exe

libbook.so: book.c book.h
	gcc -shared -fPIC book.c -o libbook.so

program.exe: main.c libbook.so
	gcc main.c -L. -lbook -o program.exe

run: program.exe
	LD_LIBRARY_PATH=. ./program.exe

test: libbook.so
	gcc test.c -L. -lbook -o test.exe
	LD_LIBRARY_PATH=. ./test.exe

clean:
	rm -f *.o *.so *.exe