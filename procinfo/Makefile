COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: info

info: info.c childinfo.c wait4parent.c
	${COMPILER} ${CFLAGS} childinfo.c -o childinfo.exe
	${COMPILER} ${CFLAGS} info.c -o info.exe
	${COMPILER} ${CFLAGS} wait4parent.c -o wait4parent.exe

clean:
	rm -f *.exe
