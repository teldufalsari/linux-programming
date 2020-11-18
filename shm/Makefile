COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: shmat

shmat: server.c client.c
	${COMPILER} ${CFLAGS} -lrt -pthread server.c -o server.exe
	${COMPILER} ${CFLAGS} -lrt -pthread client.c -o client.exe

clean:
	rm -f *.exe
