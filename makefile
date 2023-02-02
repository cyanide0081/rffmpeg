CC=gcc
CFLAGS=-fdiagnostics-color=always $(OPTIMIZATIONFLAGS) -Wall -Wno-unused-variable -Wno-unused-function -std=c17 -municode

IDIR=include
ODIR=src/obj
SDIR=src
LDIR=lib
OPTIMIZATIONFLAGS=-s -Os -fdata-sections -ffunction-sections -Wl,--gc-sections -flto

LIBS=-lm

_DEPS=libs.h constants.h mainloop.h handlers.h terminal.h input.h man/help.h types.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o typefunctions.o mainloop.o handlers.o input.o terminal.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rffmpeg: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean: $(OBJ)
	rm -f $(OBJ)/*.o