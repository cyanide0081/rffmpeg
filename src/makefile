CC=gcc
CFLAGS=-I. -g -std=c17

IDIR=../include
ODIR=obj
LDIR=../lib

LIBS=-lm

_DEPS=libs.h help.h mainloop.h handlers.h constants.h terminal.h input.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o mainloop.o handlers.o input.o terminal.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rffmpeg: $(OBJ)
	$(CC) -fdiagnostics-color=always -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean: rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~