CC=gcc
BIN=rffmpeg

CDIRS=./src
ODIRS=./src/obj
DDIRS=./src/dep
INCDIRS=./include ./include/man

# This build system builds a file ready to be debugged by default. If you want to use its size optimizations
# for a final build, uncomment OPTFLAGS, comment DBGFLAGS, use `make clean` and just run `make`

DBGFLAGS=#-g
OPTFLAGS=-s -Os -fdata-sections -ffunction-sections -Wl,--gc-sections -flto
DEPFLAGS=-MP -MD
CCFLAGS=-Wall -Wno-unused-variable -Wno-unused-function -std=c17 -municode -fdiagnostics-color=always -municode $(foreach D,$(INCDIRS),-I$(D)) $(DEPFLAGS)

CFILES=$(foreach D,$(CDIRS),$(wildcard $(D)/*.c))
HFILES=$(foreach D,$(INCDIRS),$(wildcard $(D)/*.h))
OFILES=$(patsubst $(CDIRS)/%.c, $(ODIRS)/%.o, $(CFILES))
DFILES=$(patsubst $(ODIRS)/%.o, $(DDIRS)/%.d, $(OFILES))

all: $(BIN)

$(BIN): $(OFILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(DBGFLAGS) -o $@ $^

-include $(DFILES)

%.o: ../%.c makefile $(HFILES)
	$(CC) $(CCFLAGS) $(OPTFLAGS) $(DBGFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	del /Q .\\src\\obj\\*
#	del /Q .\\src\\dep\\*
	del .\\rffmpeg.exe
	del .\\debug.exe