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
    // still better than logics of eating 1,5 of words and than shifting etc.
    while (!eof) {
        int it = -1;  // buffer iterator
        int consumed = 0;
        while ((consumed = read(STDIN_FILENO, &buf[++it], 1)) > 0 && !isspace(buf[it]))
            ;
        if (consumed < 0) handle_error("reader read error");
        eof = !consumed;
        buf[it] = '\0';  // assure the string is null terminated
        // printf("[%s](%d)<%d>\n", buf, it, eof ? 1 : 0);
        if (buf[0] != '\0') { // eliminate 0 length words
            words++;
            if (!eof) swapcontext(&uctx_f1, &uctx_f2);
        }
    }

    fprintf(stderr, "\nwords = %d\n", words);
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
        buf[i] = ' ';  // string is now ' ' terminated
        // eliminate filtered out words
        if (buf[0] != ' ' && !write(STDOUT_FILENO, &buf, i + 1)) exit(EXIT_FAILURE);
        // printf("[%s](%d)<%d>\n", buf, i, eof ? 1 : 0);
        memset(buf, 0, 255);

        swapcontext(&uctx_f3, &uctx_f1);
    }

    fprintf(stderr, "chars = %d\n", chars);
}

#define STSZ 16384
int main() {
    char f1_stack[STSZ], f2_stack[STSZ], f3_stack[STSZ];

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
    uctx_f3.uc_link = &uctx_f1; // repeat untill eof
    makecontext(&uctx_f3, func_3, 0);

    if (swapcontext(&uctx_main, &uctx_f1) == -1) handle_error("swapcontext");

    exit(EXIT_SUCCESS);
}
