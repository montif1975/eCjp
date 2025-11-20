# Makefile to build the workspace as a shared library
# and to compile the test program.

# Compiler and flags
CC = gcc
#CFLAGS = -Wall -I. -I./include -fPIC -O2
CFLAGS = -Wall -I. -I./include -fPIC -g -O0
LDFLAGS = -shared
EXAMPLE_LDFLAGS = -Wl,-rpath=. -L. -L$(BUILD) -l$(LIB)

SRC = src
EXAMPLE = example
BUILD = build

# Source and target
LIB = ecjp
LIB_SRC = $(SRC)/$(LIB).c
LIB_OBJ = $(BUILD)/$(LIB).o
LIB_TARGET = $(BUILD)/lib$(LIB).so

EXAMPLE = example_ecjp
EXAMPLE_SRC = $(SRC)/$(EXAMPLE).c
EXAMPLE_OBJ = $(BUILD)/$(EXAMPLE).o	
EXAMPLE_TARGET = $(BUILD)/$(EXAMPLE)

$(BUILD):
	mkdir -p $(BUILD)

lib: $(LIB_TARGET)

example: $(EXAMPLE_TARGET)

all: lib example

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_TARGET): $(LIB_OBJ)
	@echo "Linking $@..."
	$(CC) $(LDFLAGS) -o $@ $^

$(EXAMPLE_TARGET): $(EXAMPLE_OBJ)
	@echo "Linking $@..." 
	$(CC) -o $(EXAMPLE_TARGET) $< $(EXAMPLE_LDFLAGS)

clean:
	rm -rf $(BUILD)

.PHONY: all example lib clean

