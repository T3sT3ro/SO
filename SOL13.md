# SO lista 13

* Tanenbaum (wydanie czwarte): 2.4, 8.1.4, 10.3.4, A 5.2
* Stallings (wydanie dziewiąte): 9.1, 9.2, 10.1 – 10.3, 10.5
* Silberschatz (wydanie dziewiąte): 6.6

---
**Zadanie 1**. W pewnej chwili do systemu wsadowego przybywają kolejno zadania od P1 do P5. Oszacowany czas działania `T` w minutach oraz priorytet zadań `P` wczytano z opisu zadań wsadowych na taśmie magnetycznej i przedstawiono w tabeli poniżej. Zadania ważniejsze mają mniejszy numer priorytetu.

|Zadanie| Czas działania| Priorytet|
|---|---|---
|P1| 15| 6|
|P2| 9| 3|
|P3| 3| 7|
|P4| 6| 9|
|P5| 12| 4|

Przedstaw diagram szeregowania zadań, oblicz średni **czas przebywania** (ang. turnaround time) zadania i uśrednioną **przepustowość** (ang. throughput) systemu w okresie 10 minut. Rozważ następujące algorytmy:

* algorytm karuzelowy (ang. round-robin) z kwantem czasu o długości 1 minuty,
* algorytm uwzględniający priorytety (ang. priority scheduler),
* pierwszy zgłoszony, pierwszy obsłużony (ang. first come, first served),
* najpierw najkrótsze zadanie (ang. shortest job first).

---

* ***Czas przebywania*** – czas od nadejścia zadania do systemu do jego zakończenia.
* ***Przepustowość*** – liczba zadań ukończonych w zadanej jednostce czasu.

1. ***Algorytm karuzelowy (round-robin)*** – co minutę zmienia się zadanie aktualnie na procesorze, idąc po kolei: P1, P2, P3, P4, P5, P1, … . Jak któreś się skończy, wypada.
   * Czasy ukończeń: P1 – 45, P2 – 35, P3 – 13, P4 – 26, P5 – 42. Średni czas przebywania (SCP) = (13+26+35+42+45)/5 = 32,2
   * Przepstowość w okresie 10 minut: (5*10)/45 = 1,(1)   [pięć zadań wykonało się w sumie w 45 min, kiedy wszystkie zadania przybywają w tym samym czasie przepustowość jest taka sama dla wszystkich algorytmów].
2. ***Algorytm z priorytetami*** – szereguje po proiorytetach (od najniższej wartości)
   * Kolejność wykonywania zadań (w nawiasie czasy zakończenia): P2(9), P5(21), P1(36), P2(39), P4(45). SCP = (9+21+36+39+45)/5 = 30
3. ***Pierwszy zgłoszony, pierwszy obsłużony*** – de facto FIFO, kolejność wykonywania zgodna z kolejnością numeracji.
   * Czasy wykonania poszczególnych zadań: P1 – 15, P2 – 24, P3 – 27, P4 – 33, P5 – 45. SCP = (15+24+27+33+45)/5 = 28,8
4. ***Najpierw najkrótsze zadanie***: self-explanatory
   * Czasy obsłużenia zadań: P3 – 3, P4 – 9, P2 – 18, P5 – 30, P1 – 45. SCP = (3+9+18+30+45)/5 = 21

![to](https://i.imgur.com/yQjVSpt.png)

Diagram ma być w tej formie, ale z poprawnymi danymi, nie chce mi się rysować 4x tego samego, będzie podobnie jak w 6.

> ^autor: M. Ba.

---

**Zadanie 2**. Przeczytaj §10.3.4 (Tanenbaum) i wyjaśnij działanie planisty O(1) stosowanego w jądrze Linux 2.6. Uwzględnij struktury danych przechowujących wątki i sposób wyznaczania priorytetów.

> Wskazówka: Więcej informacji można znaleźć w artykule[„Inside the Linux scheduler”](https://www.ibm.com/developerworks/library/l-scheduler/)

---

* **Planista O(1)**: rozróżnia 140 priorytetów zadań, gdzie zadania o priorytecie 0 są najważniejsze, a te o priorytecie 139 – najmniej ważne. Priorytety od 0 do 99 są zarezerwowane dla zadań czasu rzeczywistego, podczas gdy te od 100 do 139 – dla pozostałych zadań. Planista trzyma strukturę danych złożoną z dwóch list kolejek. Jedna z nich to lista zadań active, druga to lista expired. Kolejek w każdej liście jest tyle, co priorytetów i są tak samo ponumerowane. Algorytm wybierania następnego taska do wykonywania działa następująco: wybierane jest pierwsze z brzegu zadanie z kolejki active o najniższym numerze (najważniejszej). Kiedy minie kwant czasu tego zadania, jest ono wywłaszczane i przesuwane na kolejkę expired (niekoniecznie dla tego samego priorytetu) i algorytm zaczyna się od początku. Kiedy wszystkie kolejki active są już puste, planista po prostu przepina wskaźniczki, czyniąc kolejki expired kolejkami active i na odwrót.

Nie wszystkie zadania mają takie same kwanty czasu, zadania w kolejkach o wyższym priorytecie (niższym numerze) mogą mieć przydzielone więcej czasu (mieć dłuższy kwant czasu).

Algorytm ten zapobiega głodzeniu procesów o niskim priorytecie (o ile nie następi sytuacji, gdy cały czas przychodzą nowe zdarzenia czasu rzeczywistego, zapychające kolejkę, ale jaka jest na to szansa…).

Jeśli proces zatrzymał się (np. czeka na I/O) przed zakończeniem swojego kwantu czasu, to planista wywłaszcza go od razu i przenosi z powrotem na kolejkę active, z której go zabrał. Wrzuca go na koniec tej kolejki z adnotacją, jaka część jego kwantu czasu mu jeszcze została.

Chcąc znaleźć jak najbardziej optymalną kolejność wkładania zadań na procesor, planista O(1) stosuje dynamiczny przydział priorytetów. Ktoś wykoncypował sobie, że procesy, które więcej korzystają z I/O są lepsze, bo więcej się dzielą – częściej śpią  w oczekiwaniu na I/O i wtedy zwalniają procesor. Dlatego chcemy im nadać wyższy priorytet. Planista O(1) dopuszcza wahana w przedziale od -5 do +5 w stosunku do poczatkowego priorytetu. Aby zbadać, czy dane zadanie jest bardziej I/O friendly czy CPU friendly trzyma dla każdego zadania zmienną sleep_avg, początkowo  ustawioną na 0. Jest ona inkrementowana, gdy proces nie śpi i zmniejszana, gdy śpi lub jest wywłaszczany. W ten sposób planista wie, które procesy dużo śpią i nadaje im wyższy priorytet.

![Struktura danych reprezentująca tablicę kolejek oczekujących procesów](https://i.imgur.com/n5QPC9T.png)

> ^autor: R. Ta.

---

**Zadanie 3**. Przeczytaj §10.3 (Stallings) lub §10.3.4 (Tanenbaum) o algorytmie Completely Fair Scheduler stosowanym w bieżących wersjach jądra Linux. Opowiedz o niedostatkach planisty O(1) i implementacji idei **sprawiedliwego szeregowania** (ang. fair-share scheduling). W jaki sposób algorytm CFS wybiera kolejne zadanie do uruchomienia? W jaki sposób nalicza wirtualny czas wykonania `vruntime`, aby wymusić sprawiedliwy przydział czasu procesora?

> Wskazówka: Więcej szczegółów można znaleźć w artykule [„Inside the Linux 2.6 Completely Fair Scheduler”](https://www.ibm.com/developerworks/library/l-completely-fair-scheduler/)

---

* ***sprawiedliwe szeregowanie***: algorytm szeregowania, który zapewnia, że użycie procesora jest równo rozdzielone między użytkowników i grupy, w przeciwieństwie do równej dystrybucji między procesy. Nie można dopuścić do głodzenia i przydzielania nienaturalnie dużych kwantów czasu.
* problemy z **O(1)**: Główną wadą algorytmu jest skomplikowana heurystyka oznaczania zadań jako interaktywne albo nieinteraktywne. Stara się je identyfikować przez badanie średniego czasu uśpienia (ilość czasu jaką zadanie czeka na wejście). Procesy czekające dużo pewnie czekają na dane od użytkownika, więc scheduler zakłada, że są interaktywne → daje bonus do priorytetu takich zadań i zmniejsza priorytety nieinteraktywnych. Obliczenia sprawdzające, czy task jest interaktywny czy nie są złożone i podatne na błędy, czyli mogą powodować nieinteraktywne zachowanie interaktywnych procesów. Czyli po prsotu heura to heura - kod niezbyt łatwy do utrzymywania i brakowało mu poparcia w algorytmach.
* *CFS*: ogólna idea jest taka, że jeśli czas na taska jest niezbalansowany bo za duży, to te niezbalansowane powinny dostać czas na wykonanie. Aby mieć nad tym kontrolę, DFS zapamiętuje *virtual runtime* - im mniejsze, tym bardziej ten task potrzebuje procesora. Dodatkowo implementuje koncept sprawiedliwego usypiania, żeby zapewnić, że jeśli taski nie są aktualnie *runnable* (np. czekają na I/O) to dostaną porównywalne części procka kiedy będą go już potrzebować. Zamiast trzymać taski w kolejce jak w poprzednich schedulerach, CFS trzyma drzewo czerwono-czarne posortowane pop czasie - samo się balansuje, operacje mają O(log n) czyli szybkie dodawanie/usuwanie tasków. Pod linkiem z polecenia pierwszy obrazek ładnie to pokazuje - najbardziej załodzone taski są na lewo w drzewie. Scheduler wybiera najbardziej lewy wierzchołek jako kolejny do odpalenia, dodaje czas uruchomienia do *vruntime* i z powrotem dodaje na RB-tree. No i to samo się sortuje przez to, że to jest RB-tree. Co do priorytetów - używane pośrednio, jako mnożnik(raczej współczynniki przemijania xD) do określania czasu, przez który task może się wykonywać. Znaczy to, że czas zadań o mniejszym priorytecie ubywa szybciej niż tych o wyższym. CFS ma też mechanizm dla grup procesów, żeby zapewnić sprawiedliwość między jakimiś hierarchiami procesów, między użytkownikami etc.

> ^autor: M. Po.

---

**Zadanie 4**. Na podstawie §10.5 (Stallings) wyjaśnij w jaki sposób krótkoterminowy planista ULE wykorzystuje **kolejki kalendarzowe**, opisane w artykule [Calendar Queues: A Fast O(1) Priority Queue Implementation for the Simulation Event Set Problem](https://dl.acm.org/citation.cfm?id=63045), do szeregowania zadań z priorytetami. Pokaż, że algorytm nie dopuszcza głodzenia zadań.

> Wskazówka: Więcej informacji można znaleźć w artykule [„ULE: A Modern Scheduler For FreeBSD”](https://www.usenix.org/legacy/event/bsdcon03/tech/full_papers/roberson/roberson.pdf)

---

* ***kolejka kalendarzowa***: *"A fast priority queue implementation having N buckets each with width w, or covering w time. An item with priority p more than current goes in bucket (p/w)%N. Choose N and w to have few items in each bucket. Keep items sorted within buckets. Double or halve N and change w if the number of items grows or shrinks a lot."*. W czasie zmienia się head i tail tej kolejki.

w FreeBSD jest 5 klas szeregowania zadań, zdefiniowane w tabelce 10.6 w Stallingsie. Per procesor są niby 3 kolejki: dwie dla klas kernel, rea-time, i time-sharing (priorytety 0-223), trzecia tylko na klasę idle: (224-255). Te dwie kolejki to "current" i "next". Każdy wątek który dostaje slice czasu ląduje w jednej z tych kolejek uwzględniając odpowiedni priorytet. Scheduler wybiera wątki z "current" dopóki się nie opróżni, a potem swapuje kolejki "current" i "next". To gwarantuje, że każdy wątek dostanie czas co dwie zmiany kolejki i zapobiega głodzeniu. Dodatkowo: kernel i real-time zawsze idą na "current", a time-sharing idzie na current jeśli jest interactive, albo na next w.p.p. Zadania o wyższym priorytecie są umieszczane bliżej, a te o niższym dalej obecnej głowy kolejki kalendarzowej. [Tutaj](https://books.google.pl/books?id=KfCuBAAAQBAJ&pg=PA121&lpg=PA121&dq=ULE+calendar+queue&source=bl&ots=uav3twJk35&sig=ACfU3U1qIOixmgcwS3TE1Ua-YcqhixW7_A&hl=pl&sa=X&ved=2ahUKEwje6qK5pJPgAhXooYsKHV_UDHIQ6AEwCHoECAAQAQ#v=onepage&q=ULE%20calendar%20queue&f=false) fajnie opisane. Dzieki kolejce kalendarzowej mechanizm pull i push między procesorami jest prosty, bo dostęp do elementów kolejki, które są w jakimś kubełku jest szybki.

---

**Zadanie 5**. Najważniejszym kryterium w systemach czasu rzeczywistego jest **dotrzymywanie terminów**. Zatem zależy nam na minimalizacji **opóźnienia przetwarzania zdarzeń** (ang. event latency). Na podstawie §6.6.1 (Silberschatz) wymień fazy przetwarzania zdarzenia. Wyjaśnij z czego wynika opóźnienie **przetwarzania przerwania** (ang. interrupt latency ) i **opóźnienie ekspedycji** (ang. dispatch latency ).

---

* ***dotrzymywanie terminów***: wykonywanie zadań w * wyznaczonym dla nich maksymalnym czasie.
* ***System o ostrych ograniczeniach czasowych***: system czasu rzeczywistego, w którym niedotrzymanie wymaganych terminów może mieć katastrofalne skutki i uniemożliwić poprawne działanie systemu. Np. operatory chłodzenia w reaktorach nuklearnych, autopilot w samolocie, maszyna podtrzymująca bicie serca.
* ***System o łagodnych ograniczeniach czasowych***: system w którym niedotrzymanie pewnej liczby terminów prowadzi jedynie do zmniejszenia efektywności systemu, ale nie zakłóca jego działania, o ile ich procent nie jest zbyt duży. Np. systemy audio i obrazu w komputerze – pominięcie kilku pikseli/dźwięków nie jest problemem, ale jak robi się ich za dużo, to obraz staje się nieczytelny, a dźwięk niezrozumiały.
* ***Opóźnienie przetwarzania zdarzeń***: czas pomiędzy pojawieniem się zdarzenia w systemie a rozpoczęciem jego obsługi.

Opóźnienia w przetwarzaniu zadań dzielą się na dwa rodzaje: opóźnienie obsługi przerwania (*interrupt latency*, czas potrzebny na sprawdzenie ) i opóźnienie ekspedycji (*dispatch latency*).

![interrupt latency](https://i.imgur.com/1dABK7m.png)

Opóźnienie obsługi przerwania zaczyna być liczone, kiedy przerwanie przychodzi do systemu. Czas ten obejmuje: zakończenie obecnie wykonywanej instrukcji na procesorze, wykrycie typu przerwania, zmianę kontekstu wraz z zapisaniem stanu obecnie wykonywanego procesu i przełączenie na odpowiednią procedurę obsługi przerwania ISR (*Interrupt Service Routine*). Liczenie opóźnienia obsługi przerwania kończy się wraz z tym przełączeniem. Bardzo ważny w tej kategorii jest czas, w którym siedzimy w jądrze z wyłączonymi przerwaniami (zmiana kontekstu, praca dyspozytora). Systemy czasu rzeczywistego są na to bardzo wrażliwe, bo w tym czasie może przyjść jakieś ważne przerwanie. Chcemy ten czas minimalizować.

![dispach latency](https://i.imgur.com/UHuHr0G.png)

Drugi typ opóźnień wiąże się z pracą dyspozytora. Opóźnienie ekspedycji liczymy od czasu, gdy proces jest gotowy do bycia wciągniętym na procesor. Obejmuje czas zatrzymania obecnie wykonywanego procesu i przełączenia na nasz, zwolnienia odpowiednich blokad, a także przydzielenia nam potrzebnych zasobów (o ile mamy uprawnienia do tych zasobów wyższe niż ten, kto te zasoby teraz posiada). Opóźnienia są dużo (kilka rzędów wielkości) mniejsze, gdy system obsługuje wywłaszczenia i wywłaszcza niegrzeczne procesy, gdy jest taka potrzeba.

> ^autor: M. Ba.

---

**Zadanie 6**. Opisz zasadę działania algorytmów szeregowania zadań okresowych ze statycznym i dynamicznych przydziałem priorytetów: odpowiednio **RMS** (ang. Rate Monotonic Scheduling) i **EDF** (ang. Earliest Deadline First). Podaj kryterium **szeregowalności** dla powyższych algorytmów. Rozważmy zadania `P1` i `P2`, gdzie `p1 = 50`, `t1 = 25`, `p2 = 75`, i `t2 = 30`. Dla obydwu algorytmów narysuj diagram przydziału czasu procesora dla pierwszych 200 milisekund działania algorytmów.

> Wskazówka: Przeczytaj A5.3 i A5.4 (Tanenbaum), albo §6.6.3 i §6.6.4 (Silberschatz), albo §10.2 (Stallings).

---

* ***Kryterium szeregowalności***: zasada, wedle której ustala się kolejność wykonywania zadań.
* ***Rate Monotonic Scheduling***: algorytm z przydzielonymi z góry stałymi priorytetami. Algorytm wywłaszcza proces, gdy w systemie pojawia się inny o wyższym priorytecie. Zazwyczaj priorytety ustalane są odwrotnie proporconalnie od długości okresu między terminami zadania (zadanie o najkrótszym terminie na wykonanie ma najwyższy priorytet).
* ***Earliest Deadline First***: algorytm z dynamicznym przydziałem priorytetów. Najwyższy priorytet dostaje zawsze zadanie, którego wyznaczony termin wykonania wypada najszybciej względem obecnego czasu.

* RMS, P1 > P2, Jeden niedotrzymany termin dla zadania P2
 ![RMS P1>P2](https://i.imgur.com/WesuEfS.png)
* RMS, P1 < P2, Dwa niedotrzymane terminy dla zadania P1.
 ![RMS P1>P2](https://i.imgur.com/XR7DG2v.png)
* EDF, Zero niedotrzymanych terminów.
 ![EDF](https://i.imgur.com/VJZCDXN.png)

> ^autor: M. Ba.

---

**Zadanie 7**. Opisz niekorzystne zjawiska, które należy brać pod uwagę projektując algorytmy szeregowania zadań dla maszyn wieloprocesorowych. Wyjaśnij jak algorytmy **szeregowania według powinowactwa** (ang. affinity scheduling) i **szeregowania zespołów** (ang. gang scheduling) adresują te problemy. Jakich technik używa algorytm ULE do **równoważenia obciążenia** (ang. load balancing) między rdzenie procesora? Na przykładzie architektury AMD Bulldozer z rysunku 10.3 (Stallings) zaproponuj metodę wyznaczania kosztu migracji wątków między rdzeniami procesora.

---

* Niekorzystne zjawiska:
   1. Wątki współdzielące przestrzeń adresową również współdzielą zasoby sprzętowe: TLB, predyktor, pamięć podręczną.
   2. Wątki często synchronizujące swoje działania powinny działać równolegle, by szybko postępować z obliczeniami.
   3. Wsparcie dla symmetric multiprocessing (SMP, wieloprocesorowość symetryczna, używane w renderingu, bazach danych, operacjach macierzowych etc.)
* **szeregowanie według powinowactwa (ang. affinity scheduling)**: Algorytm zwraca uwagę na to by kolejny wątek do uruchomienia pochodził z tej samej przestrzeni adresowej.
* **szeregowanie zespołowe (ang. gang scheduling)**: Zespół współpracujących wątków szeregowane jako jednostka, ekspediowane jednocześnie do różnych procesorów, członkowie grupy zaczynają i kończą kwant czasu wspólnie.
* **ULE ()**: Planista ULE przelicza priorytety na bieżąco i każdemu wątkowi z osobna. Każdy procesor posiada własny zestaw kolejek. Izolacja stanu ogranicza potrzebę synchronizacji procesorów.
  * **Algorytmy równoważenia obciążenia**:
    * **przyciąganie zadań (ang. pull migration)**: Gdy procesor się nudzi ustawia flagę “wolny” i szuka przeciążonego procesora; jak znajdzie to kradnie zadanie o najwyższym priorytecie, w p.p. wchodzi w stan uśpienia, a wybudzić można go wysyłając IPI (ang. Inter-Processor Interrupt).
    * **wypychanie zadań (ang. push migration)**: Zanim przeciążony procesor doda zadanie do swojej kolejki szuka “wolnego”; jak znajdzie to dodaje do jego kolejki i go wybudza.
    * **Scenariusz: 2 procesory i 3 wątki**: jak każdemu dać 66% procesora? Żeby symulować sprawiedliwy przydział czasu, system musi regularnie zmieniać przypisanie zadań do rdzeni. Długoterminowy algorytm równoważenia obciążenia wyrównuje obciążenie.
  * **Koszt migracji AMD Bulldozer**: Dla podanej struktury cache są 3 możliwe koszta:
    * 0 - strona już znajduje się w L1D docelowego rdzenia(łatwo sprawdzić)
    * L2 - L1D - musimy przenieść z L2 do L1D docelowego rdzenia :
      * jeśli przenosimy do "brata"
      * przenosimy do rdzenia który już ma w L2 naszą stronę
      * L3 - L1D - jeśli docelowy rdzeń nie ma w L2 naszej strony  

    dodatkowe koszta L3 - L2 jeśli wątek korzystał z wielu stron które miał już w L2 swojego rdzenia, a nie ma ich w L2 docelowego

    ![AMD](https://i.imgur.com/zDh2V7H.png)

> ^autor: R. Ta.

---