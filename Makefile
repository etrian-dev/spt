.POSIX:
.PREFIX:

CC = gcc
CFLAGS = -Wall -pedantic `pkg-config --cflags glib-2.0`
DBFLAGS = -g -D DEBUG
LDLIBS = `pkg-config --libs glib-2.0` -lreadline -lm

all: spt
debug: main.c spt.s.c spt.l.c glib-graph.c
	$(CC) $(CFLAGS) $(DBFLAGS) -o spt-db main.c spt.s.c spt.l.c glib-graph.c $(LDLIBS)
spt: main.c spt.s.o spt.l.o glib-graph.o
	$(CC) $(CFLAGS) -O2 -o spt main.c spt.s.o spt.l.o glib-graph.o $(LDLIBS)
spt.s.o: spt.s.c glib-graph.o
	$(CC) $(CFLAGS) -c spt.s.c glib-graph.o
spt.l.o: spt.l.c glib-graph.o
	$(CC) $(CFLAGS) -c spt.l.c glib-graph.o
glib-graph.o: glib-graph.c
	$(CC) $(CFLAGS) -c glib-graph.c
clean:
	rm -f spt spt-db spt.l.o spt.s.o glib-graph.o
