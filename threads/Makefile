COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: threads

threads: with_data_race.c semaphores.c
	${COMPILER} ${CFLAGS} -pthread with_data_race.c -o data_race.exe
	${COMPILER} ${CFLAGS} -pthread semaphores.c -o semaphores.exe

clean:
	rm -f *.exe