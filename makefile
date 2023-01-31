CC=gcc
CFLAGS=-fdiagnostics-color=always $(OPTIMIZATIONFLAG) -Wall -Wno-unused-variable -Wno-unused-function -std=c17 -municode

IDIR=./include
ODIR=src/obj
SDIR=src
LDIR=lib
OPTIMIZATIONFLAG=-Os

LIBS=-lm

_DEPS=libs.h man/help.h mainloop.h handlers.h constants.h terminal.h input.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o mainloop.o handlers.o input.o terminal.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rffmpeg: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean: $(OBJ)
	rm -f $(OBJ)/*.o