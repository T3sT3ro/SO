/* Imię nazwisko: Maksymilian Debeściak
 * Numer indeksu: 999999
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego nie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Zdefiniuj proces "sierotę".
 * A: Proces sierota nie posiada ojca, ale sam działa. W Unixach taki proces
 * jest adoptowany przez przez specjalny proces 'init' systemu.
 *
 * Q: Co się stanie, jeśli główny proces nie podejmie się roli żniwiarza?
 * A: ...
 */

/*
Zadanie 2. Napisz program, który utworzy proces sierotę. Uruchom proces
żniwiarza (ang. reaper) przy użyciu prctl(2). Używając wywołania
fork(2) utwórz kolejno syna i wnuka. Następnie zakończ działanie dziecka tym
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

int main(void) { return EXIT_SUCCESS; }
