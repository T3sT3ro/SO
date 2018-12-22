#define _GNU_SOURCE  // comment out if not working xD
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "semaphore.h"

#define CNT 5
sem_t semfork[CNT];  // semaphores for cancelation release
pthread_mutex_t critsec;
pthread_t philosophers[CNT];

#define BLU "\x1B[34;12m"
#define RED "\x1B[31;12m"
#define GRE "\x1B[32;12m"
#define _RE "\x1B[0m"
#define XXX "XXXXX"
typedef enum { THINK, WAIT, EAT } STATUS_t;
static inline void printStatus(int phID, STATUS_t status) {
    // write status to CERR
    pthread_mutex_lock(&critsec);
    static char msg[] = "\r[1]:" BLU XXX _RE "[2]:" BLU XXX _RE "[3]:" BLU XXX _RE
                        "[4]:" BLU XXX _RE "[5]:" BLU XXX _RE;
    static int mstart = 5, moff = 8 + 5 + 4 + 4;  // for offset in status

    if (status == THINK) strncpy(&msg[mstart + moff * phID], BLU "THINK"_RE, 8 + 5 + 4);
    if (status == WAIT) strncpy(&msg[mstart + moff * phID], RED "WAIT "_RE, 8 + 5 + 4);
    if (status == EAT) strncpy(&msg[mstart + moff * phID], GRE "EAT  "_RE, 8 + 5 + 4);
    if (write(STDERR_FILENO, msg, strlen(msg)) < 0) exit(EXIT_FAILURE);
    pthread_mutex_unlock(&critsec);
}

static inline void delay(int timescale) { usleep(timescale + rand() % timescale); }

void take_forks(int i) {
    if ((int)i == 0 || (int)i == 2) {  // righty (two for more concurrently running)
        sem_wait(&semfork[(i + 1) % CNT]);
        sem_wait(&semfork[i]);
    } else {  // lefty
        sem_wait(&semfork[i]);
        sem_wait(&semfork[(i + 1) % CNT]);
    }
}

void put_forks(int i) {
    sem_post(&semfork[i]);
    sem_post(&semfork[(i + 1) % CNT]);
}

static void die(void *i){
    pthread_mutex_lock(&critsec);
    int phID = (int)(long)i;
    int l, r;
    sem_getvalue(&semfork[phID], &l);
    sem_getvalue(&semfork[(phID+1)%CNT], &r);
    if(l==0) sem_post(&semfork[phID]);
    if(r==0) sem_post(&semfork[(phID+1)%CNT]);
    pthread_mutex_unlock(&critsec);
}

void* live(void* i) {
    int phID = (int)(long)i;
    pthread_cleanup_push(&die, NULL); // cleanup handler
    while (1) {
        printStatus(phID, THINK);
        delay(1000000);
        printStatus(phID, WAIT);
        take_forks(phID);
        printStatus(phID, EAT);
        delay(1000000);
        printStatus(phID, THINK);
        put_forks(phID);
    }
    pthread_cleanup_pop(true); // cleanup handler
}

int main() {
    pthread_mutex_init(&critsec, NULL);

    // mask out SIGINT for children(sigmask is copied for children threads)
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    int err;
    if ((err = pthread_sigmask(SIG_BLOCK, &set, NULL)) != 0) exit(err);

    for (int i = 0; i < CNT; i++) {
        sem_init(&semfork[i], 1);
        int err = 0;
        if ((err = pthread_create(&philosophers[i], NULL, &live, (void*)(long)i)))
            exit(err);
    }
    // wait for SIGINT in main thread
    if ((err = pthread_sigmask(SIG_UNBLOCK, &set, NULL)) != 0) exit(err);
    sigwait(&set, &err);
    for (int i = 0; i < CNT; i++) pthread_cancel(philosophers[i]);
    for (int i = 0; i < CNT; i++) pthread_join(philosophers[i], NULL);
    pthread_mutex_destroy(&critsec);
    exit(EXIT_SUCCESS);
}