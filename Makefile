CC = gcc

CFLAGS = -Wall -o2
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

$(EXE): $(BUILDDIR)/display.o $(BUILDDIR)/chip8.o $(BUILDDIR)/chip8_utils.o
	@echo "$(YELLOW)Compiling $(MAGENTA)$(notdir $@)$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^ $(shell sdl2-config --cflags --libs) -lpthread -lm

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "$(YELLOW)Compiling $(BLUE)$(notdir $@)$(NC)"
	@$(CC) $(CFLAGS) -c -o $@ $^

$(BUILDDIR):
	@echo "$(GREEN)Create build$(GREEN) directory$(NC)"
	@mkdir -p $@

clean:
	@echo "$(RED)Clean project$(NC)"
	@rm -fr $(BUILDDIR) $(EXE) ||: