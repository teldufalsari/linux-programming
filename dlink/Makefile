COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: 
	${COMPILER} ${CFLAGS} -ldl dlink.c -o dlink.exe
	${COMPILER} ${CFLAGS} -ldl integrate.c -o integrate

clean:
	rm -f *.exe
