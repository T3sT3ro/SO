#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "semaphore.h"

#define NDEBUG  // udefine to enable testing
#include <assert.h>

#define CARTS 5
#define DELAYSCALE 1000000  // standard delayscale for delay
#define min(a, b) ((a) < (b) ? (a) : (b))
pthread_t p_village_producer[2], p_village_consumer[2], p_trolley_operator;
// each village has one package terminal for produced items and bell to wait for parcel
sem_t parcel_inpost[2], parcel_outpost[2], parcel_bell[2];
sem_t parcel_pending;  // to signal that some village produced something. Either 0 or 1
// proof of concept - payload items and "queue" for produced items as integer numbers
int parcel_inpost_queue[2] = {0, 0}, parcel_outpost_queue[2] = {0, 0};

inline void delay(int timescale) { usleep(timescale + rand() % timescale); }

// not thread-safe. vID_offset=-1 for message without vID
inline void send_msg(char* msg) {
    if (write(STDERR_FILENO, msg, strlen(msg)) < 0) exit(EXIT_FAILURE);
}

void* village_producer_sender_lifecycle(void* data) {
    int vID = (int)(long)(data);
    char _submit[34];
    while (1) {
        sem_wait(&parcel_outpost[vID]);  // exclusive access to outpost
        int lim = (rand() % 99) + 1;
        sem_post(&parcel_pending);

        parcel_outpost_queue[vID] += lim;
        snprintf(_submit, sizeof(_submit),
                 "village %d: submit --> %2d items.\n",
                 vID, lim);
        send_msg(_submit);

        sem_post(&parcel_outpost[vID]);
        delay(DELAYSCALE);  // random sleep for next item generation
    }
}

// could be split into village_producer/vilage_consumer, but I simply prioritize consuming
void* village_receiver_consumer_lifecycle(void* data) {
    int vID = (int)(long)(data);
    char _receive[46];
    while (1) {
        sem_wait(&parcel_bell[vID]);    // wait till the parcel arrives
        sem_wait(&parcel_inpost[vID]);  // lock the inbox for exclusive access
        if (parcel_inpost_queue[vID] > 0) {
            snprintf(_receive, sizeof(_receive),
                     "village %d: <-- took %4d items from inpost.\n",
                     vID, parcel_inpost_queue[vID]);
            send_msg(_receive);

            parcel_inpost_queue[vID] = 0;
        }
        sem_post(&parcel_inpost[vID]);
        delay(DELAYSCALE / 10);  // make receive 10x faster
    }
}

// trolley waits for parcel to arrive, and starting from the opposite of last_sender
// makes a one way trip
void* trolley_operator_lifecycle(void* data) {
    int last_sender = 0;
    int payload = 0;
    char _transfer[46];
    while (1) {
        sem_wait(&parcel_pending);  // wait for parcel
        // if in here, one of outposts has queue > 0

        sem_wait(&parcel_outpost[1 - last_sender]);        // starvation prevention
        if (parcel_outpost_queue[1 - last_sender] == 0) {  // if current favoured empty
            last_sender = 1 - last_sender;                 // swap priority
            sem_post(&parcel_outpost[last_sender]);
        } else
            sem_post(&parcel_outpost[1 - last_sender]);

        int vID_from = 1 - last_sender, vID_to = last_sender;

        sem_wait(&parcel_outpost[vID_from]);  // locking the outposts
        payload = min(parcel_outpost_queue[vID_from], CARTS);
        parcel_outpost_queue[vID_from] -= payload;
        if (parcel_outpost_queue[vID_from] > 0)
            sem_post(&parcel_pending);
        sem_post(&parcel_outpost[vID_from]);
        //if (payload == 0) continue;

        delay(DELAYSCALE / 60);  // transfer time
        snprintf(_transfer, sizeof(_transfer),
                 "\x1B[34;12m[transfer]: %d --[%2d]--> %d\x1B[0m\n",
                 vID_from, payload, vID_to);
        send_msg(_transfer);

        sem_wait(&parcel_inpost[vID_to]);  // lock the inpost
        parcel_inpost_queue[vID_to] += payload;
        sem_post(&parcel_inpost[vID_to]);
        sem_post(&parcel_bell[vID_to]);  // signal parcel delivery

        payload = 0;
        last_sender = 1 - last_sender;  // swap the priority
    }
}

// to stop the program, ^C
int main() {
    sem_init(&parcel_pending, 0);  // no pending parcels at first

    pthread_create(&p_trolley_operator, NULL, &trolley_operator_lifecycle, NULL);
    for (long i = 0; i < 2; i++) {
        sem_init(&parcel_inpost[i], 1);   // terminals for parcels are open
        sem_init(&parcel_outpost[i], 1);  // ^^^
        sem_init(&parcel_bell[i], 0);     // nothing to receive at first

        int err = 0;
        if ((err = pthread_create(&p_village_consumer[i], NULL,
                                  &village_receiver_consumer_lifecycle, (void*)(long)i)))
            exit(err);
        if ((err = pthread_create(&p_village_producer[i], NULL,
                                  &village_producer_sender_lifecycle, (void*)(long)i)))
            exit(err);
    }

    pthread_join(p_trolley_operator, NULL);
    return 0;
}