CC = gcc

all : gad

gad : gadget_finder.c
	$(CC) gadget_finder.c -o gad

clean :
	rm -rf gad
