#include "semaphore.h"
#include <assert.h>
#include <errno.h>
#define ASSERT()

void sem_init(sem_t* sem, unsigned value) {
    sem->value = value;
    sem->wakeups = 0;
    pthread_mutexattr_t type;
    pthread_mutexattr_settype(&type, PTHREAD_MUTEX_ERRORCHECK_NP);
    pthread_mutex_init(&sem->mutex, &type);
    pthread_cond_init(&sem->cond, NULL);
    // pthread_mutexattr_settype();
}

void sem_wait(sem_t* sem) {
    int err = 0;
    assert(!(err = pthread_mutex_lock(&sem->mutex)) || err == EBUSY);
    sem->value--;
    if (sem->value < 0) {
        do {
            pthread_cond_wait(&sem->cond, &sem->mutex);
        } while (sem->wakeups < 1);
        sem->wakeups--;
    }
    assert(pthread_mutex_unlock(&sem->mutex) == 0);
}

void sem_post(sem_t* sem) {
    int err = 0;
    assert(!(err = pthread_mutex_lock(&sem->mutex)) || err == EBUSY);
    sem->value++;
    if (sem->value <= 0) {
        sem->wakeups++;
        pthread_cond_broadcast(&sem->cond);
    }
    assert(pthread_mutex_unlock(&sem->mutex) == 0);
}

void sem_getvalue(sem_t* sem, int* sval) { *sval = sem->value; }
