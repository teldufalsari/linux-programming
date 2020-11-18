#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

enum {NUM_THREADS = 12, N = 1000000};

volatile unsigned g_counter = 0;

void* thread_body(void* arg)
{
    sem_t* mutex_sem = (sem_t*)arg;
    for (unsigned i = 0; i < N; i++)
    {
        sem_wait(mutex_sem);
        g_counter++;
        sem_post(mutex_sem);
    }
    return NULL;
}

int main()
{
    sem_t* mutex_sem = sem_open("mutex_sem", O_CREAT, 0700, 1);
    if (mutex_sem == SEM_FAILED)
    {
        perror("Failed to create a named semaphore");
        return 2;
    }
    pthread_t workers[NUM_THREADS];
    for (unsigned i = 0; i < NUM_THREADS; i++)
        if ((errno = pthread_create(workers + i, NULL, &thread_body, mutex_sem)) != 0)
        {
            perror("pthread: failed to create thread");
            return 1;
        }
    for (unsigned i = 0; i < NUM_THREADS; i++)
        if ((errno = pthread_join(workers[i], NULL)) != 0)
        {
            perror("pthread: failed to join thread");
            return 1;
        }
    sem_close(mutex_sem);
    sem_unlink("mutex_sem");
    printf("The calculations say 12 000 000 is equal to %u\n", g_counter);
    return 0;
}