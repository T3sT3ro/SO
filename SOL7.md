# SO Lista 7

* Tannenbaum: 3.2, 10.4;
* Stallings: 7.2;

**UWAGA!** W trakcie prezentacji należy być gotowym do zdefiniowania pojęć oznaczonych **wytłuszczoną** czcionką. Dla każdej
metody przydziału pamięci, która pojawi się w poniższych zadaniach, prowadzący zajęcia może wymagać by student wyjaśnił:

* jak wygląda struktura danych przechowująca informację o zajętości bloków?
* jak przebiegają operacje `alloc` i `free`?
* jaka jest oczekiwana złożoność czasowa powyższych operacji?
* jaki jest narzut (ang. *overhead*) pamięciowy **metadanych**?
* jaki jest maksymalny rozmiar **nieużytków**?
* czy w danym przypadku **fragmentacja wewnętrzna** lub **zewnętrzna** jest dużym problemem?  

> **Założenie!** Twoje algorytmy zarządzania pamięcią mogą korzystać tylko i wyłącznie z uprzednio przydzielonych ciągłych
obszarów pamięci – również do przechowywania dynamicznych struktur danych. Jakiekolwiek mechanizmy przydziału pamięci
poza tymi obszarami są niedozwolone!

---

**Zadanie 1.** Biblioteczny algorytm przydziału pamięci dysponuje ciągłym obszarem 50 słów maszynowych. Dostarcza funkcji o sygnaturach `malloc: size -> @id` i `free: @id -> unit`. Implementacja wykorzystuje dwukierunkową listę wolnych bloków posortowanych względem adresu. Wyszukiwanie wolnych bloków działa zgodnie z polityką **best-fit**. Operacja zwalniania **gorliwie złącza** bloki. Zasymuluj działanie alokatora dla poniższego ciągu żądań:  
5 14 20 4 @2 @1 @3 7  
> Wskazówka: Przejrzyj poprawione slajdy do wykładu. Rozwiąż zadanie podobnie, ale po jednym kroku na operację.

---

1. ***best-fit***: szuka minimum po pasujących obszarach.  
2. ***gorliwe złączanie bloków***: przy deallokacji następuje złączenie obszaru z poprzednim i następnym wolnym, jeśli są bezpośrednimi sąsiadami.

* *malloc*: znajdź pierwszy blok z pola `FIRST-FREE`, sprawdź czy się mieści, jeśli tak to zapamiętaj rozmiar i idź do końca aż znajdziesz minimum; złożoność: 𝐎(𝐧)
* *free*: idź pod adres, zmień `size := -size` i wepnij na listę wolnych, sprawdź, czy następnik jest bezpośrednim sąsiadem i złącz jeśli tak, analogicznie z poprzednikiem. złożoność: `𝐎(𝐧)`, ze względu na wpinanie na listę wolnych bloków.
* *meta*: 3 pola dla każdego wolnego obszaru, jedno dla każdego zajętego. No słabo, bo jakbyśmy chcieli dane rozmiaru 1 słowa alokować to mksymalnie może być ≈50/2;
* *nieużytki*: na wykładzie było, że best-fit i first-fit mają `𝐎(𝐌 log₂𝐧)`, gdzie 𝐌=maksymalny sumaryczny rozmiar przydzielonego miejsca, 𝐧=maksymalny rozmiar przydzielanego bloku;
* *fragmentacja*: wewnętrzna-brak(?), zewnętrzna - no tak, bo nie mamy spójnych danych bo zawsze wpasowujemy się w najmniejszy wolny obszar. Potencjalnie możemy mieć bardzo dużo dziur i wiele małych obszarów.
    > Internal fragmentation is the wasted space within each allocated block because of rounding up from the actual requested allocation to the allocation granularity.  External fragmentation is the various free spaced holes that are generated in either your memory or disk space. External fragmented blocks are available for allocation, but may be too small to be of any use.

---

**Zadanie 2.** Rozważmy listowy algorytm zarządzania pamięcią z polityką **first-fit**, gdzie wolne bloki posortowano po rosnącym rozmiarze. W jakim celu wykorzystuje się **scalanie** (ang. coalescing) wolnych bloków? Porównując do first-fit z sortowaniem po adresie bloku – **jak wybór polityki przydziału miejsca** (ang. placement policy ) wpłynął na algorytm zwalniania bloku? Jaki wpływ na algorytm przydziału miałaby **leniwa strategia scalania**?

---

* ***first-fit***: alokuje pierwszy wolny blok w który się wpasujemy rozmiarem.
* ***scalanie***: srsly? mimo że było w poprzednim zadaniu? ... Scala się, żeby mieć potem znowu miejsce na większe bloki, Bo inaczej byśmy mogli ciągle alokować tylko coraz mniejsze i mniejsze.
* ***polityka przydziału miejsca***: to inaczej metoda umieszczania nowych bloków - {first, worst, first, next, quick} fit. No niestety w tym wypadku (first-fit posortowane po rozmiarach a nie kolejności) problem ze zwalnianiem jest taki, że nie potrafimy bezpośrednio powiedzieć który to jest nasz bezpośredni lewy i prawy (fizyczny) sąsiad w pamięci. No nie ma za bardzo nawet jak strzelić gdzie zaczyna się lewy i prawy blok. Trzeba przejść (w najgorszym wypadku) całą listę porównując przy okazji np. czy mój adres == adres aktualnie sprawdzany ± size aktualnie sprawdzanego. Czyli scalanie robi się z 𝐎(1) 𝐎(𝐧).
* ***leniwa strategia scalania***: nie mogę znaleźć o tym nigdzie info. Zdaje mi się, że to jest wykorzystywane w tzw. *lazy buddy allocator*. Mamy złączać bloki dopiero kiedy potrzebujemy więcej miejsca. Przy posortowanym po rozmiarze mamy dobrą złożoność do momentu aż nie zarządamy bloku który jest większy niż wszystkie inne. Wtedy trzeba odpalić coś co nam "posprząta pamięć" - połączy możliwe bloki. No ja wyobrażam sobie to w ten sposób, że wtedy jest odpalany algo co leci po liście po kolei i dla każdego sprawdza czy kolejny jest obok i czy może połączyć, AKA 𝐎(𝐧²).

---

**Zadanie 3.** Rozważmy algorytm listowy **best-fit** bez ustalonego porządku listy wolnych bloków (np. zawsze dodajemy na początek). W trakcie scalania bloku chcemy w O(1) znaleźć następnika oraz poprzednika i jeśli są wolne złączyć z naszym blokiem. Jak zmodyfikować algorytm przydziału i zwalniania? Jakich dodatkowych metadanych potrzebujesz?
> **Podpowiedź:** Technika ta jest znana pod nazwą „boundary tag” i została opisana w rozdziale §2.5 książki
„The Art of Computer Programming: Volume 1 / Fundametal Algorithms”, Donald E. Knuth.

---

* ***best-fit***: @UP[1]

To mamy blok który ma na swoich końcach tag mówiący o zajętości i rozmiarze
[zajęte][rozmiar][next][prev][dane][rozmiar][zajęte]
Dla czterech przybadków zwalniania bloku X ([A][X][B]):

* A i B zajęte
* A zajęte B wolne
* A wolne B zajęte
* A i B wolne

Następuje przepinanie wskaźników w czasie O(1) i scalanie w O(1) bo mamy dostęp do tagów słowo za końcami przedziałów.

---

**Zadanie 4.** Rozważmy algorytm listowy **next-fit** z wolnymi blokami posortowanymi po adresie. Chcemy zaprogramować algorytm przydziału pamięci wyłącznie dla ciągów znakowych nie dłuższych niż 100 bajtów. Możemy przyjąć, że **obszary** (ang. chunks) mają maksymalnie 16 stron, a wskaźniki zwracane przez malloc nie muszą być **wyrównane** (ang. aligned). Rozwiąż poniższe problemy:

* Chcemy efektywne kodować metadane (tj. minimalizować **narzut pamięciowy**), dla algorytmu listowego przyjmując, że będzie on działał na architekturze 64-bitowej.
* Program korzystający z naszego algorytmu będzie modyfikował zawartość przydzielonych bloków pamięci – chcemy skutecznie i szybko wykrywać błędy zapisu poza koniec ciągu znaków.
> **Podpowiedź:** Zauważ, że przy podanych ograniczeniach metadane można dość skutecznie skompresować.

---

[free(1b)size(7b)][next_cyclic(2B)][Dane][canary(1B)]

Bloki są na liście cyklicznej. Maksymalny adres to 2B bo 16KiB pamięci

---

**Zadanie 5.** Pokaż jak przy pomocy **algorytmu kubełkowego** (ang. segregated-fit, quick-fit) przyspieszyć działanie operacji przydziału bloku. Zaproponuj modyfikację tego algorytmu używając innych struktur danych niż lista dwukierunkowa. Odpowiedz na pytania z nagłówka listy! Czy **gorliwe złączanie** wolnych bloków jest w tym przypadku dobrym pomysłem?
> **Podpowiedź:** Rozważ drzewa zbalansowane, np. czerwono-czarne (ang. red-black) lub rozchylane (ang. splay).

---

Kubełki na deque i trzymają obszary w odpowiadających przedziałach rozmiarów. Każdy kubełek ma wskaźnik na pierwszy obszar a tamten na każdy kolejny więc branie(i oddawanie) w O(k) gdzie k to ilość kubełków. Gorliwe złączanie jest bez sensu - nigdy nie robimy, żeby podział na kubełki był optymalny. Metadane to tag zajętości, rozmiar i wskaźnik na next blok na liście. Jeśli obszary nie wyrównane, to jeszcze rozmiar. Nieużytki rzędu drugiego maksymalnego rozmiaru kubełka jeśli mamy wyrównywanie obszarów, lub 0 jeśli trzymamy obszary o rozmiarach z przedziału.

---

**Zadanie 6.** Rozważmy **algorytm bitmapowy** przydziału pamięci dla rekordów ustalonego rozmiaru. Pokaż jak przyspieszyć wyszukiwanie wolnych bloków z użyciem wbudowanej instrukcji procesora [`find-first-set`](https://en.wikipedia.org/wiki/Find_first_set) i drzewiastej struktury mapy bitowej. Uwzględnij, że blok pamięci podręcznej ma 64 bajty. Odpowiedz na pytania z nagłówka listy! Jak ulepszyć algorytm przydziału biorąc pod uwagę, że niedawno zwolnione bloki prawdopodobnie znajdują się w pamięci podręcznej procesora?

---

Możemy mieć drzewo bitmapowe, gdzie każdy element mówi nam "czy na przedziale adresów jest coś wolnego" albo przeciwnie "czy jest coś zajętego". Pierwsze jest dobre dla brania małych bloków, drugie fajne dla brania dużych bloków naraz. Rozgałęzienie drzewa może być nawet 64(ze względu na operację `ffs` operującą na QWORD) ze względu na rozmiar bloku cache. Czyli operacje zwalniania/przydziału rzędu log₆₄P gdzie P to całkowita dostępna pamięć. Żeby łatwo potem znajdywać bloki które były niedawno zwalniane można przetrzymywać dodatkowe metadane zawierające ostatnią ścieżkę w drzewie, np. 16 takich wpisów. operacja `ffs()` służy do znalezienie pierwszej jedynki na 64b słowie, czyli pierwszy wolny blok.

---

**Zadanie 7.** Rozważmy zarządzanie pamięcią w środowisku wielowątkowym. Czemu algorytmy przydziału
pamięci często cierpią na zjawisko **rywalizacji o blokady** (ang. lock contention)? Dlaczego błędnie zaprojektowany algorytm przydziału może powodować problemy z wydajnością programów wynikających z **fałszywego współdzielenia** (ang. false sharing)? Opisz pobieżnie struktury danych i pomysły wykorzystywane w bibliotece [`thread-caching malloc`](http://goog-perftools.sourceforge.net/doc/tcmalloc.html). Wyjaśnij jak zostały użyte do poradzenia sobie z problemem efektywnego przydziału pamięci w programach wielowątkowych?

---

* ***lock contention***: jak jeden wątek próbuje zdobyć blokadę, którą posiada inny. A czemu algorytmy alokacji na to cierpią? Najlogiczniejsze wydaje sięto, że rezerwowanie miejsca to sekcja krytyczna - można ostro zepsuć sobie kod jeśli będziemy równolegle próbować modyfikowac struktury alokatora. Przynajmniej w jednym obszarze zarządzanym przez alokator - jeśli równolegle na różnych obszarach które mają swoje własne metadane to może jeszcze się udać.
* ***false sharing***: problem przy cachowaniu - jak mamy wiele CPU to operacje na tej samej lini cache sprowadzają ją na nowo, bo cała jest wtedy oznaczano jako *invalid*. Czyli możemy mieć sytuację, w której wiele procesorów zmienia tę samą linię cache ale w innych miejscach - czyli niby bliskie ale nie te same dane, czyli mogłyby współdzielić, ale przez politykę cachowania trzeba całą linię wywalić. Kiedy cache się updatuje mamy zablokowane możliwości operacji na danej linii.
    > False sharing degrades performance when all of the following conditions occur:
    >
    > * Shared data is modified by multiple processors.
    > * Multiple processors update data within the same cache line.
    > * This updating occurs very frequently (for example, in a tight loop).
    >
    > Note that shared data that is read-only in a loop does not lead to false sharing.

* *thread caching malloc*: każdemu wątkowi przypisuje własny lokalny cache. Występuje okresowa wymiana danych między tymi cache-ami i centralnymi strukturami danych np. heapem. Obiekty ≤ 32K są traktowane inaczej - duże są bezpośrenio na centralnym heapie przez alokator stron (strony są 4K). Małe obiekty są przechowywane w specjalnej strukturze, która dla każdej klasy rozmiarów (np. 961→1024) jest linked lista wolnych obszarów. Klasy są specjalne rozmieszczone w zależności od swoich rozmiarów, np. małe co 8B średnio 16B większe 32B etc. (max: 256B). Mapuje się obiekt do klasy i zwraca element z listy(niepustej) czyli wolny obszar. I to niby jest lock-free. Jest jeszcze taka sama strukturka współdzielona między wszystkie wątki. Jak lista pusta to zgłasza się do kolejnego alokatora(czyli tutaj do tego współdzielonego, a ten do stronicowego jeśli nie ma). Dla większych niż 32K mamy podobną strukturkę, tylko że trzymającą ciągi stron(klasy ciągów {1..255} i inno- elementowych). Jak lista dla klasy jest pusta to idziemy do kolejnej klasy(jak się skończy to próbujemy mmapem albo sbrk). Ciągi stron są reprezentowane przez tzw. *Span*(może być *allocated* albo *free*). Jeśli jest *allocated* jako małe obiekty. Jest sobie też centralna tablica, która mówi która strona należy do którego *Span*-u. Jeśli obiekt jest dealokowany liczymy numer strony, patrzymy który span okupuje (span mówi czy obiekt jest mały czy nie i jego klasę jeśli jest mały). Jak jest mały to go dorzucamy na odpowiednią listę dla klasy na lokalny cache. Jeśłi lokalny cache się przepełni (≥2MB) to odpala się GC, który przenosi nieużywane obieky z lokalnych cechów do centralnych wolnych list(?). Jeśli obiekt jest duży to span mówi nam które strony pokrywa obiekt, więc patrzymy dodatkowo na spany dla stron poprzenich i następnych (sąsiadów) i scalamy i dodajemy spowrotem do odpowiedniej klasy ciągów stron. Te centralne listy na każdą klasę to 2D struktura: spany i listy wolnych obiektów na spany. Obiekt alokuje się z centralnej listy wolnych obiektów przez wzięcie pierwszego elementu z listy jakiegoś spany (dla małych). Jak zwalniamy do dodajemy do listy odpowiedniego spana. Jak jest peny(≡pusty xD) to całe strony są zwracane. Ogónie - branie pierwszego z listy jako pierwszego wolnego ma byćlock free i dodawanie do listy (na koniec pewnie) też.

---