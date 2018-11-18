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
 * A: jak za główny proces rozumiemy init, to jak jest ustawione PR_SET_CHILD_SUBREAPER 
 * to przejmie go pierwszy ancestor który ma ustawione PR_SET_CHILD_SUBREAPER. Jeśli przez
 * główny proces rozumiemy proces który wywoał forka, to ojcem stanie się proces init.
 * 
 */

/*
Zadanie 2. Napisz program, który utworzy proces *sierotę*. Uruchom proces
*żniwiarza* (ang. reaper) przy użyciu *prctl(2)*. Używając wywołania
*fork(2)* utwórz kolejno syna i wnuka. Następnie zakończ działanie dziecka tym
samym osieracając wnuka. Uruchom polecenie «ps», aby wskazać kto przygarnął
sierotę.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) { 
    pid_t pid;
    prctl(PR_SET_CHILD_SUBREAPER, 1); // set parent subreaper

    if((pid = fork()) == 0){ // son
        if(fork() != 0)
            exit(0); // middle child - terminate
        else 
            sleep(2); // grandson
    } else { // father
        sleep(1);
        char* const _ps_argv[] = {"ps", "-o", "pid,ppid,state,cmd", NULL};
        execve("/bin/ps", _ps_argv, NULL);
    }
    return EXIT_SUCCESS;
}
