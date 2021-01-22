.POSIX:
.PREFIX:

CC = gcc
CFLAGS = -pedantic `pkg-config --cflags glib-2.0`
DBFLAGS = -g -D DEBUG
LDLIBS = `pkg-config --libs glib-2.0` -lreadline -lm
MYHEADERS = /home/nicola/include/
MYSRC = /home/nicola/src/

all: spt.s spt.l
debug: spt.s.c spt.l.c
	$(CC) $(CFLAGS) $(DBFLAGS) -o db-spt.s spt.s.c -L/home/nicola/bin/shared -lglib-graph $(LDLIBS)
	$(CC) $(CFLAGS) $(DBFLAGS) -o db-spt.l spt.l.c -L/home/nicola/bin/shared -lglib-graph $(LDLIBS)
spt.s: spt.s.o
	$(CC) -O2 -o spt.s spt.s.o -L/home/nicola/bin/shared -lglib-graph $(LDLIBS)
spt.l: spt.l.o
	$(CC) -O2 -o spt.l spt.l.o -L/home/nicola/bin/shared -lglib-graph $(LDLIBS)
spt.s.o: spt.s.c
	$(CC) $(CFLAGS) -c spt.s.c
spt.l.o: spt.l.c
	$(CC) $(CFLAGS) -c spt.l.c
clean:
	rm -f spt.l spt.l.o spt.s spt.s.o db-spt.l db-spt.s
