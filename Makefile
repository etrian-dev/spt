.POSIX:
.PREFIX:

CC = gcc
CFLAGS = -pedantic `pkg-config --cflags glib-2.0`
DBFLAGS = -g -D DEBUG
LDLIBS = `pkg-config --libs glib-2.0` -lreadline -lm

all: spt.s spt.l
debug: spt.s.c spt.l.c glib-graph.c
	$(CC) $(CFLAGS) $(DBFLAGS) -o db-spt.s spt.s.c glib-graph.c $(LDLIBS)
	$(CC) $(CFLAGS) $(DBFLAGS) -o db-spt.l spt.l.c glib-graph.c $(LDLIBS)
spt.s: spt.s.o glib-graph.o
	$(CC) -O2 -o spt.s spt.s.o glib-graph.o $(LDLIBS)
spt.l: spt.l.o glib-graph.o
	$(CC) -O2 -o spt.l spt.l.o glib-graph.o $(LDLIBS)
spt.s.o: spt.s.c glib-graph.c
	$(CC) $(CFLAGS) -c spt.s.c glib-graph.c
spt.l.o: spt.l.c glib-graph.c
	$(CC) $(CFLAGS) -c spt.l.c glib-graph.c
clean:
	rm -f spt.l spt.l.o spt.s spt.s.o db-spt.l db-spt.s glib-graph.o
