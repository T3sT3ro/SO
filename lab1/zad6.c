/* Imię nazwisko: Maksymilian Polarczyk
 * Numer indeksu: 300791
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego nie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Dlaczego w pliku Makefile przekazujemy opcję '-Wl,-rpath,ścieżka'
 *    do sterownika kompilatora?
 * A: -Wl,opcja1,opcja2,... przekazuje opcje dla linkera. -rpath,ścieżka potrzebna jest
 * dynamicznemu linkerowi do znajdywania wymaganych libów. Używane gdy ELF linkowany
 * z obiektami współdzielonymi(.so)
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  return EXIT_SUCCESS;
}
