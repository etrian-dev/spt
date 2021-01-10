.POSIX:
.PREFIX:

CC = gcc
CFLAGS = -g -pedantic `pkg-config --cflags glib-2.0`
LDLIBS = `pkg-config --libs glib-2.0` -lreadline -lm
MYHEADERS = /home/nicola/include/
MYSRC = /home/nicola/src/

spt.s: spt.s.o
	$(CC) $(CFLAGS) -o spt.s spt.s.o -L/home/nicola/bin/shared -lglib-graph $(LDLIBS)
spt.l: spt.l.o
	$(CC) -o spt.l spt.l.o -L/home/nicola/bin/shared -lglib-graph $(LDLIBS)
spt.s.o: spt.s.c
	$(CC) $(CFLAGS) -c spt.s.c
spt.l.o: spt.l.c
	$(CC) $(CFLAGS) -c spt.l.c
clean:
	rm -f spt.l spt.l.o spt.s spt.s.o
