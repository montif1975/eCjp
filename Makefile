# Makefile to build the workspace as a shared library
# and to compile the test program.

# Compiler and flags
CC = gcc
#CFLAGS = -Wall -I. -I./include -fPIC -O2
CFLAGS = -Wall -I. -I./include -fPIC -g -O0
LDFLAGS = -shared
EXE_LDFLAGS = -Wl,-rpath=. -L. -L$(BUILD) -l$(LIB)

SRC = src
EXAMPLE = example
BUILD = build

# Source and target
LIB = ecjp
LIB_SRC = $(SRC)/$(LIB).c
LIB_OBJ = $(BUILD)/$(LIB).o
LIB_TARGET = $(BUILD)/lib$(LIB).so

EXAMPLE_1 = example_ecjp_1
EXAMPLE_1_SRC = $(SRC)/$(EXAMPLE_1).c
EXAMPLE_1_OBJ = $(BUILD)/$(EXAMPLE_1).o	
EXAMPLE_1_TARGET = $(BUILD)/$(EXAMPLE_1)

EXAMPLE_2 = example_ecjp_2
EXAMPLE_2_SRC = $(SRC)/$(EXAMPLE_2).c
EXAMPLE_2_OBJ = $(BUILD)/$(EXAMPLE_2).o	
EXAMPLE_2_TARGET = $(BUILD)/$(EXAMPLE_2)

EXAMPLE_3 = example_ecjp_3
EXAMPLE_3_SRC = $(SRC)/$(EXAMPLE_3).c
EXAMPLE_3_OBJ = $(BUILD)/$(EXAMPLE_3).o	
EXAMPLE_3_TARGET = $(BUILD)/$(EXAMPLE_3)

TEST_SOURCES := $(wildcard $(SRC)/test_*.c)
TEST_TARGETS := $(patsubst $(SRC)/%.c,$(BUILD)/%,$(TEST_SOURCES))

$(BUILD):
	mkdir -p $(BUILD)

lib: $(LIB_TARGET)

example: $(EXAMPLE_1_TARGET) $(EXAMPLE_2_TARGET) $(EXAMPLE_3_TARGET)

tests : $(TEST_TARGETS)

all: lib example tests

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_TARGET): $(LIB_OBJ)
	@echo "Linking $@..."
	$(CC) $(LDFLAGS) -o $@ $^

$(EXAMPLE_TARGET): $(EXAMPLE_OBJ)
	@echo "Linking $@..." 
	$(CC) -o $(EXAMPLE_TARGET) $< $(EXE_LDFLAGS)

# Compila ogni test separatamente
$(BUILD)/%: $(SRC)/%.c | $(BUILD)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@.o
	@echo "Linking $@..."
	$(CC) -o $@ $@.o $(EXE_LDFLAGS)

clean:
	rm -rf $(BUILD)

.PHONY: all example lib clean

