#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define SHM_NAME "/clock"
#define TIMESTR_SIZE 64
#define TIME_OUT_FORMAT "%Y-%m-%d UTC %z (%Z) %H:%M:%S"
volatile int g_terminate = 0;

struct shared_buffer_t {
    sem_t sem;
    char string[TIMESTR_SIZE];
};

void handler(int sig) {
    (void)sig;
    g_terminate = 1;
}

int main() {
    struct sigaction term_action = {};
    term_action.sa_flags = SA_RESTART;
    term_action.sa_handler = handler;
    if ((sigaction(SIGTERM, &term_action, NULL)) 
    || (sigaction(SIGINT, &term_action, NULL))) {
        perror("sigaction");
        return 1;
    }

    int shm_des = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (shm_des < 0) {
        if (errno == EEXIST) {
            printf("A server is already running\n");
            return 0;
        } else {
            perror("Could not create shared memory file");
            return 1;
        }
    }
    if (ftruncate(shm_des, sysconf(_SC_PAGE_SIZE))) {
        perror("ftruncate");
        close(shm_des);
        shm_unlink(SHM_NAME);
        return 2;
    }

    void* p = mmap(
        NULL,
        sysconf(_SC_PAGE_SIZE),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_des,
        0);
    if (p == MAP_FAILED) {
        perror("Failed to map shared memory");
        close(shm_des);
        unlink(SHM_NAME);
        return 3;
    }
    printf("Allocated page at [%p]\n", p);
    
    struct shared_buffer_t* buf = (struct shared_buffer_t*)p;
    sem_init(&buf->sem, 1, 1);
    time_t btime;
    struct tm* timestruct;
    while(!g_terminate) {
        sem_wait(&buf->sem);
        btime = time(NULL);
        timestruct = localtime(&btime);
        strftime(buf->string, TIMESTR_SIZE, TIME_OUT_FORMAT, timestruct);
        sem_post(&buf->sem);
        sleep(1);
    }
    printf("\nStopping server...\n");
    if(close(shm_des)) {
        perror("close");
        return 4;
    }
    if(shm_unlink(SHM_NAME)) {
        perror("Failed to remove shared memory file");
        return 4;
    }
    return 0;
}
