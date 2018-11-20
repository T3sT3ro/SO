/* Imię nazwisko: Maksymilian Polarczyk
 * Numer indeksu: 300791
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego nie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Czemu procedura printf nie jest wielobieżna, a snprintf jest?
 * A: snprintf pisze do bufora podanego przez użytkownika. O ile zadbamy o osobny
 * bufor dla wątków, nie będzie wyścigów. Printf pisze do jednego bufora: stdout, więc
 * może wystąpić wyścig.
 */

#define _GNU_SOURCE
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <ucontext.h>
#include <unistd.h>

#define err(msg)            \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

static void sighandler(int signum, siginfo_t *info, void *_context) {
    ucontext_t *context = (ucontext_t *)_context;
    // assure thread-safe reentrant stderr write
    char str[256];
    snprintf(str, 256, "[%p]<RIP:%llx><RSP:%llx> :\n\t%s\n", info->si_addr,
             context->uc_mcontext.gregs[REG_RIP], context->uc_mcontext.gregs[REG_RSP],
             info->si_code == SEGV_MAPERR
                 ? "Address not mapped to object."
                 : info->si_code == SEGV_ACCERR ? "Invalid permissions for mapped object."
                                                : "Welp, SEGFAULT returned other error.");
    if (write(STDERR_FILENO, str, strlen(str)) < 0) err("error in handler");

    void *buffer[512];
    const int calls = backtrace(buffer, sizeof(buffer) / sizeof(void *));
    backtrace_symbols_fd(buffer, calls, 1);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_sigaction = sighandler;   // handler assign
    sa.sa_flags = SA_SIGINFO;       // pass context and info to handler
    sigaction(SIGSEGV, &sa, NULL);  // handler register
    if (argc != 2 ||
        (strcmp(argv[1], "--maperr") != 0 && strcmp(argv[1], "--accerr") != 0)) {
        fprintf(stderr, "Run with either --maperr or --accerr parameter.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "--maperr") == 0) {  // unmappind the pointer - self explainatory
        int *ptr = mmap(NULL, 1, PROT_READ, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
        munmap(ptr, 1);
        *ptr = 7;
    } else {  // readonly - mmap without PROT_WRITE
        int *ptr = mmap(NULL, 1, PROT_READ, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
        *ptr = 13;
    }
}
