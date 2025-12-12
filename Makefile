# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lcurl -ljson-c -lm

# Directories
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
SRCDIR = src
INCDIR = include

# Target executable
TARGET = speedtest

# Source files
SRCS = $(SRCDIR)/main.c $(SRCDIR)/network.c $(SRCDIR)/ip_info.c $(SRCDIR)/display.c
OBJS = $(SRCS:.c=.o)

# Default target
all: check-deps $(TARGET)


# Run target - checks deps, builds if needed, and runs
run: check-deps-silent
	@if [ ! -f $(TARGET) ]; then \
	    echo "Building $(TARGET)..."; \
	    $(MAKE) $(TARGET); \
    fi
	@./$(TARGET)

# Check dependencies silently (for run target)
check-deps-silent:
	@command -v $(CC) >/dev/null 2>&1 || { echo "Error: gcc not found. Run 'sudo make install-deps'"; exit 1; }
	@pkg-config --exists libcurl 2>/dev/null || { echo "Error: libcurl not found. Run 'sudo make install-deps'"; exit 1; }
	@pkg-config --exists json-c 2>/dev/null || { echo "Error: json-c not found. Run 'sudo make install-deps'"; exit 1; }



# Check if dependencies are installed
check-deps:
	@echo "Checking dependencies..."
	@command -v $(CC) >/dev/null 2>&1 || { echo "Error: gcc not found. Run 'chmod +x ./install-deps.sh && sudo ./install-deps.sh'"; exit 1; }
	@pkg-config --exists libcurl 2>/dev/null || { echo "Error: libcurl not found. Run 'chmod +x ./install-deps.sh && sudo ./install-deps.sh'"; exit 1; }
	@pkg-config --exists json-c 2>/dev/null || { echo "Error: json-c not found. Run 'chmod +x ./install-deps.sh && sudo ./install-deps.sh'"; exit 1; }
	@echo "✓ All dependencies found"

# Install dependencies (calls the script)
install-deps:
	@if [ ! -f ./install-deps.sh ]; then \
        echo "Error: install-deps.sh not found"; \
        exit 1; \
    fi
	@echo "Installing dependencies..."
	@chmod +x ./install-deps.sh
	@sudo ./install-deps.sh

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Build complete: ./$(TARGET)"

# Compile source files
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Install to system
install: $(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR)..."
	@install -d $(BINDIR)
	@install -m 755 $(TARGET) $(BINDIR)/$(TARGET)
	@echo "✓ Installed successfully! Run '$(TARGET)' from anywhere"

# Uninstall from system
uninstall:
	@echo "Removing $(TARGET) from $(BINDIR)..."
	@rm -f $(BINDIR)/$(TARGET)
	@echo "✓ Uninstalled successfully"

# Clean build files
clean:
	@rm -f $(SRCDIR)/*.o $(TARGET)
	@echo "✓ Cleaned build files"

# Help message
help:
	@echo "Speedtest CLI - Makefile commands:"
	@echo ""
	@echo "  make                - Build the project (checks deps)"
	@echo "  make install-deps   - Install system dependencies"
	@echo "  make install        - Install to $(BINDIR) (requires sudo)"
	@echo "  make uninstall      - Remove from system (requires sudo)"
	@echo "  make clean          - Remove build files"
	@echo "  make help           - Show this help message"
	@echo ""
	@echo "Quick start:"
	@echo "  1. sudo make install-deps"
	@echo "  2. make"
	@echo "  3. ./speedtest (test locally)"
	@echo "  4. sudo make install (install system-wide)"

.PHONY: all check-deps install-deps install uninstall clean help