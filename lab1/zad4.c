/* Imię nazwisko: Maksymilian Debeściak
 * Numer indeksu: 999999
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego dnie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Jak proces wykrywa, że drugi koniec potoku został zamknięty?
 * A: ...
 */

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define err(msg)            \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int fd1[2];  // 1-2 pipe
int fd2[2];  // 2-3 pipe

static void func_1() {
    close(fd1[0]);  // close 1-2 read
    close(fd2[0]);  // close 2-3 read
    close(fd2[1]);  // close 2-3 write
    static int words = 0;
    // generating read for each char is not very optimal but welp ¯\_(ツ)_/¯...
    char buf[256];
    int eof = 0;
    while (!eof) {
        int it = -1;  // buffer iterator
        while (!(eof = !read(STDIN_FILENO, &buf[++it], 1)) && !isspace(buf[it]))
            ;
        buf[it] = '\0';  // assure the string is null terminated
        // printf("[%s](%d)<%d>\n", buf, it, eof ? 1 : 0);
        if (buf[0] != '\0') {
            words++;
            if (!eof)
                if (write(fd1[1], buf, 256) < 0) exit(EXIT_FAILURE);  // pipe write
        }
    }
    close(fd1[1]);  // close pipe => func_2 will get EOF
    fprintf(stderr, "\nwords = %d\n", words);
    exit(EXIT_SUCCESS);  // exit to prevent forking
}

static void func_2() {
    close(fd1[1]);  // close 1-2 write
    close(fd2[0]);  // close 2-3 read
    static int removed = 0;
    char buf[256];
    while (read(fd1[0], &buf, 256) > 0) {
        int place_it = 0, seek_it = -1;
        while (buf[++seek_it] != '\0') {  // assume the string is null terminated
            if (isalnum(buf[seek_it]))
                buf[place_it++] = buf[seek_it];
            else
                removed++;
        }
        buf[place_it] = '\0';  // assure the string is \0 terminated
        //printf("[%s](%d)(%d)\n", buf, place_it, seek_it);
        if (write(fd2[1], &buf, 256) < 0) err("pipe");  // write to pipe
    }
    close(fd1[0]);
    close(fd2[1]);
    fprintf(stderr, "removed = %d\n", removed);
    exit(EXIT_SUCCESS);  // exit to prevent forking
}

static void func_3() {
    close(fd1[1]);  // close 1-2 write
    close(fd1[0]);  // close 1-2 read
    close(fd2[1]);  // close 2-3 write
    static int chars = 0;
    char buf[256];
    while (read(fd2[0], &buf, 256) > 0) {
        int i = -1;
        while (buf[++i] != '\0') chars++;
        buf[i] = ' ';
        if (buf[0] != ' ' && write(STDOUT_FILENO, &buf, i + 1) < 0) exit(EXIT_FAILURE);
        // printf("[%s](%d)\n", buf, i);
        memset(buf, 0, 255);
    }
    close(fd2[0]);
    fprintf(stderr, "chars = %d\n", chars);
    exit(EXIT_SUCCESS);  // exit to prevent forking
}

int main(void) {
    pid_t pids[3];  // [0]=reader, [1]=filter, [2]=writer;
    // void (*functions[3])(void) = {func_1, func_2, func_3};  // function pointers

    if (pipe(fd1) < 0 || pipe(fd2) < 0) err("pipe");

    if ((pids[0] = fork()) == 0) {
        func_1();
    } else {
        if ((pids[1] = fork()) == 0) {
            func_2();
        } else {
            if ((pids[2] = fork()) == 0) {
                func_3();
            } else if (pids[2] != 0) {
                // close parent pipes ....
                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);
                // wait for all processes
                int status, n = 3;
                while (n > 0) {
                    wait(&status);  // wait for any process to finish
                    if (WEXITSTATUS(status) != EXIT_SUCCESS) {
                        kill(pids[0], SIGKILL);
                        kill(pids[1], SIGKILL);
                        kill(pids[2], SIGKILL);
                    } else
                        n--;
                }
                exit(EXIT_SUCCESS);
            }
        }
    }
}
