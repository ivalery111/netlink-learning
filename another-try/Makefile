obj-m += lkm.o

CC = gcc
CCFLAGS = -Wall -Werror -Wunused -Wextra -Wno-pointer-sign -Wno-unused-parameter
SOURCES = userspace.c
APP_NAME = userspace

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

userspace:
	$(CC) $(SOURCES) $(CCFLAGS) -o $(APP_NAME)

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm -f userspace