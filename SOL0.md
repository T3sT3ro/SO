# SO Lista 0

## **Zadanie 1**

Opisz różnice między **przerwaniem sprzętowym** (ang. hardware interrupt), **wyjątkiem procesora** (ang. exception) i **pułapką** (ang. trap). Dla każdego z nich podaj co najmniej trzy przykłady zdarzeń, które je wyzwalają. W jakim scenariuszu wyjątek procesora nie oznacza błędu czasu wykonania programu? Kiedy pułapka jest generowana w wyniku prawidłowej pracy programu?

---

* ***przerwanie*** :
  * asynchroniczne, spowodowane zdarzeniami zewnętrznymi
  * np. przerwanie z innego procesora w maszynie z wieloama procesorami (*inter-processor interrupt*), przyjście pakietu internetowego, przybycie danych z dysku, przerwanie zegara co kilka ms.
* ***wyjątek*** :
  * wewnętrzne, zgłaszane przez CPU dla sytuacji wyjątkowych, np. dzielenie przez 0.
  * generowane przez proces
  * synchroniczne z wykonywanym programem
  * przetwarzane przez OS i dopiero idzie do programu (o ile ma iść...)
  * np. przepełnienie (instrukcja INTO), sprawdzanie granic (BOUND), dzielenie przez 0, wyjątek stosu, page faut(czyli program działą dobrze, po prostu strona została wypieprzona np. na swapa)
* ***pułapki*** :
  * wyjątek przewidziany, program i procesor operjują na zasadzie "oho, tutaj będzie ten trap i się wykona"
  * synchroniczne
  * SO przełącza się w kernel mode a potem wraca i robi `RIP++`
  * np. breakpoint (czyli działa dobrze) , debugger, syscall, assert

---

## **Zadanie 2**

Opisz mechanizm **obsługi przerwań** bazujący na **wektorze przerwań** (ang. interrupt vector table). Co robi procesor przed pobraniem pierwszej instrukcji **procedury obsługi przerwania** (ang. interrupt handler) i po natrafieniu na instrukcję powrotu z przerwania? Czemu procedura obsługi przerwania powinna być wykonywana w **trybie nadzorcy** i używać odrębnego stosu?

---

* ***wektor przerwań***: wskaźnik na handler do przerwania, czyli procedurę obługi danego przerwania uruhcamianą, kiedy zostanie ono zgłoszone. Wygląda to mniej więcej tak: `context switch -> (opcjonalnie wykryj urządzenie przerwania) -> obsłuż przerwanie -. (opcjonalnie info dla sprzętu, że obsłużone) -> context switch -> instrukcja iret`
* ***tryb nadzorcy (supervisor)*** : pozwala na dostęp do bebechów komputera - głównie pamięć, sieć, niskopoziomowe procesy. Obsługa przerwania powinna być obsługiwana w tym trybie, bo musi mieć dostęp do uprzywilejowanych zasobów.

---

## **Zadanie 3**

Język C definiuje kilka słów kluczowych wpływających na właściwości definiowanych bytów. Jakie jest znaczenie słowa kluczowego `static` w odniesieniu do zmiennych, a do funkcji? Kiedy należy użyć słowa kluczowego `volatile` w stosunku do zmiennych?

---

* `static`:
  * globalna: coś jest widoczne tylko w danym module translacji (prawie że pliku)
  * lokalna:
    * zmienna: w funkcji oznacza, że jej wartość jest współdzielona między wszystkimi instancjami funkji w danym procesie
* `volatile`: tylko do zmiennych - wartość moze się zmienić "z zewnątrz", np. jak mamy jakieś zewnętrzne urządzenia które są zmapowane na naszą pamięć (czyli jakby np. podpięte są pod kartę SD) to mogą sobie po niej pisać a my nic nie wiemy. Więc wtedy kompilator nam tego nie wyoptymalizuje.

---

## **Zadanie 4**

Bazując na formacie ELF (ang. Executable and Linkable Format) opisz składowe pliku wykonywalnego. Czym różni się **sekcja** od **segmentu**? Co opisują **nagłówki** programu (ang. program headers)? Jakie ważne informacje zawiera nagłówek pliku ELF?
> Wskazówka: Skorzystaj z narzędzia `«readelf»`.

---

* ***segment*** a ***sekcja***: sekcja zawiera informacje potrzebne przy linkowaniu, a segment przy działaniu programu

| ELF | opis |
| --- | --- |
| ELF header | wielkość słowa, endian, (.o/.so/.exec), maszyna, rozmiar adresów |
| Segment header table | wielkość strony, segmenty wirtualnych adresów, wielkość, typ segmentów, rozmiar |
| .text section | kod |
| .rodata | tylko do odczytu |
| .data | zainicjalizowane globale |
| .bss | ninezainicjalizowane globale |
| .symtable | tablica symboli |
| .rel.txt | relokacja dla sekcji .text |
| .debug | dodatkowe info do debagowania |
| Section header table | offsety i rozmiary sekcji |

---

## **Zadanie 5**

 Zasada **lokalności odwołań** mówi o tym, że programy wykazują pewne regularności w dostępie do pamięci. Zdefiniuj pojęcia **lokalności czasowej** i **przestrzennej**, a następnie wytłumacz je używając pseudokodu podanego na tablicy. Opisz jak stosuje się tę zasadę w projektowaniu systemów komputerowych na przykładach pamięci operacyjnej, dysku twardego i sieci Web.

---

* nie ma co się o lokalnościach rozpisywać: jak odwołujemy się do elementu x w pewnym momencie, to pewnie do niego będziemy się też odwoływać za niedługo(czasowa), albo do elementów (fizycznie) blisko x - tablica na przykład(przestrzenna). Kod łatwo wymyślić, przykłady też więc nie piszę.

---

## **Zadanie 6**

Zapoznaj się z rozdziałami 3 – 5 i A dokumentów [System V Application Binary Interface](http://www.sco.com/developers/gabi/latest/contents.html) i [System V Application Binary Interface AMD64 Architecture Processor Supplement](https://www.uclibc.org/docs/psABI-x86_64.pdf). Wymień główne obszary ABI (ang. Application Binary Interface), których definicja jest wykorzystywana w implementacji jądra systemu operacyjnego, kompilatora, konsolidatora i biblioteki standardowej.

---

ABI definiuje np. jak wołać funkcje, kto zapisuje sobie stos, jak przekazywane parametry, jak się powraca z funkcji, jak syscalle się robi, (od tąd do końca: czzego trzeba do bibliotek) jak się zmienia kontekst, zarządza wyjątkami, zwijanie stosu, jak trzymać globale, jak zachowują się funkcje na floatach etc.

---

## **Zadanie 7**

Przypomnij jak wygląda mechanizm tłumaczenia adresów bazujący na dwupoziomowej tablicy stron w procesorze z rodziny IA-32. Przedstaw algorytm obliczania adresu fizycznego na podstawie adresu wirtualnego z uwzględnieniem uprawnień dostępu. Jaką rolę w procesie tłumaczenia odgrywa pamięć TLB (ang. Translation Look-aside Buffer)?

---

* ***TLB*** : **T**ranslation **L**ookaside **B**uffer - mały cache na wpisy z tablicy stron. Pozwala szybciej przeprowadzić translację adresów. 

wirtualny adres składa się z części `[OF][IF][OFF]` gdzie `[OF]` koduje offset na zewnętrznej tablicy, `[IF]` na wewnętrznej dając page number a `[OFF]` to offset na stronie. Są też dodatkowe bity na `[U]ser/supervisor`, `[R]ead/write`, `[P]resent` i inne.

---

## **Zadanie 8**

Wyjaśnij znaczenie bitów: P, R/W, U/S, PWT, PCD, A, D, G; we **wpisie tablicy stron** (ang. page table entry) procesora z rodziny IA-32. W jakim celu stosuje je jądro systemu operacyjnego? Które z nich może modyfikować procesor i dlaczego?
> Wskazówka: Zapoznaj się z rozdziałem czwartym dokumentu „Intel 64 and IA-32 Architectures Software Developer’s Manual – Volume 3: System Programming Guide”.

---

* ***page table entry (PTE)*** : przechowuje informacje o stronie. bitowo (liczba przed to ile bitów zajmuje. brak to 1 bit): `[20: fizyczny adres strony][3: avail][G][_][D][A][C=PCD][W=PWT][U=U/S][R=R/W][P]`.
  * G - nie czyść wpisu TLB na context switch
  * _ - zawsze pusty
  * D - dirty: już zmieniana
  * A - accessed: już czytana/pisana
  * C - cache disable
  * W - writhe-through
  * U - 1:wszyscy, 0:supervisor
  * R - 1:R+W, 0:R
  * P - present: 1:aktualnie w pamięci fizycznej
  
  Nie pamiętam, które może zmieniać CPU. Wszystko to jest potrzebne do kontroli uprawnień, cachowania, wymiany stron.

---