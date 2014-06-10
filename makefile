CC=gcc
CCFLAGS=-Wall -g -std=c99
PROD_CCFLAGS=-O3
CFLAGS=`pkg-config --cflags gtk+-2.0`
LIBS=`pkg-config --libs gtk+-2.0`
MWFLAGS=`MagickWand-config --cflags`
MWLIBS=`MagickWand-config --ldflags --libs`
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJS)
	$(CC) $(CCFLAGS) -o booth $(OBJS) $(LIBS) 

tem:
	$(CC) $(CCFLAGS) $(MWFLAGS) template.c -o template $(MWLIBS)

clean:
	rm -f *.o booth

