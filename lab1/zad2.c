/* Imię nazwisko: Maksymilian Polarczyk
 * Numer indeksu: 300791
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego nie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Zdefiniuj proces "sierotę".
 * A: Proces sierota to taki proces, któremu umarł/zakończył się ojciec,
 * ale sam jest jeszcze żywy. W Unixach taki proces
 * jest adoptowany przez przez specjalny proces 'init' systemu.
 *
 * Q: Co się stanie, jeśli główny proces nie podejmie się roli żniwiarza?
 * A: Jeśli przez główny proces rozumiemy proces który wywoła forka, to ojcem stanie się
 * proces init(no chyba że któryś proces pomiędzy głównym a sierotą będzie żniwiażem).
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// set method of synchronization to sleep(poor quality) or pipes(better)
#define SLEEP_SYNCHRONIZATION 0

// version 1
void sleep_synced() {
    pid_t pid;
    if ((pid = fork()) == 0) {  // son
        if (fork() != 0)
            exit(0);  // middle child - terminate
        else
            sleep(2);  // grandson
    } else {           // father
        sleep(1);
        char* const _ps_argv[] = {"ps", "-o", "pid,ppid,state,cmd", NULL};
        execve("/bin/ps", _ps_argv, NULL);
    }
}

// version 2
void pipe_synced() {
    pid_t pid;
    int g_p[2], p_g[2];                      // 2 pipes for synchronization
    if (pipe(g_p) == -1 || pipe(p_g) == -1)  // g_p = granchild->parent, p_g->par.->grand.
        exit(EXIT_FAILURE);
    // 2 pipes used to communicate: (parent -> grandchild) and (grandchild -> parent)
    // parent blocks on read untill grandchid writes (aka signal iff grandchild created)
    // grandchild blocks on read untill parent closes write end with execve (O_CLOEXE)
    // pipe from parent to grandchild is closed after execve (descriptor passed to execve)
    char placeholder = 0x00;

    if ((pid = fork()) == 0) {  // son
        if (fork() != 0) {
            exit(0);  // middle child - terminate
            // son closes it's pipe's ends on exit
            exit(EXIT_SUCCESS);  // parents waits() for child
        } else {                 // grandson
            close(g_p[0]);       // close reading end of first pipe
            close(p_g[1]);       // close writing end of O_CLOEXE pipe

            // signaling parent that grandchild was created ( parent wil unblock in read)
            close(g_p[1]);
            // block untill parent end closed
            if (0 > read(p_g[0], &placeholder, 1)) exit(EXIT_FAILURE);
            exit(EXIT_SUCCESS);
        }
    } else {                    // father
        waitpid(pid, NULL, 0);  // wait for child(middle) to terminate
        close(g_p[1]);          // close writing end of first pipe
        close(p_g[0]);          // close reading end of O_CLOEXE pipe
        // block untill grandchild signals
        if (0 > read(g_p[0], &placeholder, 1)) exit(EXIT_FAILURE);
        // pipe descriptor passed to process in execve - closed after ps ends
        char* const _ps_argv[] = {"ps", "-o", "pid,ppid,state,cmd", NULL};
        if (execve("/bin/ps", _ps_argv, NULL) == -1) exit(EXIT_FAILURE);
    }
}

int main(void) {
    prctl(PR_SET_CHILD_SUBREAPER, 1);  // set parent subreaper
    char buf[32];  // pid
    snprintf(&buf[0], 32, "Parent pid: %d\n", getpid());
    if (0 > write(STDERR_FILENO, &buf[0], strlen(buf))) exit(EXIT_FAILURE);

    if (SLEEP_SYNCHRONIZATION)
        sleep_synced();
    else
        pipe_synced();
}
