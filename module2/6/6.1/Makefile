all: libbook.a program.exe

book.o: book.c book.h
	gcc -c book.c -o book.o

libbook.a: book.o
	ar rcs libbook.a book.o

program.exe: main.c libbook.a
	gcc main.c -L. -lbook -o program.exe

run: program.exe
	./program.exe

test: libbook.a
	gcc test.c -L. -lbook -o test.exe
	./test.exe

clean:
	rm -f *.o *.a *.exe