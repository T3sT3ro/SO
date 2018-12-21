#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "semaphore.h"

#define NDEBUG   // udefine to enable testing
#ifndef NDEBUG
#include <assert.h>
#endif

// it's called the barbershop problem...
#define SEATS 5  // don't set to higher than 99
#define CLIENTS 2 * SEATS
#define DELAYSCALE 100000 // standard delayscale for delay


pthread_t pbarber, pclient[CLIENTS];
sem_t mutex, customer, barber;  // for critsection, to wait for customers and barber
sem_t hair_cut, customer_paid;  // for barber to cut only customer one at a time
int occupied = 0;

// act as grow_hair() and cut_hair()
static inline void delay(int timescale) { usleep(timescale + rand() % timescale); }

// only one at a time should invoke this function. First char is the thread ID
static inline void id_send_msg(int phID, char* const msg) {
    msg[0] = phID / 10 + '0';
    msg[1] = phID % 10 + '0';
    if (write(STDERR_FILENO, msg, strlen(msg)) < 0) exit(EXIT_FAILURE);
}

void* barber_lifecycle(void* data) {
    while (1) {
        sem_wait(&customer);
        sem_post(&barber);
        delay(DELAYSCALE);
        sem_post(&hair_cut);  // cut only one at a time
        sem_wait(&customer_paid); // don't cut the next one until this one pays
    }
}

void* client_lifecycle(void* data) {
    int id = (int)(long)(data);
    char _cut[] = "__ yey, my turn!\n";
    char _sweet[] = "__ is now nicely cut. Sweet!\n";
    char _retreat[] = "__ Oh no, the barbershop is loaded! I'll come later.\n";
    char _enque[] = "__ I have time, I'll wait in the queue.\n";
    while (1) {
        sem_wait(&mutex);
        if (occupied == SEATS) {
            id_send_msg(id, _retreat);
            sem_post(&mutex);
            delay(DELAYSCALE);  // go for a coffee meanwhile
            continue;           // retreat !!!
        } else {
            occupied++;
            id_send_msg(id, _enque);
            sem_post(&mutex);
        }
        sem_post(&customer);
        sem_wait(&barber);

#ifndef NDEBUG
        static int inside_critsec = 0;
        inside_critsec++;
        assert(inside_critsec == 1);
#endif

        id_send_msg(id, _cut);
        delay(DELAYSCALE);
        id_send_msg(id, _sweet);

#ifndef NDEBUG
        assert(inside_critsec == 1);
        inside_critsec--;
#endif

        sem_wait(&hair_cut);

        sem_wait(&mutex);
        occupied--;
        sem_post(&mutex);

        sem_post(&customer_paid);
        delay(DELAYSCALE);  // Meet me again when your hair grow long as Merlin's beard!
    }
}

int main() {
    sem_init(&mutex, 1);
    sem_init(&customer, 0);
    sem_init(&barber, 0);
    sem_init(&hair_cut, 0);
    sem_init(&customer_paid, 0);

    pthread_create(&pbarber, NULL, &barber_lifecycle, NULL);
    for (int i = 0; i < CLIENTS; i++) {
        int err = 0;
        if ((err = pthread_create(&pclient[i], NULL, &client_lifecycle, (void*)(long)i)))
            exit(err);
    }

    pthread_join(pbarber, NULL);  // brutal end with ^C, the signal mask and handler
    return 0;                     // would be the same as in the 8th task
}