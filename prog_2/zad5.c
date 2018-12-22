#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>

#undef NDEBUG  // udefine to enable testing
#include <assert.h>

#define CAPACITY 5
#define CUSTOMERS 20
#define DELAYSCALE 100000  // standard delayscale for delay
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
const char* shm_name = "/mem";
const char* mutex_name = "/mutex";
const char* gate_name = "/gate";
int *eating, *waiting, *must_wait;
sem_t *mutex, *gate;

int shm_fd;  // shared memory file descriptor
void* shm;

pid_t p_passenger[CUSTOMERS];

void live(int cID) {
    shm_fd = shm_open(shm_name, O_RDWR, 0666);;
    mutex = sem_open(mutex_name, O_RDWR, 0666, 1);
    gate = sem_open(gate_name, O_RDWR, 0666, 1);
    while (1) {
        // wait on gate
        // wait on mutex
        // if ++eating != max: signal gate, must_wait = 1
        // ...critsec
        // if --eating == 0: signal mutex, must_wait = 0
    }
}

int main() {
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, 16);
    shm = mmap(0, 16, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    eating = &((int*)shm)[0];
    waiting = &((int*)shm)[1];
    must_wait = &((int*)shm)[2];
    mutex = sem_open(mutex_name, O_CREAT | O_RDWR, 0666, 1);
    gate = sem_open(gate_name, O_CREAT | O_RDWR, 0666, 1);


    for (long i = 0; i < CUSTOMERS; i++) {
        pid_t pid;
        if ((pid = fork()) == 0) {  // child
        }
    }

    return 0;  // sigmask etc. would be the same as in the 2nd task
}