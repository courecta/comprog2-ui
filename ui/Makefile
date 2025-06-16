# Twisted Fables GUI - Unified Build System
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -DSDL_DELAY_AVAILABLE
INCLUDES = -Iinclude -Iassets/lib
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image -lm -lcurl

# Source and object directories
SRCDIR = src
OBJDIR = obj
INCDIR = include

# Source files
BOT_SOURCES = $(wildcard $(SRCDIR)/bot/*.c)
MAIN_SOURCES = $(wildcard $(SRCDIR)/*.c)
SOURCES = $(MAIN_SOURCES) $(BOT_SOURCES)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(MAIN_SOURCES)) \
          $(patsubst $(SRCDIR)/bot/%.c,$(OBJDIR)/bot_%.o,$(BOT_SOURCES))

# Target executable
TARGET = twisted_fables_gui

# Game engine library
GAME_ENGINE = assets/lib/tf.o

.PHONY: all clean test test-verbose test-build clean-tests clean-all quick-test info debug release

all: $(TARGET)

# Create obj directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile bot object files
$(OBJDIR)/bot_%.o: $(SRCDIR)/bot/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link final executable
$(TARGET): $(OBJECTS) $(GAME_ENGINE)
	$(CC) $(GAME_ENGINE) $(OBJECTS) $(LIBS) -o $@

# Clean build files
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Clean everything including tests
clean-all: clean clean-tests

# Test target - quick run of main application
quick-test: $(TARGET)
	./$(TARGET)

# Test compilation only
test-build: $(OBJECTS)
	@echo "All source files compile successfully!"

# Comprehensive testing framework
test: test-build
	@echo "Running comprehensive test suite..."
	$(CC) $(CFLAGS) $(INCLUDES) tests/comprehensive_test.c $(GAME_ENGINE) $(LIBS) -o tests/test_runner
	./tests/test_runner

# Verbose test output
test-verbose: test-build
	@echo "Running tests with verbose output..."
	$(CC) $(CFLAGS) $(INCLUDES) -DVERBOSE_TESTS tests/comprehensive_test.c $(GAME_ENGINE) $(LIBS) -o tests/test_runner_verbose
	./tests/test_runner_verbose

# Clean test artifacts
clean-tests:
	rm -f tests/test_runner tests/test_runner_verbose

# Install dependencies (for reference)
install-deps:
	sudo apt-get update
	sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

# Show project info
info:
	@echo "Twisted Fables GUI - Unified Project"
	@echo "====================================="
	@echo "Source files: $(SOURCES)"
	@echo "Object files: $(OBJECTS)"
	@echo "Target: $(TARGET)"
	@echo "Game engine: $(GAME_ENGINE)"
	@echo "Includes: $(INCLUDES)"
	@echo ""
	@echo "Available targets:"
	@echo "  all         - Build the main application"
	@echo "  clean       - Clean build artifacts"
	@echo "  test        - Run comprehensive tests"
	@echo "  quick-test  - Build and run application"
	@echo "  info        - Show this information"

# Debug build
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)
