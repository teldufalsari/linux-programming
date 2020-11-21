#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#define SHM_NAME "/clock"
#define SEMAPH_NAME "/m_sem"
#define err_handle(message, code) {perror(message); exit(code);}

void handler(int sig, siginfo_t *si, void *unused);

int term_g = 1;

int main()
{
    struct sigaction term_action = {};
    term_action.sa_sigaction = handler;
    if ((sigaction(SIGTERM, &term_action, NULL)) 
    || (sigaction(SIGINT, &term_action, NULL)))
        err_handle("sigaction", 2);

    int shm_des = shm_open(SHM_NAME, O_CREAT | O_RDONLY, 0700);
    if (shm_des < 0)
        err_handle("Failed to open shared memory", 1);

    void* p = mmap(
        NULL,
        sysconf(_SC_PAGE_SIZE),
        PROT_READ,
        MAP_SHARED,
        shm_des,
        0);
    if (p == MAP_FAILED)
        err_handle("Failed to map memory", 3);
    close(shm_des);
    printf("Allocated page at [%p]\n", p);

    sem_t* ex_sem = sem_open(SEMAPH_NAME, O_RDWR);
    if (ex_sem == SEM_FAILED)
        err_handle("Failed to open semaphore", 1);
    
    char* src = (char*)p;
    while(term_g)
    {
        sem_wait(ex_sem);
        printf("[%s]\n", src);
        sem_post(ex_sem);
        sleep(1);
    }
    printf("\nStopping client...\n");
    return 0;
}

void handler(int sig, siginfo_t *si, void *unused)
{
    (void)sig;
    (void)si;
    (void)unused;
    term_g = 0;
}