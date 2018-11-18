/* Imię nazwisko: Maksymilian Polarczyk
 * Numer indeksu: 300791
 *
 * Oświadczam, że:
 *  - rozwiązanie zadania jest mojego autorstwa,
 *  - jego kodu źródłowego nie będę udostępniać innym studentom,
 *  - a w szczególności nie będę go publikować w sieci Internet.
 *
 * Q: Czemu nie musisz synchronizować dostępu do zmiennych współdzielonych?
 * A: Bo to ja tu jestem programistą i mogę robić co mi się żewnie podoba.
 */

/*
Zadanie 3 (2). Korzystając z rodziny procedur makecontext(3) utwórz współprogramy
(ang. coroutines)  realizujące następujące funkcje:
1. Pobierz z «stdin» słowo oddzielone spacjami. Zliczaj ilość pobranych słów. Przełącz na #2.
2. Zlicz i usuń ze słowa znaki niebędące znakami alfanumerycznymi isalnum(3). Przełącz na #3.
3. Wydrukuj słowo na «stdout». Zliczaj znaki w słowach. Przełącz na #1.
Kiedy współprogram #1 natrafi na koniec pliku pozostałe współprogramy mają wydrukować wartość
liczników, po czym proces ma zakończyć swe działanie.
Stan programu możesz przechowywać w zmiennych globalnych – nie musisz martwić się synchronizacją.
Możesz założyć, że na wejściu pojawiają się tylko znaki ASCII i żadne słowo nie jest dłuższe niż 255 bajtów.
Dane należy wczytywać i zapisywać odpowiednio wywołaniami read(2) i write(2). Zawartość liczników
należy wydrukować na «stderr» z użyciem funkcji printf(2).

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ucontext.h>

static void func_1() {
  static int words = 0;

  while (1) {
  }

  fprintf(stderr, "words = %d\n", words);
}

static void func_2() {
  static int removed = 0;

  while (1) {
  }

  fprintf(stderr, "removed = %d\n", removed);
}

static void func_3() {
  static int chars = 0;

  while (1) {
  }

  fprintf(stderr, "chars = %d\n", chars);
}

int main() {
  return EXIT_SUCCESS;
}
