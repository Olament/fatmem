CC := clang
SRC := src
BUILD := build
TARGET := main

SOURCES := $(shell find $(SRC) -type f -name *.c)
OBJECTS := $(patsubst $(SRC)/%,$(BUILD)/%,$(SOURCES:.c=.o))
CFLAGS := -g -Wall -Wno-unused-function
LIB := 
INC := -I include

all: main

main: $(OBJECTS)
	$(CC) $(BUILD)/fd.o $(BUILD)/fs.o $(BUILD)/main.o $(BUILD)/posix.o -o main $(LIB)

$(BUILD)/%.o: $(SRC)/%.c
	@mkdir -p $(BUILD)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "$(RM) -r $(BUILD) $(TARGET)"; $(RM) -r $(BUILD) $(TARGET)

.PHONY: all clean