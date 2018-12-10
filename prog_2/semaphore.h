#define _GNU_SOURCE // comment out if not working xD
#include <pthread.h>
#include <stdlib.h>

// implementacja z listy 5 zadania 1
typedef struct {
    int value, wakeups;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sem_t;

void sem_init(sem_t* sem, unsigned value);
void sem_wait(sem_t* sem);
void sem_post(sem_t* sem);
void sem_getvalue(sem_t* sem, int *sval);
