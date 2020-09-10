CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -O3 -fPIC

SLIB=libchessic.so
TEST=test

src_chessic= \
	src/bits.c \
	src/board.c \
	src/move.c \
	src/movegen.c \
	src/parser.c \
	src/piece.c

src_test=$(src_chessic) \
	tests/make_undo_tests.c \
	tests/movegen_tests.c \
	tests/parser_tests.c \
	tests/perft_tests.c \
	tests/test.c

obj_chessic=$(src_chessic:.c=.o)
obj_test=$(src_test:.c=.o)

$(SLIB): $(obj_chessic)
	$(CC) $(CFLAGS) -shared $^ -o $@

$(TEST): $(obj_test)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj_chessic) $(obj_test) $(SLIB) $(TEST)
