CC=gcc
CFLAGS=-Wall -Wextra -Werror
TFLAGS = -lcheck --coverage

ifeq ($(shell uname), Linux)
	TFLAGS += -lsubunit -lm
endif

ifeq ($(shell uname), Darwin)
	TFLAGS += -fprofile-arcs -ftest-coverage
endif


all: clean install

install: build
	install tetris ~/Desktop/tetris

uninstall:
	rm -rf ~/Desktop/tetris

build: *.o
	$(CC) $(CFLAGS) $^ -lncurses -o tetris

tetris_test.a: ./brick_game/tetris/tetris.c
	$(CC) $(CFLAGS) $(TFLAGS) -c $^
	ar rcs $@ tetris.o

*.o: main.c ./brick_game/tetris/tetris.c ./brick_game/tetris/tetris.h ./gui/cli/cli.c ./gui/cli/cli.h
	$(CC) $(CFLAGS) -c $^

dist: main.c Makefile ./brick_game/tetris/tetris.c ./brick_game/tetris/tetris.h ./gui/cli/cli.c ./gui/cli/cli.h
	mkdir tetris-1.0
	cp $^ tetris-1.0
	tar -czvf tetris-1.0.tar.gz tetris-1.0
	rm -rf tetris-1.0

dvi: 
	texi2dvi tetris.texi
	evince tetris.dvi

test: tetris_test.a
	$(CC) -o test.a tetris_test.c tetris_test.a $(TFLAGS)
	./test.a


gcov_report: test
	gcov -b ./brick_game/tetris/tetris.c tetris_test.c
	lcov -d . -c -o coverage.info
	genhtml coverage.info -o html_report
	open html_report/index.html


format:
	clang-format --style=Google -i main.c ./brick_game/tetris/tetris.c ./brick_game/tetris/tetris.h ./gui/cli/cli.c ./gui/cli/cli.h

format_check:
	clang-format --style=Google -n main.c ./brick_game/tetris/tetris.c ./brick_game/tetris/tetris.h ./gui/cli/cli.c ./gui/cli/cli.h

clean: uninstall
	rm -rf tetris tetris-1.0 tetris-1.0.tar.gz *.o tetris.log tetris.toc tetris.dvi *.gcno *.gcda *.a test.a coverage.info html_report