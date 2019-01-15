# SO Lista 10

* Tanenbaum (wydanie czwarte): 3.4 – 3.6
* Stallings (wydanie dziewiąte): 8.2

**Zadanie 1**. Rozważamy zarządzanie pamięcią wirtualną. Kiedy system korzysta z polityki **ładowania**, **przydziału miejsca**, **zastępowania** i **usuwania**? Porównaj **stronicowanie wstępne** (ang. prepaging) i **stronicowanie na żądani**(ang. demand paging). Jakie zachowania programów będą efektywnie obsługiwane przez jedną, a jakie przez drugą politykę?

---

* ***polityka ładowania***: kiedy ładuje się do pamięci operacyjnej potrzebne strony
  * *na żądanie*: czekamy za każdym razem
  * *prepaging*: ładuje parę `(potrzebna, następna)`- dobre dla szeregowucj dostępów do pamięci
* ***przydziału miejsca***: gdzie zapisywać w pamięci dane
  * niejednorodny dostęp: bliżej ustawiane częściej używane dane
  * kolorowanie stron: pewien kolor tylko w określone miejsca
  * duże strony
* ***zastępowanie***: cała dotępna pamięć zajęta, błąd strony i trzeba zwolnić jednąz ramek:
  * FIFO
  * LRU
* ***usuwanie***: wyczyścić ramki i zintegrować z pamięcią drugorzędną:
  * leniwe: jak strona będzie jeszcze wykorzystywana
  * golriwe: zebranie kilku brudnych ramek i czyszczenie za jednym razem

---

**Zadanie 2**. Czym różni się **odwzorowanie pamięci anonimowej** (ang. anonymous mapping) od **odwzorowania pliku na pamięć** (ang. memory-mapped I/O)? Rozważmy proces, który utworzył dziecko z użyciem wywołania fork. Opisz działanie mechanizmu **kopiowania przy zapisie** (ang. copy-on-write) dla odwzorowań **prywatnych**. Wszystkie procesy korzystające z pewnego **dzielonego** odwzorowania pliku na pamięć zakończyły swe działanie. Co w takim przypadku musi zrobić system? Strony których odwzorowań można przenieść do **przestrzeni wymiany** (ang. swap space)?

---

* ***anon map***: Nie ma pokrycia w pamięci drugorzędnej - dane są tylko w operacyjnej. Dane tymczasowe w ten sposób sięalokuje, np. stos. Na początku dowzorowanie jest tylko w VM (**V**irtual**M**emory), jeśli proces domaga się pamięci dostaje wyzerowane strony. Jeśli brakuje miejsca to odwzorowanie może być przeniesione do pamięci wymiany, ale gdy jej nie ma, jest przypięte do operacyjnej.
* ***mmap I/O***: odwzorowanie w pamięci podręcznej, które ma odpowiadający sobie region w pamięci drugorzędnej.
* ***odwzorowanie prywatne***: zmiany dokonywane przez proces nie są przekazywane do oryginalnego pliku z pamięci drugorzędnej, ale są trzymane w innym jej miejscu lub w pamięci operacyjnej. Proces ma jedną kopię swoich stron. Prościej: proces tworzy odwzorowanie tylko w celu czytania z pliku.
* ***odwzorowanie dzielone***: zmiany są zapisywane w oryginalnym pliku i inne procesy to widzą.
* ***copy-on-write***: dopiowanie strony odbywa się dopiero gdy proces próbuje zapisać coś do pamięci. W innym wypadku czyta z oryginału. Wydajne dla samych odczytów.
* ***swap space***: przestrzeń na dyku na tymczasowe strony kiedy RAM jest pełny. Na linuxie to jest partycja. Przenosi się tam głównie strony odwzorowań anonimowych, bo nei ma gdzie ich dać.

Jeśli wszystkie procesy z współdzielopnym mapowaniem zakończyły się to system zsynchronizuje stronę.

---

**Zadanie 3**. Na podstawie slajdów do wykładu „CSAPP3e: Virtual Memory Systems” wyjaśnij w jaki sposób system Linux obsługuje błąd strony. Kiedy jądro wyśle procesowi sygnał `SIGSEGV` z kodem `«SEGV_MAPERR»` lub `«SEGV_ACCERR»`? W jakiej sytuacji wystąpi **pomniejszy** (ang. minor) błąd strony lub **poważny** (ang. major) błąd strony? W jakiej sytuacji wystąpi błąd strony przy zapisie mimo, że pole `«vm_prot»` pozwala na zapis do **obiektu wspierającego** (ang. backing object)?

---

* ***major page fault***: dostęp do strony nieobecnej w pamięci operacyjnej.
* ***minor page fault***: dostęp do strony ze złymi uprawnieniami/strona niepodczepiona do procesu.
* *MAPERR*: odwołanie do niezmapowanych adresów
* *ACCERR*: błąd uprawnień - np. pisanie pod adresy READ-ONLY
* ***obiekt wspierający***: pamięc drugorzędna, z której system pobiera dane.

* obsługa błędu strony w linuxie([[1]](https://goo.gl/QSHpTL), [[2]](https://goo.gl/3s3gvK)): w skrócie - wywołanie handlera, aktualizacja PTE o stronę z odpowiednimi flagami (accessed zawsze, dirty tylko jeśli zapisywanie), powrót z handlera.  

Błąd przy zapisie może wystapić gdy jest współdzielony i w tym samym czasie ktoś inny z niego korzysta, albo gdy strona została wyrzucona na swapa.

---

**Zadanie 4**. Rozważmy system ze stałym rozmiarem **zbioru rezydentnego**. Mamy pamięć fizyczną składającą się z 4 **ramek** oraz pamięć wirtualną złożoną z 8 **stron**. Startujemy z pustą pamięcią fizyczną. Jedyny wykonujący się proces generuje następujący ciąg dostępów do stron: `0, 1, 7, 2, 3, 2, 7, 1, 0, 3`. Zaprezentuj działanie **algorytmów zastępowania stron** FIFO i LRU. Który z nich generuje najmniej **błędów stron**? Załóż, że zastąpienie ramki zachodzi dopiero w momencie zapełnienia całej pamięci fizycznej.

* ***ramka***: miejsce w pamięci fizycznej na jedną stronę
* ***strona***: "jednostkowy" obszar wirtualnej przestrzeni adresowej. Zazwyczaj 4kb.
* ***zbiór rezydentny***: zbiór wszystkich stron w pamięci operacyjnej w danej chwili dla procesu.

* FIFO - lista kolejności przychodzenia stron. LRU - najstarszy użyty.

|FIFO| | | | |PF?|    |LRU| | | | |PF?|
|----|-|-|-|-|---|----|---|-|-|-|-|---|
|0   |0| | | |PF |    |0  |0| | | |PF |
|1   |0|1| | |PF |    |1  |0|1| | |PF |
|7   |0|1|7| |PF |    |7  |0|1|7| |PF |
|2   |0|1|7|2|PF |    |2  |0|1|7|2|PF |
|3   |3|1|7|2|PF |    |3  |3|1|7|2|PF |
|2   |3|1|7|2|   |    |2  |3|1|7|2|   |
|7   |3|1|7|2|   |    |7  |3|1|7|2|   |
|1   |3|1|7|2|   |    |1  |3|1|7|2|   |
|0   |3|0|7|2|PF |    |0  |0|1|7|2|PF |
|3   |3|0|7|2|   |    |3  |0|1|7|3|PF |

---

**Zadanie 5**. Wyjaśnij działanie **algorytmu drugiej szansy**. Następnie rozważ przykład z obrazu 3-15(b) w podręczniku Tanenbaum. Niech bity R stron, począwszy od B a kończąc na A, wynoszą odpowiednio: `1, 1, 0, 1, 1, 0, 1, 1`. Która ze stron zostanie zastąpiona w trakcie obsługi następnego błędu strony? Jak będzie działał algorytm, jeśli wszystkie bity są ustawione na 1?

---

* ***druga szansa***: Modyfikacja FIFO korzystająca z bitów `R`(reference) i `M`(modified). Przed usunięciem patrzy na bit `R`. Jeśli był `0` to wywala i albo pisze na dysk(`M==1`) albo porzuca(`M==0`), a jeśli `1` to dodaje na poczatek kolejki i czyści `R`. Jeśli wszystkie były `R==1` to degeneruje się do FIFO. Dla przykładu z treści zadania, czyli stron `B C D E F G H A` w tej kolejności pierwszą wywaloną będzie strona `D`

---

**Zadanie 6**. Rozważmy komputer podobny do [MERA-400](https://mera400.pl/) posiadający tylko 4 ramki o rozmiarze 4KiB. W pierwszym takcie zegara systemowego bity R ramek są ustawione zgodnie z bitami w ciągu 0111. W kolejnych taktach zegarowych zarejestrowano wartości bitów: 1011, 1010, 1101, 0010, 1010, 1100 i 0001. Używamy **algorytmu postarzania stron** z 8-bitowymi licznikami. Oblicz wartości liczników dla ostatniego taktu zegara systemowego. Następnie proces wywołuje błąd strony – wybierz ramkę ofiarę. Jaka będzie wartość licznika dla nowo wstawionej ramki?

---

* ***algorytmu postarzania stron***: programowe przybliżenie metody LRU wykorzystujące rejestry przesuwane w prawo dla każdej strony, do których dopisywane jest w każdym takcie wirtualnego zegara `1` jeśli strona zostałą szturchnięta i `0` w.p.p.

|page|8|7|6|5|4|3|2|1|VAL|
|----|-|-|-|-|-|-|-|-|---|
|A   |0|1|1|0|1|1|1|0|110|
|B   |0|1|0|0|1|0|0|1|73 |
|C   |0|0|1|1|0|1|1|1|55 |
|D   |1|0|0|0|1|0|1|1|139|
Wywalamy ramkę `C` bo ma najmniejszy licznik. Wartość licznika nowej strony to `0b10000000=128`.

---

**Zadanie 7**. Na podstawie artykułu [A Simple and Effective Algorithm for Virtual Memory Management](https://goo.gl/fKta9r) opisz działanie algorytmu WSClock. Kiedy zleca strony do **wyczyszczenia**? Rozważmy przykład z obrazu 3-20(c) w podręczniku Tanenbaum. Ramię zegara wskazuje na stronę ze znacznikiem czasowym `1213` i wyzerowanym bitem R. Która ze stron zostanie zastąpiona, jeśli `τ = 400`, a która jeśli `τ = 1000`?

---

> UWAGA! Opis algorytmu z podręcznika Tanenbaum zawiera błędy – proszę przeczytać zamieszczony artykuł.

![schemat blokowy WSClock](https://i.imgur.com/50nCg40.png)

przykład z tannenbauma:  
![Tannenbaum](https://i.imgur.com/9nffpGv.png)
`VT = 2204`

* `τ = 400`: tylko `2204-1213 > 400` więc ona zastąpiona
* `τ = 1000`: żadnej strony nie można wywalić, więc algorytm zleci uśpienie procesu.

* ***wyczyszczenie***: integracja danych ze strony w RAMie z tymi w pamięci drugrzędnej.

---

**Zadanie 8**. W jakim celu jądro systemu operacyjnego stosuje **buforowanie stron**? Wymień główne zadania **demona stronicowania** (ang. pageout daemon). Jakie kryteria, oprócz **częstości błędów stron** (ang. page fault frequency ), można stosować określając wielkość zbioru rezydentnego dla procesu? Wyjaśnij przyczyny powstawania zjawiska **szamotania**. W jaki sposób można to zjawisko wykryć i jak mu zapobiegać?

---

* ***buforowanie stron***: trzymanie kiku ramek wolnych, aby móc szybko wtorzyć potrzebną stronę a dopiero potem martwić się którą wyrzucić(? a to bardziej wygląda na coś w stylu slot pool-a). Według mnie z kolei, jest to nie wywalanie od razu ramki na pamięć drugorzędną ale przeniesienie na bufor - drugą listę w której tylko oznaczamy stronę odpowiednią flagą. I takie bufory są dwa: na wtoczenie (puste ramki, nie trzeba synchronizować z drugorzędną i pełne ramki do synchronizacji - żeby poczekać, aż sięuzbierają)
* ***demona stronicowania***: wątek jądra regularnie przeglądający bufor stron, który usuwa strony jeśli mało wolnych ramek, uspójnia dane z dyskiem, zapomina o czystych nieużywanych stronach/stronach bez właściciela.
* ***frequency***: liczba błędów stron przez liczbę odwołań do pamięci - całkiem skuteczny sposób iczenia zbioru rezydentnego. Innymi może być zliczanie stron do których się odnosi dany proces.
* ***szamotanie***: zbiór roboczy > zbiór rezydentny => komputer spędza dużo czasu na wymianie ramek. Dużo błędów stron przez to i duża bezczynność procka.

---