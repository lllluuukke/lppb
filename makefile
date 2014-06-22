CC=gcc
CCFLAGS=-Wall -Wextra -g -std=c99
PROD_CCFLAGS=-O3

CFLAGS=`pkg-config --cflags gtk+-2.0`
LIBS=`pkg-config --libs gtk+-2.0`

GLIB_FLAGS=`pkg-config --cflags glib-2.0`
GLIB_LIBS=`pkg-config --libs glib-2.0`

MWFLAGS=`MagickWand-config --cflags`
MWLIBS=`MagickWand-config --ldflags --libs`

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJS)
	$(CC) $(CCFLAGS) -o booth $(OBJS) $(MWFLAGS) $(LIBS) $(MWLIBS)

tem: template.o
	$(CC) $(CCFLAGS) template.o -o template $(GLIB_LIBS) $(MWLIBS)

template.o:
	$(CC) $(CCFLAGS) $(GLIB_FLAGS) $(MWFLAGS) template.c -c

clean:
	rm -f *.o booth

