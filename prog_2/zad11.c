#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "semaphore.h"

#define NDEBUG  // udefine to enable testing
#include <assert.h>

#define CARTS 5
#define DELAYSCALE 100000  // standard delayscale for delay
#define min(a, b) ((a) < (b) ? (a) : (b))
pthread_t p_village_producer[2], p_village_consumer[2], p_trolley_operator;
// each village has one package terminal for produced items and bell to wait for parcel
sem_t parcel_terminal[2], parcel_bell[2];
sem_t trolley_unloaded;
// proof of concept - payload items and "queue" for produced items as integer numbers
int payload = 0, parcel_queue[2] = {0, 0};
sem_t parcel_pending;  // to signal that some village produced something

inline void delay(int timescale) { usleep(timescale + rand() % timescale); }

// not thread-safe. vID_offset=-1 for message without vID
inline void id_send_msg(int vID, int vID_offset, char* const msg) {
    if (vID_offset >= 0) msg[vID_offset] = vID + '0';
    if (write(STDERR_FILENO, msg, strlen(msg)) < 0) exit(EXIT_FAILURE);
}

// thread-safe (for two village producers)
void parcel_send_item(int vID) {  // AKA produce and send a single item
    char _parcel[] = "village _: submitted item to the parcel post.\n";
    id_send_msg(vID, 8, _parcel);
    parcel_queue[vID]++;
    sem_post(&parcel_pending);
}

// not thread-safe (for two village consumers)
inline void trolley_unload_item(int vID) {  // pop from queue of items
    char _unloaded[] = "village _: unloaded item.\n";
    id_send_msg(vID, 8, _unloaded);
    payload--;  // take from trolley payload
}

void* village_producer_sender_lifecycle(void* data) {
    int vID = (int)(long)(data);
    while (1) {
        sem_wait(&parcel_terminal[vID]);
        for (int i = 0; i < 1 + rand() % CARTS; i++) // to see the max 5 payload effect
            parcel_send_item(vID);
        sem_post(&parcel_terminal[vID]);
        delay(DELAYSCALE);  // random sleep for next item generation
    }
}

// could be split into village_producer/vilage_consumer, but I simply prioritize consuming
void* village_receiver_consumer_lifecycle(void* data) {
    int vID = (int)(long)(data);
    while (1) {
        sem_wait(&parcel_bell[vID]);  // wait till the parcel arrives

        assert(payload >= 0 && payload <= 5);
        while (payload > 0) trolley_unload_item(vID);

        sem_post(&trolley_unloaded);
        delay(DELAYSCALE / 10);  // unloading 10x faster than production for safety:memory
    }
}

// trolley waits for parcel to arrive, and starting from the opposite of last_sender
// makes a one way trip
void* trolley_operator_lifecycle(void* data) {
    static int last_sender = 0;
    char _payload[] = "trolley:_ items in payload. sending to _\n";
    while (1) {
        sem_wait(&trolley_unloaded);  // wait untill trolley is free
        sem_wait(&parcel_pending);    // sleep until any parcel comes

        sem_wait(&parcel_terminal[1 - last_sender]);  // start with other than last sender
        if (parcel_queue[1 - last_sender] > 0) {      // iff there are items to be sent
            payload = min(parcel_queue[1 - last_sender], CARTS);  // don't overload
            parcel_queue[1 - last_sender] -= payload;
        }
        sem_post(&parcel_terminal[1 - last_sender]);

        if (payload > 0) {  // if we have a parcel to deliver from this village
            _payload[39] = '0' + last_sender;
            id_send_msg(payload, 8, _payload);
            delay(DELAYSCALE);                        // make a trip
            sem_post(&parcel_bell[last_sender]);  // signal delivery complete
            last_sender = 1 - last_sender;  // swap the priority (starvation prevention)
        } else {  // otherwise check if we have a parcel from the other village
            sem_post(&parcel_pending);
            sem_post(&trolley_unloaded);
        }
    }
}

int main() {
    sem_init(&trolley_unloaded, 1);  // trolley is unloaded at first
    sem_init(&parcel_pending, 0);    // no pending parcels at first

    for (long i = 0; i < 2; i++) {
        sem_init(&parcel_terminal[i], 1);  // terminals for parcels are open
        sem_init(&parcel_bell[i], 0);      // nothing to unload at first

        int err = 0;
        if ((err = pthread_create(&p_village_consumer[i], NULL,
                                  &village_receiver_consumer_lifecycle, (void*)(long)i)))
            exit(err);
        if ((err = pthread_create(&p_village_producer[i], NULL,
                                  &village_producer_sender_lifecycle, (void*)(long)i)))
            exit(err);
    }
    pthread_create(&p_trolley_operator, NULL, &trolley_operator_lifecycle, NULL);
    pthread_join(p_trolley_operator, NULL);  // end with ^C
    return 0;  // sigmask etc. would be the same as in the 2nd task
}