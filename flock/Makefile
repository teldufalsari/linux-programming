COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: notify

notify: main.o

main.o: main.c
	${COMPILER} ${CFLAGS} main.c -o lock.exe

clean:
	rm -f *.exe
