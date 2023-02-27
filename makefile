# Compiler / binaries
CC=gcc
BIN=rffmpeg

# Compiler flags
OPTFLAGS=-g -DDEBUG
DEPFLAGS=-MP -MD
CCFLAGS=-Wall -Wno-unused-variable -Wno-unused-result -std=gnu17  -fdiagnostics-color=always $(foreach D,$(INCDIRS),-I$(D)) $(DEPFLAGS) $(OPTFLAGS)

# Directories
CDIRS=./src
ODIRS=./src/obj
DDIRS=./src/dep
INCDIRS=./include ./include/help
TESTDIRS=./tests
TESTBINDIRS=./tests/obj

# Source files
CFILES=$(foreach D, $(CDIRS), $(wildcard $(D)/*.c))
HFILES=$(foreach D, $(INCDIRS), $(wildcard $(D)/*.h))

# Object and CC dependency files
OFILES=$(patsubst $(CDIRS)/%.c, $(ODIRS)/%.o, $(CFILES))
DFILES=$(patsubst $(ODIRS)/%.o, $(DDIRS)/%.d, $(OFILES))

# Test files
TESTFILES=$(foreach D, $(TESTDIRS), $(wildcard $(TESTDIRS)/*.c))
TESTBINS=$(patsubst $(TESTDIRS)/%.c, $(TESTBINDIRS)/%, $(TESTFILES))

all: $(BIN)

# Release mode toggles optimization switches and disables debug info generation
release: OPTFLAGS=-DNDEBUG -s -Os -fdata-sections -ffunction-sections -Wl,--gc-sections -flto
release: clean
release: $(BIN)

$(TESTBINDIRS)/%: $(TESTDIRS)/%.c
	$(CC) $(CCFLAGS) -o $@ $< $(OFILES)

test: $(TESTBINS)
	for test in $(TESTBINS) ; do ./$$test ; done

$(BIN): $(OFILES)
	$(CC) $(CCFLAGS) -o $@ $^

-include $(DFILES)

%.o: ../%.c makefile $(HFILES)
	$(CC) $(CCFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm ./src/obj/*
	rm ./rffmpeg
	rm ./rffmpeg.exe