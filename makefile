APP=test
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O3

src=$(shell find src/ -type f -name '*.c')
obj=$(src:.c=.o)

test: $(obj)
	$(CC) $(CFLAGS) $^ -o $(APP)

.PHONY: clean
clean:
	rm -f $(obj) $(APP)
