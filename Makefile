all: spt.l
spt.l: spt.l.o graph.o
	gcc -o spt.l spt.l.o graph.o `pkg-config --libs glib-2.0`
spt.l.o: spt.l.c graph.c
	gcc `pkg-config --cflags glib-2.0` -c spt.l.c queue.c

