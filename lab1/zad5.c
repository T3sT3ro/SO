/* Imię nazwisko: Maksymilian Debeściak
 * Numer indeksu: 999999
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego dnie będę udostępniać innym studentom,
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
#include <ucontext.h>
#include <unistd.h>

static void sighandler(int signum, siginfo_t *info, void *_context) {
    ucontext_t *context = (ucontext_t *)_context;
    flockfile(stderr);
    fprintf(stderr, "[%p]<RIP:%llx><RSP:%llx> :\n\t%s\n", info->si_addr,
             context->uc_mcontext.gregs[REG_RIP], context->uc_mcontext.gregs[REG_RSP],
             info->si_code == SEGV_MAPERR
                 ? "Address not mapped to object."
                 : info->si_code == SEGV_ACCERR ? "Invalid permissions for mapped object."
                                                : "Welp, SEGFAULT returned other error.");
    flockfile(stderr);    
    void *buffer[512];
    const int calls = backtrace(buffer, sizeof(buffer) / sizeof(void *));
    backtrace_symbols_fd(buffer, calls, 1);
    exit(EXIT_FAILURE);
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

    if (strcmp(argv[1], "--maperr") == 0) {
        int *ptr = (int *)1;
        *ptr = 3;
    } else {
        int *ptr = (int *)&main;
        *ptr = 17;
    }
}
