#define _BSD_SOURCE
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

#define SHM_NAME "/clock"
#define SEMAPH_NAME "/m_sem"
#define err_handle(message, code) {perror(message); exit(code);}
#define TIME_OUT_FORMAT "%Y-%m-%d %H:%M:%S"

void handler(int sig, siginfo_t *si, void *unused);
int term_g = 1;

int main()
{

    struct sigaction term_action;
    sigemptyset(&term_action.sa_mask);
    term_action.sa_sigaction = handler;
    if ((sigaction(SIGTERM, &term_action, NULL)) 
    || (sigaction(SIGINT, &term_action, NULL)))
        err_handle("sigaction", 2);

    //umask

    int shm_des = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0700);
    if (shm_des < 0)
        err_handle("Failed to create shared memory", 1);
    ftruncate(shm_des, 80);
    void* p = mmap(
        NULL,
        sysconf(_SC_PAGE_SIZE),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_des,
        0);
    if (p == MAP_FAILED)
        err_handle("Failed to map memory", 3);
    printf("Allocated page at [%p]\n", p);
    
    sem_t* ex_sem = sem_open(SEMAPH_NAME, O_CREAT, 0700, 1);
    if (ex_sem == SEM_FAILED)
        err_handle("Failed to open semaphore", 1);
    time_t btime;
    struct tm* timestruct;
    char timebuf[80];
    char* dest = (char*)p;
    while(term_g)
    {
        btime = time(NULL);
        timestruct = localtime(&btime);
        strftime(timebuf, sizeof(timebuf), TIME_OUT_FORMAT, timestruct);
        sem_wait(ex_sem);
        strcpy(dest, timebuf);
        sem_post(ex_sem);
        sleep(1);
    }
    close(shm_des);
    sem_close(ex_sem);
    sem_unlink(SEMAPH_NAME);
    unlink(SHM_NAME);
    return 0;
}

void handler(int sig, siginfo_t *si, void *unused)
{
    term_g = 0;
}