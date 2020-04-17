obj-m += lkm.o

CC = gcc
CCFLAGS = -Wall -Werror -Wunused -Wextra -Wno-pointer-sign -Wno-unused-parameter
LIBS = -lpthread
SOURCES = userspace.c
APP_NAME = userspace
LKM_NAME = lkm.ko

.PHONE: all clean userspace insmod rmmod

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

userspace:
	$(CC) $(SOURCES) $(CCFLAGS) -o $(APP_NAME) $(LIBS)

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm -f userspace

insmod:
	sudo insmod $(LKM_NAME)

rmmod:
	sudo rmmod $(LKM_NAME)