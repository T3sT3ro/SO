# Lista 6

**Zadanie 1.** Podaj w pseudokodzie semantykę **instrukcji atomowej** compare-and-swap i z jej użyciem zaimplementuj **blokadę wirującą** (ang. spin-lock). W jakich systemach komputerowych stosuje się ten typ blokad? Wymień zalety i wady blokad wirujących w porównaniu do **blokad usypiających**. Opisz rozwiązanie pośrednie, czyli **blokady adaptacyjne**.

---

* ***instrukcja atomowa***: taka, w trakcie której nie nastąpi wywłaszczenie. Wykona się od początku do końca bez np. zmian kontekstu.
* ***spinlock***: blokada zużywająca czas procesora do aktywnego sprawdzania czy zachądzą warunki na kontynuację. np. `volatile int flag; while(flag);`. Dobre, jeśli mamy pewność, że czekanie nie będzie długie i koszt poczekania jest mniejszy niż koszt uśpienia i zmian kontekstu. Przykładowo w systemach wbudowanych bardzo często stosuje się spinlocki do sprawdzenia czy dane z magistrali szeregowej zostały wysłane i można przeprowadzić kolejny zapis. Słabe bo nie zawsze jest pewność ile będziemy czekać, a granica czy zastosować czy nie spinlocka jest śliska. Dodatkowo systemy czasu rzeczywistego z tego korzystają. A i spinlocki są nazywane spinlockami jeśli korzystają z atomowych operacji.
* ***blokada usypiająca***: usypia wątek aż ten nie dostanie np. jakiegoś sygnału. Nie marnuje się czasu na czekaniu w uśpionym wątku.
* ***blokady adaptacyjne***: inaczej połączenie metod wielu blokad. Np. Możemy mieć spinlock który kręci się do ustalonej wartości licznika, np. 10k, a następnie jeśli dalej nie może się doczekać - usypia się. Jeśli ponaddto właściciel locka A czeka na locka B i jest uspiony, to nie ma sensu, żeby ktoś czekający na A robił spinlocka.
* *`compare_and_swap(*where, old, new)`*: jeśli `*where==old`, to `*where:=new`. Zwraca `True` jeśli nowa i stara wartość były takie same i zapis się udał.

```C
1 volatile tid_t owner = NULL;
2 while(compare_and_swap(owner, NULL, this->tid))
3       ;
4 // critsec
5 owner = NULL;
```

---

**Zadanie 2.** Poniżej widnieje rozwiązanie problemu wzajemnego wykluczania Peterson’a dla dwóch procesów (Tanenbaum, §2.3.3). Uzasadnij jego poprawność i pokaż jak można go rozszerzyć na wiele procesów.

```C
1  volatile int turn = 0;
2  volatile int interested[2] = {0, 0};
3
4  void enter_critical(int process) {
5       interested[process] = 1;
6       turn = process;
7       while (turn == process && interested[1 - process] == 1) {}
8  }
9
10 void leave_critical(int process) {
11      interested[process] = 0;
12 }
```

> **Ciekawostka:** Czasami ten algorytm stosuje się w praktyce dla architektur bez instrukcji atomowych np.: [tegra_pen_lock](https://goo.gl/E1x7vn).

---

Po pierwsze: każdy wątek sam zmienia sobie pole w tablicy `interested[]` więc wyścigów tutaj nie ma. Po drugie: mamy potencjalny wyścig na zmiennej `turn`. Ale kiedy wątek wejdzie w sekcję krytyczną? tylko kiedy będzie `(turn != process || interester[1-process] != 1)`. innymi słowy: "jeśli jest moja kolej lub drugi proces nie chce wejść, to ja mogę". To teraz patrzymy na nasz potencjany wyścig na turn: proces A przeszedł spinlocka, więc jest zainteresowany. Co najwyżej turn może mieć 2 różne wartości dla wątku B: albo `turn=B` albo `turn=A`. Jeśli jest sytuacja pierwsza, to B się kręci bo nie przechodzi drugi warunek `while`. jeśli sytuacja 2 to też się kręci, bo `while` się wykrzacza wcześnie na pierwszym warunku.

Algorytm na wiele procesów nazywa się algorytmem piekarnianym. Nie chcę pisać pseudokodu bo jest w [necie](https://goo.gl/ze3voy), ale idea opiera się na systemie ticketów: klienci przychodzą do piekarni i pobierają numerek. Na ekranie wyświetla się aktualny numerek. Klient z tym numerkiem jest obsługiwany. Kiedy wyrzuca swój numerek do śmieci, na ekranie pojawia się następny numerek.

---

**Zadanie 3 (S).** Poniżej podano jedno z rozwiązań **problemu ucztujących filozofów** (Tanenbaum, §2.5.1). Zakładamy, że istnieją tylko leworęczni i praworęczni filozofowie, którzy podnoszą odpowiednio lewy i prawy widelec jako pierwszy. Widelce są ponumerowane zgodnie ze wskazówkami zegara. Udowodnij, że jakikolwiek układ pięciu lub więcej ucztujących filozofów z co najmniej jednym leworęcznym i praworęcznym zapobiega zakleszczeniom i głodzeniu.

```C
                    semaphore fork[N] = {1, 1, 1, 1, 1, ...};

1  void righthanded (int i) {                       13 void lefthanded (int i) {
2       while (true) {                              14      while (true) {
3           think ();                               15          think ();
4           wait (fork[(i+1) mod N]);               16          wait (fork[i]);
5           wait (fork[i]);                         17          wait (fork[(i+1) mod N]);
6           eat ();                                 18          eat ();
7           signal (fork[i]);                       19          signal (fork[(i+1) mod N]);
8           signal (fork[(i+1) mod N]);             20          signal (fork[i]);
9       }                                           21      }
10 }                                                22 }
```

---

* ***problem ucztujących filozofów***: Mamy `n` filozofów przy okrągłym stole. Każdy z nich ma spaghetti i po swojej prawej widelec. Spaghetti je się dwoma widelcami. Żeby jakiś filozof mógł zjeść swoje spaghetti potrzebuje swojego i widelca sąsiada z lewej. w wypadku kiedy każdy z filozofów weźmie do ręki swój widelec, żaden nie będzie mógł pożyczyć od sąsiada widelca, co skutkuje śmiercią z głodu (deadlock). Ponadto, jeśli jeden z filozofów je swoje danie to w najgorszym wypadku każdy z pozostałych czeka, więc nici z urównoleglenia jedzenia spaghetti.
* ***prawo i leworęczni filozofowie***: [tutaj opisane](https://goo.gl/S3V8uZ), mi się nie chce przepisywać.

---

**Zadanie 4 (P).** Podaj w pseudokodzie implementację **blokady współdzielonej** z operacjami «init», «rdlock», «wrlock» i «unlock» używając wyłącznie muteksów i zmiennych warunkowych. Nie definiujemy zachowania dla następujących przypadków: zwalnianie blokady do odczytu więcej razy niż została wzięta; zwalnianie blokady do zapisu, gdy nie jest się jej właścicielem; wielokrotne zakładanie blokady do zapisu z tego samego wątku. Jeśli rozwiązanie dopuszcza głodzenie, to podaj propozycję jak to naprawić.
> **Podpowiedź:** `RWLock = {owner: Thread, readers: int, critsec: Mutex, noreaders: CondVar, nowriter: CondVar, writer: Mutex}`

---

***shared lock***: wiele wątków może być podpiętych do jednego zasobu na jednej blokadzie i wiele może naraz czytać, tylko jeden może naraz pisać.

```C
tid_t owner;
int readers;
mutex_t critsec, writer;
cond_t noreaders, nowriter;

void init(){
    owner = NULL; readers = 0;
    mutex_init(&critsec); mutex_init(&writer);
    cond_init(&noreaders); cond_init(&nowriter);
}

void rdlock(){
    lock(critsec);
    while(owner) // aka writer exists
        wait(nowriter, critsec);
    readers++;
    unlock(critsec);
}

void wrlock(tid_t tid){
    lock(writer); // other writers will block here
    lock(critsec);
    while(readers > 0) // on 0 readers critsec acquired
        wait(noreader, critsec);
    owner = tid; // set owner AKA there is a writer
    unlock(critsec);
}

void rwunlock(tid_t tid){
    lock(critsec);
    if(owner == tid){ // writer unlock
        owner = NULL;
        broadcast(nowriter); // readers can continue. Broadcast to wakeup all readers
        unlock(writer); // next writer will unlock and wait for critsec
    } else { // reader unlock
        readers--;
        if(readers == 0) // budzimy writera tylko jeśli readerów jest 0 żeby nie było fake-ów
            signal(noreaders); // można broadcasta żeby budzić wszystkich writerów
    }
    unlock(critsec)
}
```

`while(owner)...` w rdlock() rozwiązuje problem fałszywych wybudzeń na condvara (eng. spurious wakeups AKA false wakeups).

Głodzenie może wystąpić, bo w wypadku dużej ilości readerów i małej writerów, zarówno `rdlock()` jak i `wrlock()` będą rywalizować o lock na `critsec`. Można wprowadzić priorytetyzowanie writerów w stosunku do readerów i wprowadzić zmienną `wantwrite` mówiącą, że pojawił się ktoś kto chce pisać i mamy nie pozwalać na dodawanie kolejnych readerów (zakładając, że zapisy są dużo rzadsze niż odczyty).

---

**Zadanie 5 (S).** Poniżej podano błędną implementację semafora zliczającego z użyciem semaforów binarnych. Znajdź kontrprzykład i zaprezentuj wszystkie warunki niezbędne do jego odtworzenia.

```C
1  struct csem {                    13 void wait(csem &s) {             23 void signal(csem &s) {
2       bsem mutex;                 14      wait (s.mutex);             24      wait (s.mutex);
3       bsem delay;                 15      s.count--;                  25      s.count++;
4       int count;                  16      if (s.count < 0) {          26      if (s.count <= 0)
5  };                               17          signal (s.mutex);       27          signal (s.delay);
6                                   18          wait (s.delay);         28      signal (s.mutex);
7  void init(csem &s, int v) {      19      } else {                    29 }
8       s.mutex = 1;                20          signal (s.mutex);
9       s.delay = 0;                21      }
10      s.count = v;                22 }
11 }
```

---

To mamy tak: 2 zasoby(wartość semafora =1) 4 wątki A, B, C, D. Kazdy robi wait, ale C i D są wywłaszczone przed 18 linijką. czyli `count=-2`. A robi signal:`count=-1, delay=1`, B robi signal: `count=0, delay=1`. wznawia się wątek C, dochodzi do 18 linijki, `delay=0`, robi signal i `count=1` więc nie wysyła signala na `delay` w linii 27, `delay=0`. Wątek D ma deadlocka na 18 linijce.

---

**Zadanie 6 (S).** Rozważmy zasób, do którego dostęp jest możliwy wyłącznie w kodzie otoczonym parą wywołań «acquire» i «release». Chcemy by wymienione operacje miały następujące właściwości:

* mogą być co najwyżej trzy procesy współbieżnie korzystające z zasobu,
* jeśli w danej chwili zasób ma mniej niż trzech użytkowników, to możemy bez opóźnień przydzielić zasób kolejnemu procesowi,
* jednakże, gdy zasób ma już trzech użytkowników, to muszą oni wszyscy zwolnić zasób, zanim zaczniemy dopuszczać do niego kolejne procesy,
* operacja «acquire» wymusza porządek „pierwszy na wejściu, pierwszy na wyjściu” (ang. FIFO).

Podaj co najmniej jeden kontrprzykład wskazujący na to, że poniższe rozwiązanie jest niepoprawne. Następnie zaproponuj poprawki do poniższego kodu i uzasadnij poprawność zmodyfikowanego rozwiązania.

```py
                mutex = semaphore(1) # implementuje sekcję krytyczną
                block = semaphore(0) # oczekiwanie na opuszczenie zasobu
                active = 0 # liczba użytkowników zasobu
                waiting = 0 # liczba użytkowników oczekujących na zasób
                must_wait = False # czy kolejni użytkownicy muszą czekać?

1  def acquire():
2  mutex.wait()                                 12 def release():
3  if must_wait: '#czy while coś zmieni?'       13      mutex.wait()
4       waiting += 1                            14      active -= 1
5       mutex.signal()                          15      if active == 0:
6       block.wait()                            16          n = min(waiting, 3)
7       mutex.wait()                            17          while n > 0:
8       waiting -= 1                            18              block.signal()
9  active += 1                                  19              n -= 1
10 must_wait = (active == 3)                    20          must_wait = False
11 mutex.signal()                               21      mutex.signal()

```

---

Znalezienie problemu jest proste: kod może zostać wywłaszczony przed 6 linijką po odblokowaniu mutexa. Weźmy taki przykład: 4 wątki, 3 wchodzą, 1 zostaje wywłaszczonych przed 6 linijką. 3 wątki które miały zasoby robią release i `must_wait` zmienia się na `false`. Przychodzi kolejny, piąty wątek, przeskakuje linijkę 3 i może iść do sekcji krytycznej. Złamana została właściwość 4: kolejność FIFO. Dodatkowo widać że while nic tutaj nie zdziała ¯\\\_(ツ)_/¯. (Dodatkowo pytanie czy semafor ma kolejkę: jeśli tak to ok, ale jeśli nie to to nie ma sensu, bo nie wiemy który z czekających wątków pierwszy dostanie acquire w linijce 6).  

Poprawki są już trochę cięższe do wymyślenia: można by zrobić jak na kodzie poniżej. Mamy wtedy pewność, że nie wychodzimy z mutexa w ważnych momentach. Każdy wątek który wychodzi z acquire pociąga za pomocą signala w 10 linijce kolejny wątek jeśli tylko nie był to 3 aktywny wątek, który dopchał nam wszystkie sloty. W condvarze na 4 linijce czekamy jeśli mamy wszystkie sloty okupione, albo są jakieś niewciągnięte wątki z kolejki. Dodatkowo, jeśli wiadomo, że signal na condvarze z pustą kolejką nic nie robi, więc jest git, bo 10 linijka wciągnie nam wątek jeśli takowy istnieje. Jeśli sloty są wolne, to wątek przeskakuje od razu przez 4-5, i sobie działa. 15 natomiast tak jak 10 pociąga jeden wątek (który potem pociągnie kolejny) z kolejki, a jeśli jest pusta to nic się nie dzieje. Problemem z kolei będą fake(spurious) i lost wakeupy, no ale można zawsze skorzystać z wersji condvara, która tego nie ma (a takie istnieją). No ale ja nie jestem pewien co do tego rozwiązania, więc się nie będę wymądrzał. Jak ktoś zna lepsze to się chętnie nauczę. Z semaforem jest niestety taki problem, że jak go w mutexie zablokujemy to mamy deadlocka, bo się mutex nie odblokuje. Próbowałem rozwiązań na kilka mutexów, mutexy i semafory, ale w najlepszym wypadku miałem taki problem, że relase i acquire nie mogły być atomowe.

```py
            m = mutex() # implementuje sekcję krytyczną. Niech ma dodatkowo kolejkę.
            cond = condvar() # oczekiwanie w sekcji krytycznej
            acc = 0 # liczba użytkowników zasobu
            waiting = 0 # liczba użytkowników oczekujących na zasób
            full = False # czy kolejni użytkownicy muszą czekać?

1  def acquire():
2      m.lock()                                 12 def release():
3      waiting += 1                             13     m.lock()
4      if (waiting += 1) > 1 or full:           14     active -= 1
5          cond.wait(mutex)                     15     if full and active == 0:
6      waiting -= 1                             16         cond.signal()
7      acc += 1                                 17         full = False
8      full = (acc == 3)                        18     m.unlock()
9      if not full:
10         cond.signal()
11     m.unlock()

```

---

**Zadanie 7 (P).** Opisz semantykę operacji «FUTEX_WAIT» i «FUTEX_WAKE» mechanizmu [futex(2)](http://man7.org/linux/man-pages/man7/futex.7.html) wykorzystywanego w systemie Linux do implementacji środków synchronizacji w przestrzeni użytkownika. Podaj w pseudokodzie implementację funkcji «lock» i «unlock» **semafora binarnego** korzystając wyłącznie z **futeksów** i atomowej instrukcji compare-and-swap. Odczyty i zapisy komórek pamięci są atomowe. 
> **Podpowiedź:** Wartość futeksa wyraża stany: `(0) unlocked`, `(1) locked ∧ |waiters| = 0`, `(2) locked ∧ |waiters| ≥ 0`.

* ***semafor binarny***: taki mutex bez właściciela XD - no semafor co ma wartości tylko 0 i 1.
* [***futeks***](https://en.wikipedia.org/wiki/Futex): fast userspace mutex - specjalny mutex który implementuje wyższe koncepty jak np. semafory, POSIX-owe mutexy i condvary. 
* *FUTEX_WAIT(addr, val)*: jeśli wartość po adresem `addr` jest `val` to usypia obecny wątek
* *FUTEX_WAKE(addr, num)*: budzi `num` wątków czekających na zmiennej `addr`
* *FUTEX_REQUEUE(old_addr, new_addr, num_wake, num, val)*: jeśli wartość pod `old_addr` jest `val` to przerzuca `num` czekających na `old_addr` na `new_addr`

```js
var bsem = {cnt=1}

lock(){
    while(!compare_and_swap(bsem.cnt, 1, 0)) // iff cnt==0 or couldn't write
        FUTEX_WAIT(bsem.cnt, 0); // no deadlock as u can see
    // exit only on successful swap AKA loc kacquire
}

unlock(){
    compare_and_swap(bsem.cnt, 0, 1); // cnt always 0 at this point so it works
    FUTEX_WAKE(bsem.cnt, 1);
}

```