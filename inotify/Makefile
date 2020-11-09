COMPILER = clang
CFLAGS = -Wall -Wextra

all: notify

notify: main.o

main.o: main.c
	${COMPILER} ${CFLAGS} main.c

clean:
	rm -f *.o