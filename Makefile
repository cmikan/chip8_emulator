CC = gcc

CFLAGS = -Wall
EXE = emulator

SRCDIR = src
BUILDDIR = build

all: $(EXE)

$(EXE): $(SRCDIR)/main.c
	@$(CC) $(CFLAGS) -o $@ $^ $(shell sdl2-config --cflags --libs) 

clean:
	@rm -fr $(BUILDDIR) $(EXE) ||: