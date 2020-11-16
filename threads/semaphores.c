#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

enum {NUM_THREADS = 12, N = 1000000};

volatile unsigned g_counter = 0;
sem_t mutex;

void* thread_body(void* arg)
{
    (void)arg;
    for (unsigned i = 0; i < N; i++)
    {
        sem_wait(&mutex);
        g_counter += 1;
        sem_post(&mutex);
    }
    return NULL;
}

int main()
{
    sem_init(&mutex, 0, 1);
    pthread_t workers[NUM_THREADS];
    for (unsigned i = 0; i < NUM_THREADS; i++)
        if ((errno = pthread_create(workers + i, NULL, &thread_body, NULL)) != 0)
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
    printf("The calculations say 12 000 000 is equal to %u\n", g_counter);
    return 0;
}