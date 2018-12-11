#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaphore.h"

pthread_t pagent, psmoker[3], pguardian[3];
// put - semaphore signalling the ingredient was put, roll - smokers waiting on 'em
sem_t put[3], roll[3], smoke, mutex;
typedef const enum { PAPER = 0, MATCH, TOBACCO } ITEM;  // enum for something
const char sym[3] = {[PAPER] = 'P', [MATCH] = 'M', [TOBACCO] = 'T'};  // symbols

void* agent(void* arg) {
    while (1) {
        sem_wait(&smoke);
        ITEM x = rand() % 3;  // fuk the srand()

        char buf[13];
        snprintf(buf, sizeof(buf), "AGENT: %c %c\n", sym[(x + 1) % 3], sym[(x + 2) % 3]);
        if (write(STDOUT_FILENO, buf, sizeof(buf)) < 0) exit(EXIT_FAILURE);

        sem_post(&put[(x + 1) % 3]);
        sem_post(&put[(x + 2) % 3]);

        sleep(1);
    }
}

void* smoker(void* arg) {
    ITEM type = (ITEM)arg;  // evil cast
    while (1) {
        sem_wait(&roll[type]);

        char buf[15];
        snprintf(buf, sizeof(buf), "SMOKER: %c\n", sym[type]);
        if (write(STDOUT_FILENO, buf, sizeof(buf)) < 0) exit(EXIT_FAILURE);

        sleep(2);
        sem_post(&smoke);
    }
}

void* guardian(void* arg) {
    ITEM item = (ITEM)arg;  // evil cast
    static bool is[3] = {[PAPER] = false, [MATCH] = false, [TOBACCO] = false};
    while (1) {
        sem_wait(&put[item]);  // wait for guardian's item to appear
        sem_wait(&mutex);      // enter critsection (prevent race on is[])

        if (is[(item + 1) % 3]) {  // if other item is present then guardian signalled
            is[(item + 1) % 3] = false;
            sem_post(&roll[(item + 2) % 3]);  // so wake up the smoker
        } else if (is[(item + 2) % 3]) {      // same with other ingredient
            is[(item + 2) % 3] = false;
            sem_post(&roll[(item + 1) % 3]);
        } else {  // otherwise signal that the ingredient is on table
            is[item] = true;
        }

        sem_post(&mutex);
    }
}

// TODO signal handler
int main() {
    sem_init(&mutex, 1);  // for critsec
    sem_init(&smoke, 1);

    for (int i = 0; i < 3; i++) {
        sem_init(&put[i], 0);
        sem_init(&roll[i], 0);
        int err = 0;
        if ((err = pthread_create(&pguardian[i], NULL, &guardian, (void*)(long)i)) ||
            (err = pthread_create(&psmoker[i], NULL, &smoker, (void*)(long)i)))
            exit(err);
    }
    pthread_create(&pagent, NULL, &agent, NULL);

    pthread_join(pagent, NULL);
    exit(EXIT_SUCCESS);
}