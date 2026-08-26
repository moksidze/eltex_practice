obj-m += mychardev.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
	gcc -Wall -Wextra -Werror -std=c11 test.c -o test

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f test
