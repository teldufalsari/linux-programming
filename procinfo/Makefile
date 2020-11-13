COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: info

info: info.c childinfo.c wait4parent.c
	${COMPILER} ${CFLAGS} childinfo.c -o childinfo
	${COMPILER} ${CFLAGS} info.c -o info
	${COMPILER} ${CFLAGS} wait4parent.c -o wait4parent

clean:
	rm -f *.o
