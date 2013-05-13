CC=gcc 
CFLAGS=`pkg-config --cflags gtk+-2.0` -Wall -g
#CFLAGS=`pkg-config --cflags gtk+-3.0` -Wall -g -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE
LIBS=`pkg-config --libs gtk+-2.0`
#LIBS=`pkg-config --libs gtk+-3.0`
OBJS=gui.o main.o
PROG=booth

gui.o:
	rm -f gui.o
	$(CC) -c gui.c $(CFLAGS) 

main.o:
	rm -f main.o
	$(CC) -c main.c $(CFLAGS)

booth: $(OBJS)
	$(CC) -o booth $(OBJS) $(LIBS) 
	rm -f *.o

clean:
	rm -f *.o booth

