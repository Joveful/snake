CC?=gcc
SDL2FLAGS=$(shell pkg-config sdl2 --cflags --libs)
CFLAGS?=-std=c11 -Wall -pedantic -Werror -Wshadow -Wstrict-aliasing -Wstrict-overflow

.PHONY: all msg clean fullclean

all: msg snake

msg:
	@echo '--- C11 ---'

snake: snake.c
	${CC} ${CFLAGS} -O2 -o $@ $< ${SDL2FLAGS}

small: snake.c
	${CC} ${CFLAGS} -Os -o snake $< ${SDL2FLAGS}
	-strip snake
	-sstrip snake

debug: snake.c
	${CC} ${CFLAGS} -O1 -g -o snake $< ${SDL2FLAGS}

asm: snake.asm

snake.asm: snake.c
	${CC} ${CFLAGS} -S -masm=intel -Og -o snake.asm $< ${SDL2FLAGS}

run: msg snake
	time ./snake

clean:
	rm -f snake *.o snake.asm

fullclean: clean
