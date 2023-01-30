CC=gcc
CFLAGS=-I. -g

IDIR=../include
ODIR=obj
LDIR=../lib

LIBS=-lm

_DEPS=libs.h help.h functions.h constants.h console.h types.h strings.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o functions.o console.o strings.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rffmpeg: $(OBJ)
	$(CC) -fdiagnostics-color=always -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean: rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~