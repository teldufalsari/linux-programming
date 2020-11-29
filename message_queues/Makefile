COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: 
	${COMPILER} ${CFLAGS} -lrt default_queue.c -o default_queue.exe
	${COMPILER} ${CFLAGS} -lrt msgboard_client.c -o msgboard_client.exe
	${COMPILER} ${CFLAGS} -lrt msgboard_server.c -o msgboard_server.exe

clean:
	rm -f *.exe
