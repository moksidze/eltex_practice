all:
	gcc main.c queue.c test.c -o queue.exe

clean:
	del /Q *.exe 2>nul || rm -f *.exe

run: queue.exe
	./queue.exe