CC := clang
CFLAGS := -I.
DEPS := fs.h fd.h posix.h
OBJ := fs.o fd.o posix.o main.o

all: main

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) main

.PHONY: all clean
