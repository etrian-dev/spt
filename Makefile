all: spt.l
spt.l: spt.l.o
	gcc -o spt.l spt.l.o `pkg-config --libs glib-2.0` -lreadline -lm
spt.l.o: spt.l.c
	gcc -g -pedantic `pkg-config --cflags glib-2.0` -c spt.l.c

