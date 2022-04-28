CC := clang
CFLAGS := -I.
DEPS := fs.h
OBJ := fs.o main.o

all: main

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) main

.PHONY: all clean
