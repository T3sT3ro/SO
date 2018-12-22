#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#undef NDEBUG  // udefine to enable testing
#include <assert.h>

#define CAPACITY 10
#define PASSENGERS 30
#define DELAYSCALE 100000  // standard delayscale for delay
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

pthread_t p_passenger[PASSENGERS], p_trolley;
pthread_mutex_t mutex;
pthread_cond_t not_full, full, helper;  // helper as ride_end and unboard_end
int on_board = 0, can_board = 0, can_unboard = 0;

inline void delay(int timescale) { usleep(timescale + rand() % timescale); }

// not thread-safe. vID_offset=-1 for message without vID
inline void send_msg(int data, int data_offset, char* const msg) {
    if (data_offset >= 0) {
        msg[data_offset] = data / 10 + '0';
        msg[data_offset + 1] = data % 10 + '0';
    }
    if (write(STDERR_FILENO, msg, strlen(msg)) < 0) exit(EXIT_FAILURE);
}

void board(int cID) {
    pthread_mutex_lock(&mutex);

    while (!can_board) pthread_cond_wait(&not_full, &mutex);

    char _board[] = "Passenger __ boarded.\n";
    send_msg(cID, 10, _board);

    can_board = (++on_board != CAPACITY);
    if (on_board == CAPACITY) pthread_cond_signal(&full);  // last one signals trolley

    pthread_mutex_unlock(&mutex);
}

void unboard(int cID) {
    pthread_mutex_lock(&mutex);

    while (!can_unboard) pthread_cond_wait(&helper, &mutex);  // wait till ride end
    if (--on_board == 0) pthread_cond_signal(&helper); // trolly waiting on 'unload'
    char _unboard[] = "Passenger __ unboarded.\n";
    send_msg(cID, 10, _unboard);

    pthread_mutex_unlock(&mutex);
}

void* passenger_lifecycle(void* data) {
    int cID = (int)(long)(data);
    while (1) {
        board(cID);
        unboard(cID);
        delay(DELAYSCALE);  // random sleep for next item generation
    }
}

void load() {
    pthread_mutex_lock(&mutex);

    send_msg(0, -1, "Loading...\n");
    can_board = 1;
    pthread_cond_broadcast(&not_full);  // wake up all waiting for enter
    while (can_board) pthread_cond_wait(&full, &mutex);
    // can_board is false in here
    send_msg(0, -1, "Loading completed.\n");

    pthread_mutex_unlock(&mutex);
}

void run() {
    send_msg(0, -1, "They see me rollin. They waitin'.\n");
    delay(DELAYSCALE);
}

void unload() {
    pthread_mutex_lock(&mutex);
    send_msg(0, -1, "Unboarding...\n");
    can_unboard = 1;
    pthread_cond_broadcast(&helper);  // ride_end broadcast all to unboard()
    while (on_board > 0) pthread_cond_wait(&helper, &mutex);  // wait for all to leave
    // noone on board
    can_unboard = 0;
    send_msg(0, -1, "Unboarding completed.\n");
    pthread_mutex_unlock(&mutex);
}

void* trolley_lifecycle(void* data) {
    while (1) {
        load();
        run();
        unload();
    }
}

int main() {
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&helper, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&p_trolley, NULL, &trolley_lifecycle, NULL);

    for (long i = 0; i < PASSENGERS; i++) {
        int err = 0;
        if ((err = pthread_create(&p_passenger[i], NULL, &passenger_lifecycle,
                                  (void*)(long)i)))
            exit(err);
    }

    pthread_join(p_trolley, NULL);  // end with ^C
    return 0;                       // sigmask etc. would be the same as in the 2nd task
}