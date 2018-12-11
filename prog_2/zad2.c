#define _GNU_SOURCE  // comment out if not working xD
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "semaphore.h"

#define CNT 5
pthread_mutex_t mfork[CNT], printingMutex;
pthread_t philosophers[CNT];

#define BLU "\x1B[34;12m"
#define RED "\x1B[31;12m"
#define GRE "\x1B[32;12m"
#define _RE "\x1B[0m"
typedef enum { THINK, WAIT, EAT } STATUS_t;
static inline void printStatus(int phID, STATUS_t status) {
    // write status to CERR
    pthread_mutex_lock(&printingMutex);
    static char msg[] =
        "\r[1]:"BLU"XXXXX"_RE"[2]:"BLU"XXXXX"_RE"[3]:"BLU"XXXXX"_RE"[4]:"BLU"XXXXX"_RE"[5]:"BLU"XXXXX"_RE;
    static int mstart = 5, moff = 8 + 5 + 4 + 4;  // for offset in status

    if (status == THINK) strncpy(&msg[mstart + moff * phID], BLU "THINK"_RE, 8 + 5 + 4);
    if (status == WAIT) strncpy(&msg[mstart + moff * phID], RED "WAIT "_RE, 8 + 5 + 4);
    if (status == EAT) strncpy(&msg[mstart + moff * phID], GRE "EAT  "_RE, 8 + 5 + 4);
    if (write(STDERR_FILENO, msg, strlen(msg)) < 0) exit(EXIT_FAILURE);
    pthread_mutex_unlock(&printingMutex);
}

static inline void delay(int timescale) { usleep(timescale + rand() % timescale); }

void take_forks(int i) {
    if ((int)i == 0 || (int)i == 2) {  // righty (two for more concurrently running)
        pthread_mutex_lock(&mfork[(i + 1) % CNT]);
        pthread_mutex_lock(&mfork[i]);
    } else {  // lefty
        pthread_mutex_lock(&mfork[i]);
        pthread_mutex_lock(&mfork[(i + 1) % CNT]);
    }
}

void put_forks(int i) {
    pthread_mutex_unlock(&mfork[i]);
    pthread_mutex_unlock(&mfork[(i + 1) % CNT]);
}

void* live(void* i) {
    int phID = (int)(long)i;
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
}

int main() {
    pthread_mutex_init(&printingMutex, NULL);

    // mask out SIGINT for children(sigmask is copied for children threads)
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    int err;
    if ((err = pthread_sigmask(SIG_BLOCK, &set, NULL)) != 0) exit(err);

    for (int i = 0; i < CNT; i++) {
        pthread_mutex_init(&mfork[i], NULL);
        int err = 0;
        if ((err = pthread_create(&philosophers[i], NULL, &live, (void*)(long)i)))
            exit(err);
    }
    // wait for SIGINT in main thread
    if ((err = pthread_sigmask(SIG_UNBLOCK, &set, NULL)) != 0) exit(err);
    sigwait(&set, &err);
    printf("\n");
    exit(EXIT_SUCCESS);
}