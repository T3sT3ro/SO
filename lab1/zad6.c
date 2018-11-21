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

void * lib_handle;

int main(void) {
  lib_handle = dlopen("libmystr.so", RTLD_LAZY);
  if(!lib_handle){
    fprintf(stderr, "Error while opening libmystr.so: %s\n", dlerror());
    exit(EXIT_FAILURE);
  }
  char * buf[512], *set[128];
  scanf("%s %s", &buf, &set);

  printf("%d, %s", dlsym(lib_handle, "strdrop")(buf, "sens"), buf);
  return EXIT_SUCCESS;
}
