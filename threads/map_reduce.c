#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

enum {NUM_THREADS = 12, N = 1000000};

void* thread_body(void* resptr)
{
    unsigned result = 0;
    for (unsigned i = 0; i < N; i++)
        result++;
    *((unsigned*)resptr) = result;
    return NULL;
}

int main()
{
    unsigned results[NUM_THREADS] = {0};
    pthread_t workers[NUM_THREADS];
    for (unsigned i = 0; i < NUM_THREADS; i++)
        if ((errno = pthread_create(workers + i, NULL, &thread_body, (void*)(results + i))) != 0)
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
    unsigned answer = 0;
    for (unsigned i = 0; i < NUM_THREADS; i++)
            answer += results[i];
    printf("The calculations say 12 000 000 is equal to %u\n", answer);
    return 0;
}