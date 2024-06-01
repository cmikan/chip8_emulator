CC = gcc

CFLAGS = -Wall
EXE = emulator

SRCDIR = src
BUILDDIR = build


RED=\033[0;31m
GREEN=\033[0;32m
YELLOW=\033[0;33m
BLUE=\033[0;34m
MAGENTA=\033[0;35m
NC=\033[0m

all: $(EXE)

$(BUILDDIR):
	@echo "$(GREEN)Create build$(GREEN) directory$(NC)"
	@mkdir -p $@

$(EXE): $(BUILDDIR)/display.o $(BUILDDIR)/main.o
	@echo "$(YELLOW)Compiling $(MAGENTA)$(notdir $@)$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^ $(shell sdl2-config --cflags --libs) -lpthread

$(BUILDDIR)/display.o: $(SRCDIR)/display.c | $(BUILDDIR)
	@echo "$(YELLOW)Compiling $(BLUE)$(notdir $@)$(NC)"
	@$(CC) $(CFLAGS) -c -o $@ $^ $(shell sdl2-config --cflags --libs)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "$(YELLOW)Compiling $(BLUE)$(notdir $@)$(NC)"
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	@echo "$(RED)Clean project$(NC)"
	@rm -fr $(BUILDDIR) $(EXE) ||: