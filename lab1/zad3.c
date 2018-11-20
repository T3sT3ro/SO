/* Imię nazwisko: Maksymilian Polarczyk
 * Numer indeksu: 300791
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego nie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Czemu nie musisz synchronizować dostępu do zmiennych współdzielonych?
 * A: Ponieważ korutyny nie wywołują się równolegle a współbieżnie, dopóki nie
 * zmienimy kontekstu manualnie.
 */

/*
Zadanie 3 (2). Korzystając z rodziny procedur makecontext(3) utwórz
współprogramy (ang. coroutines)  realizujące następujące funkcje:
1. Pobierz z «stdin» słowo oddzielone spacjami. Zliczaj ilość pobranych słów.
Przełącz na #2.
2. Zlicz i usuń ze słowa znaki niebędące znakami alfanumerycznymi isalnum(3).
Przełącz na #3.
3. Wydrukuj słowo na «stdout». Zliczaj znaki w słowach. Przełącz na #1.

Kiedy współprogram #1 natrafi na koniec pliku pozostałe współprogramy mają
wydrukować wartość liczników, po czym proces ma zakończyć swe działanie.

Stan programu możesz przechowywać w zmiennych globalnych – nie musisz martwić
się synchronizacją. Możesz założyć, że na wejściu pojawiają się tylko znaki
ASCII i żadne słowo nie jest dłuższe niż 255 bajtów. Dane należy wczytywać i
zapisywać odpowiednio wywołaniami read(2) i write(2). Zawartość liczników należy
wydrukować na «stderr» z użyciem funkcji printf(2).

*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

typedef int bool;
#define true 1
#define false 0

#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

static ucontext_t uctx_main, uctx_f1, uctx_f2, uctx_f3;
static char buf[256];
static int eof = false;

static void func_1() {
    static int words = 0;
    // generating read for each char is not very optimal but welp ¯\_(ツ)_/¯...
    while (!eof) {
        int it = -1;  // buffer iterator
        while (!(eof = !read(STDIN_FILENO, &buf[++it], 1)) && !isspace(buf[it]))
            ;
        buf[it] = '\0';  // assure the string is null terminated
        // printf("[%s](%d)<%d>\n", buf, it, eof ? 1 : 0);
        if (buf[0] != '\0') { // eliminate 0 length words
            words++;
            if (!eof) swapcontext(&uctx_f1, &uctx_f2);
        }
    }

    fprintf(stderr, "words = %d\n", words);
    swapcontext(&uctx_f1, &uctx_f2);
}

static void func_2() {
    static int removed = 0;

    while (!eof) {
        int place_it = 0, seek_it = -1;
        while (buf[++seek_it] != '\0') {  // assume the string is null terminated
            if (isalnum(buf[seek_it]))
                buf[place_it++] = buf[seek_it];
            else
                removed++;
        }
        buf[place_it] = '\0';  // assure the string is \0 terminated
        // printf("[%s](%d)(%d)<%d>\n", buf, place_it, seek_it, eof ? 1 : 0);
        swapcontext(&uctx_f2, &uctx_f3);
    }

    fprintf(stderr, "removed = %d\n", removed);
    swapcontext(&uctx_f2, &uctx_f3);
}

static void func_3() {
    static int chars = 0;
    while (!eof) {
        int i = -1;
        while (buf[++i] != '\0') chars++;
        buf[i] = '\n';  // string is now \n terminated
        if (!write(STDOUT_FILENO, &buf, i + 1)) exit(EXIT_FAILURE);
        // printf("[%s](%d)<%d>\n", buf, i, eof ? 1 : 0);
        memset(buf, 0, 255);

        swapcontext(&uctx_f3, &uctx_f1);
    }

    fprintf(stderr, "chars = %d\n", chars);
}

int main() {
    char f1_stack[16384], f2_stack[16384], f3_stack[16384];

    if (getcontext(&uctx_f1) == -1) handle_error("getcontext");

    uctx_f1.uc_stack.ss_sp = f1_stack;
    uctx_f1.uc_stack.ss_size = sizeof(f1_stack);
    uctx_f1.uc_link = &uctx_main;
    makecontext(&uctx_f1, func_1, 0);

    if (getcontext(&uctx_f2) == -1) handle_error("getcontext");
    uctx_f2.uc_stack.ss_sp = f2_stack;
    uctx_f2.uc_stack.ss_size = sizeof(f2_stack);
    uctx_f2.uc_link = &uctx_f3;
    makecontext(&uctx_f2, func_2, 0);

    if (getcontext(&uctx_f3) == -1) handle_error("getcontext");
    uctx_f3.uc_stack.ss_sp = f3_stack;
    uctx_f3.uc_stack.ss_size = sizeof(f3_stack);
    uctx_f3.uc_link = &uctx_f1;
    makecontext(&uctx_f3, func_3, 0);

    if (swapcontext(&uctx_main, &uctx_f1) == -1) handle_error("swapcontext");

    exit(EXIT_SUCCESS);
}
