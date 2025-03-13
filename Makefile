CC = gcc
CFLAGS = -Iinclude -O2 -g

SRCS = src/allocator.c src/test.c

all: $(SRCS)
	$(CC) $(CFLAGS) $^ -o test.elf

clean:
	rm test.elf