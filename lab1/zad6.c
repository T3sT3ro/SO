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
#include <string.h>
#include <unistd.h>

#define err(msg)            \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

void *lib_handle;
int (*strdrop)(char *, const char *);
int (*strcnt)(const char *, const char *);

void link_libmystr() {
    lib_handle = dlopen("libmystr.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "Error while opening libmystr.so: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();  // clear any existing error
    fprintf(stderr, "> pmap before dlopen():\n");

    // load functions
    strdrop = (int (*)(char *, const char *))dlsym(lib_handle, "strdrop");
    strcnt = (int (*)(const char *, const char *))dlsym(lib_handle, "strcnt");
    fprintf(stderr, "> pmap after dlopen():\n");
}

#define ALNUM "abcdefghijklmnopqrstuvwxyz0123456789"
#define N 36

int test() {
    char s1[0 + 1] = "";
    char s2[26 + 10 + 1] = ALNUM;

    if (strdrop(&s1[0], "") != 0 && s1[0] != '\0') return 1;  // drop on empty
    // drop of empty set
    if (strdrop(&s2[0], &s1[0]) != N || strcmp(ALNUM, &s2[0]) != 0) return 1;
    // drop of itself
    if (strdrop(&s2[0], ALNUM) != 0 || strcmp(&s2[0], "") != 0) return 1;
    // drop on empty
    if (strdrop(&s1[0], ALNUM) != 0 || strcmp(&s1[0], "") != 0) return 1;

    // count on empty and empty set
    if (strcnt("", ALNUM) != 0 || strcnt("", "")) return 1;
    // count on itself is itselfs length
    if (strcnt(ALNUM, ALNUM) != N) return 1;
    // count shouldn't depend on the order of set
    if (strcnt("0123456789", "9876543210") != 10) return 1;
    return 0;
}

int main(void) {
    char cmd[15];  // system command `pmap <pid>`
    snprintf(cmd, 15, "pmap %d", getpid());

    if (system(cmd) != 0) err("pmap execute error before dlopen");
    link_libmystr();
    if (system(cmd) != 0) err("pmap execute error after dlopen");

    /* More info for this cast and POSIX in `man dlopen` code example */

    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    if (test()) {
        perror("[X] Some tests failed.\n");
        exit(EXIT_FAILURE);
    } else
        printf("[O] All tests passed.\n");

    char buf[512], set[128];
    printf("[>] Input string and set of characters or exit with ^D.\n");
    if (fscanf(stdin, "%512s %128s", &buf[0], &set[0]) < 0) return 0;
    printf("count: (%d) `%s`\n", strcnt(buf, set), buf);
    printf("drop:  (%d) `%s`\n", strdrop(buf, set), buf);
    return 0;
}
