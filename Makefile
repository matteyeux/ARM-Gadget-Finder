CC = gcc

all : gadget_finder

gadget_finder : gadget_finder.c
	$(CC) gadget_finder.c -o gadget_finder

clean :
	rm -rf gadget_finder
