COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: info

info: info.c childinfo.c childinfo2.c
	${COMPILER} ${CFLAGS} childinfo.c -o childinfo
	${COMPILER} ${CFLAGS} info.c -o info

clean:
	rm -f *.o
