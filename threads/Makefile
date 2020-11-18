COMPILER = clang
CFLAGS = -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wconversion -Wunreachable-code -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self

all: with_data_race semaphores pthread_mutex named_sem atomics map_reduce

with_data_race: with_data_race.c semaphores.c pthread_mutex.c
	${COMPILER} ${CFLAGS} -pthread with_data_race.c -o data_race.exe

semaphores: semaphores.c
	${COMPILER} ${CFLAGS} -pthread semaphores.c -o semaphores.exe

pthread_mutex: pthread_mutex.c
	${COMPILER} ${CFLAGS} -pthread pthread_mutex.c -o pthread_mutex.exe

named_sem: named_sem.c
	${COMPILER} ${CFLAGS} -pthread named_sem.c -o named_sem.exe

atomics: atomics.c
	${COMPILER} ${CFLAGS} -pthread atomics.c -o atomics.exe

map_reduce: map_reduce.c
	${COMPILER} ${CFLAGS} -pthread map_reduce.c -o map_reduce.exe

clean:
	rm -f *.exe