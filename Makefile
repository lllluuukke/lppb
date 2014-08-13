CC = gcc
CFLAGS = -Iinclude -fdiagnostics-color=always -Wall -Wextra -g -O3 -std=c99 -c
LDFLAGS = -std=c99

GTKFLAGS = `pkg-config --cflags gtk+-2.0`
GTKLIBS = `pkg-config --libs gtk+-2.0`

GLIB_FLAGS = `pkg-config --cflags glib-2.0`
GLIB_LIBS = `pkg-config --libs glib-2.0`

MWFLAGS = `MagickWand-config --cflags`
MWLIBS = `MagickWand-config --ldflags --libs`

BOOTH = booth
BOOTH_OBJS = gui.o main.o template.o dsets.o

TEMPLATE = template
TEMPLATE_OBJS = template.o dsets.o

.PHONY: clean all

all: $(BOOTH)

$(BOOTH): $(BOOTH_OBJS)
	$(CC) $(LDFLAGS) $(GTKLIBS) $(MWLIBS) -o $@ $^

$(TEMPLATE): $(TEMPLATE_OBJS)
	$(CC) $(LDFLAGS) $(GLIB_LIBS) $(MWLIBS) -o $@ $^

main.o: src/main.c
	$(CC) $(CFLAGS) $(GTKFLAGS) $<

gui.o: src/gui.c include/gui.h
	$(CC) $(CFLAGS) $(GTKFLAGS) $<

template.o: src/template.c include/template.h include/dsets.h
	$(CC) $(CFLAGS) $(GLIB_FLAGS) $(MWFLAGS) $<

dsets.o: src/dsets.c include/dsets.h
	$(CC) $(CFLAGS) $(GLIB_FLAGS) $<

clean:
	rm -f *.o $(BOOTH) $(TEMPLATE)

