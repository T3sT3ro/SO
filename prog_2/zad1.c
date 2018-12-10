#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaphore.h"

sem_t sem;
char *msg = "_=^#";
pthread_t t[4];

void work(void* arg) {
    for (int i = 0; i < 200; i++) {
        if (write(STDOUT_FILENO, arg, 1) != 1) exit(EXIT_FAILURE);
        usleep(1);
    }
}

void* testNoLock(void* arg) {
    work(arg);
    return NULL;
}

void* testLock(void* arg) {
    sem_wait(&sem);
    work(arg);
    sem_post(&sem);
    return NULL;
}

void run(void* (*f)(void*)) {
    int err = 0;
    // create 4 threads
    if ((err = pthread_create(&t[0], NULL, f, &msg[0])) ||
        (err = pthread_create(&t[1], NULL, f, &msg[1])) ||
        (err = pthread_create(&t[2], NULL, f, &msg[2])) ||
        (err = pthread_create(&t[3], NULL, f, &msg[3])))
        exit(err);
    // wait for other threads
    if ((err = pthread_join(t[0], NULL)) || (err = pthread_join(t[1], NULL)) ||
        (err = pthread_join(t[2], NULL)) || (err = pthread_join(t[3], NULL)))
        exit(err);
}

int main() {
    // pthread_mutexattr_t *mattr_type;
    // pthread_mutexattr_gettype();
    // pthread_mutexattr
    // pthread_mutexattr_settype(&sem->mutex, PTHREAD_MUTEX_ERRORCHECK_NP);
    printf("Random run without semaphore\n");
    run(&testNoLock);

    printf("\nSemaphore with 3 slots\n");
    sem_init(&sem, 3);
    run(&testLock);

    printf("\nSemaphore with 2 slots\n");
    sem_wait(&sem); // reduce to 2 slots
    run(&testLock);

    printf("\nSemaphore as anonymous mutex\n");
    sem_wait(&sem); // reduce to 1 slot
    run(&testLock);

    // create 3 threads with semaphore lock
    printf("\n");
    exit(EXIT_SUCCESS);
}