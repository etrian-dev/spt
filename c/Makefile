.POSIX:
.PREFIX:

CC = gcc
CFLAGS = -g -pedantic `pkg-config --cflags glib-2.0`
LDLIBS = `pkg-config --libs glib-2.0` -lreadline -lm

all: spt.l
spt.l: spt.l.o glib-graph.o
	$(CC) -o spt.l spt.l.o glib-graph.o $(LDLIBS)
spt.l.o: spt.l.c
	$(CC) $(CFLAGS) -c spt.l.c
glib-graph.o: glib-graph.c glib-graph.h
	$(CC) $(CFLAGS) -c glib-graph.c
clean:
	rm -f spt.l spt.l.o print-graph.o
