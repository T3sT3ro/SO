#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#undef NDEBUG  // udefine to enable testing
#include <assert.h>

#define CAPACITY 5
#define CUSTOMERS 20
#define DELAYSCALE 1000000  // standard delayscale for delay
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
const char* shm_name = "/mem";
const char* mutex_name = "/mutex";
const char* gate_name = "/gate";
int *eating, *must_wait;  //*waiting useless
sem_t *mutex, *gate;
int *cnt;

int shm_fd;  // shared memory file descriptor
void* shm;

pid_t p_passenger[CUSTOMERS];

inline void delay(int timescale) { usleep(timescale + rand() % timescale); }

void acquire() {
    sem_wait(gate);
    sem_wait(mutex);
    if (++*eating == CAPACITY)
        *must_wait = 1;
    else
        sem_post(gate);
    sem_post(mutex);
}

void release() {
    sem_wait(mutex);
    --*eating;
    if (must_wait && *eating == 0) {
        for (int i = 0; i < CAPACITY; i++)
            sem_post(gate);
        *must_wait = 0;
    } else
        sem_post(gate);
    sem_post(mutex);
}

void cleanup(int _) {
    sem_post(mutex);
    sem_post(gate);
    exit(0);
}

void live(int cID) {
    signal(SIGINT, cleanup);
    shm_fd = shm_open(shm_name, O_RDWR, 0666);
    mutex = sem_open(mutex_name, O_RDWR, 0666, 1);
    gate = sem_open(gate_name, O_RDWR, 0666, 1);
    char msg[] = "[__][__][__][__][__][__][__][__][__][__][__][__][__][__][__][__][__][__][__][__]\r";
    int msg_offset=1, msg_step=4;
    while (1) {
        acquire();
        ++*cnt;

        snprintf(&msg[msg_offset+cID*msg_step], 3, "%02d", cID);
        msg[msg_offset+cID*msg_step+2] = ']';
        if (write(STDERR_FILENO, msg, strlen(msg)) < 0) exit(1);
        delay(DELAYSCALE);
        assert(*cnt <= CAPACITY);
        --*cnt;
        release();
        delay(DELAYSCALE*2);
    }
}

void sighandler(int _) {
    shm_unlink(shm_name);
    sem_unlink(mutex_name);
    sem_unlink(gate_name);
    printf(" ended normally, memory unlinked/\n");
    exit(EXIT_SUCCESS);
}

int main() {
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (ftruncate(shm_fd, 16)) {
        perror("ftruncate");
        exit(1);
    }
    shm = mmap(0, 16, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    eating = &((int*)shm)[0];
    //waiting = &((int*)shm)[1];
    cnt = &((int*)shm)[1];
    must_wait = &((int*)shm)[2];
    if ((mutex = sem_open(mutex_name, O_CREAT, 0644, 1)) == SEM_FAILED ||
        (gate = sem_open(gate_name, O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("semaphore initialization");
        exit(1);
    }

    signal(SIGINT, sighandler);

    pid_t pids[CUSTOMERS];
    for (long i = 0; i < CUSTOMERS; i++)
        if ((pids[i] = fork()) == 0)  // child
            live(i);

    int n = CUSTOMERS;
    while (n--)
        wait(NULL);
    return 0;  // sigmask etc. would be the same as in the 2nd task
}