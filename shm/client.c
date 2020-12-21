#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define SHM_NAME "/clock"
#define TIMESTR_SIZE 64
volatile int g_terminate = 0;

struct shared_buffer_t {
    unsigned write_flag;
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

    int shm_des = shm_open(SHM_NAME, O_RDWR, 0644); // File mode is ignored
    if (shm_des < 0) {
        if (errno == ENOENT) {
            printf("Could not find time reference file\n");
            return 0;
        } else {
            perror("Could not open shared memory file");
            return 1;
        }
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
    close(shm_des);
    printf("Allocated page at [%p]\n", p);
        
    struct shared_buffer_t* buf = (struct shared_buffer_t*)p;
    const char* time_str = (const char*)buf->string;
    unsigned int read_flag;
    while(!g_terminate) {
        read_flag = buf->write_flag;
        printf("[%.*s]\n", TIMESTR_SIZE, time_str);
        if (read_flag != buf->write_flag)
            continue;
        sleep(1);
    }
    printf("\nStopping client...\n");
    return 0;
}
