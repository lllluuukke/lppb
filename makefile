CC=gcc 
CFLAGS=`pkg-config --cflags gtk+-2.0` -Wall -g
LIBS=`pkg-config --libs gtk+-2.0`
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJS)
	$(CC) -o booth $(OBJS) $(LIBS) 

clean:
	rm -f *.o booth

