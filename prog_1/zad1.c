/* Imię nazwisko: Maksymilian Polarczyk
 * Numer indeksu: 300791
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego nie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Zdefiniuj proces "zombie".
 * A: Proces, którego wykonanie zostało zakończone(terminated), ale dalej nie
 * został on zamknięty(nie obsłużony przez rodzica). Np. w wypadku, gry proces
 * potomny ma niezwolnione zasoby. Do stanu zombie przechodzi się ze stanu
 * executing.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    pid_t pid;

    if ((pid = fork()) < 0) {  // error while forking
        perror(NULL);
        exit(EXIT_SUCCESS);  // needed for wait in parent
    }

    if (argc > 1 && strcmp(argv[1], "--bury") == 0) {  // ignore SIGCHLD
        struct sigaction sa;
        sa.sa_handler = SIG_IGN; // fake ignore handler for SIGCHLD
        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
            perror(0);
            exit(1);
        };
    }

    if (pid == 0)  // child
        exit(EXIT_SUCCESS);

    sleep(1);  // fix for 'Signal 17 (CHLD) caught by ps (3.3.12).
               // ps:ps/display.c:66: please report this bug'

    char* const _ps_argv[] = {"ps", "-o", "pid,ppid,state,cmd", NULL};
    if (execve("/bin/ps", _ps_argv, NULL)) {
        perror("ps failed.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
