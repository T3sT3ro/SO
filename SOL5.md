# SO Lista 5

> * Tanenbaum (wydanie czwarte): 2.3, 6.1
> * Stallings (wydanie dziewiąte): 5.1 – 5.4, 6.1, 6.2

---
Zadanie 1. Wyjaśnij różnice między **zakleszczeniem** (ang. deadlock), **uwięzieniem** (ang. livelock) i **głodzeniem** (ang. starvation). W podręcznikach pojęcia te odnoszą się do **zasobów**. Pokaż, że podobne problemy występują w przypadku **przesyłania komunikatów**.

---

* ___deadlock___: wątek A czeka aż wątek B będzie w odpowiednim stanie (np. zwolni mutex do pliku x) a wątek B czeka na odpowiedni stan wątku A (np. zwolni mutex do pliku y). Wtedy kod nie wykonuje się a mamy pat (elektryk nie położy kabli przed rurami, a hydraulik nie położy rur przed elektryką)
* ___livelock___: wątek A robi coś dopóki jakiś stan się nie zmieni (np. iteruje i do n), natomiast wątek B czeka na pewne warunki, żeby przejść do innej pracy(np. iteruje i w dół). Oba są w sytuacji, w której jeden odwraca skutki działania drugiego więc nie mogą się zakończyć, ale procesor tyka w przeciwieństwie do deadlocka (robotnik A zasypuje dziurę piachem a B wyrzuca z dziury piach. A kończy jak dziura zasypana, B jak pusta).
* ___starvation___: wątek A czeka na pewne współdzielone zasoby, ale nie może się do nich dostać, bo inne zachłanne wątki kradną mu dostęp (np. synchronizowana metoda obiektu zajmująca dużo czasu - jak jeden wątek często ją wywołuje, to kradnie czas innym wątkom, które też wymagają częstej synchronizacji).
* ___resources___: zależy o jakie chodzi. Zasobem może być urządzenie na wyłączność np. drukarka. Może być CPU. Może być plik. Może być pamięć. Ogólnie coś co musi zostać: zdobyte → użyte → oddane
* ___≈IPC \(przesyłanie komunikatów___): wymiana informacji między procesami (na ogół w postaci pakietów). Można przez sockety, można przez pamięć współdzieloną, można przez pipy.
  * race: procesy A i B chcą drukować plik. Mamy plik który jest jak szpula do drukowania: bufor danych i zmienne `out`(który plik ma zostać wydrukowany) i `in`(następny wolny slot). {A pobiera `in` → context switch → B pobiera `in` → pisze nazwę pliku pod `in` → context switch → A nadpisuje `in` → nigdy nie będzie outputa z B}.
  * deadlock: pipe się zapchał od piszących, a czytający został w jakiś sposób zablokowany.
  * livelock: \#TODO

---
Zadanie 2. Wymień cztery warunki konieczne do zaistnienia zakleszczenia. W jaki sposób programista może **przeciwdziałać zakleszczeniom** (ang. deadlock prevention)? Których z proponowanych rozwiązań nie implementuje się w praktyce i dlaczego?

---

> tannenbaum 6.6 i [wikipedia](https://en.wikipedia.org/wiki/Deadlock)
* ___deadlock prevention___: Co robić, żeby nie spotkać deadlocka? Ubić jeden z 4 wymogów do jego powstania xD:
  * _mutual-exclusion condition_ (AKA tylko jeden naraz): Jeśli żaden zasób nie zostanie ekskluzywnie przypisany jednemu procesowi, nie będzie deadlocka - rób dane READ-ONLY, oddawaj zasób tylko w razie konieczności i małej ilości procesom.
  * _hold-and-wait confition_ (AKA proces trzyma ≥ 1 zasób i chce więcej): Jeśli pozbędziemy się przypadków, w których proces trzymający zasoby czeka na więcej zasobów - niech proces zarząda WSZYSTKICH zasobów które chce PRZED swoim uruchomieniem(optymalne to to niestety nie jest, procesy często nie wiedzą ile będą potrzebować zanim się nie uruchomią: dla procesu który {czyta → mieli 1h → pisze} I/O device będzie okupywane przez 1h...). Inny sposób to {uwonij tymczasowo wszystko co masz → poczekaj → gotta catch 'em all!}
  * _no-preemption condition_ (AKA zasób zwolnić może tylko aktualny właściciel): Jeśli proces ma zasób, wywłasz. Autor pisze: “_[...] tricky at best and impossible at worst._”. No ogólnie to można wirtualizować pewne zasoby żeby się pozbyć problemu wywłaszczania (drukarka - spooling), ale nie wszystkie się tak da(rekordy w bazach danych)
  * _circular wait condition_ (AKA processy czekają w cyklu na zwolnienie zasobów): Można na kilka sposobów:
    * zasada "jeden zasób na raz koleżko" - ze jak proces musi drukować dane z taśmy;
    * kolejność zasobów - możesz żądać kiedy chcesz ale tylko w ustalonej kolejności np. wg priorytetu (cykli nie będzie jak widać)
    * priorytet (modyfikacja poprzedniego): nie żądaj niższych priorytetów  

    dwa ostatnie mogą być ciężkie bo trudno wymyślić kolejność zasobów(priorytety) jak jest ich dużo.

---
> Niech zapis {P}I{Q} oznacza, że formuły P i Q są prawdziwe odpowiednio przed i po wykonaniu instrukcji I. Formuły te nazywamy kolejno warunkami wstępnymi (ang. preconditions) i warunkami końcowymi (ang. postconditions). Zauważ, że z racji wywłaszczania dla programu {P1}I1{Q1}; {P2}I2{Q2} nie musi zachodzić Q1 ≡ P2!  
**UWAGA!** W kolejnych zadaniach należy jawnie opisywać globalny stan przy pomocy formuł logicznych.

---
Zadanie 3 (S). W poniższym programie występuje **sytuacja wyścigu** (ang. race condition) na współdzielonej zmiennej `«tally»`. Wyznacz jej najmniejszą i największą możliwą wartość. Dyrektywa `«parbegin»` rozpoczyna współbieżne wykonanie procesów.

```c
1 const int n = 50;
2 shared int tally = 0;
3
4 void total() {
5   for (int count = 1; count <= n; count++)
6       tally = tally + 1; /* to samo co tally++ */
7 }
8
9 void main() { parbegin (total(), total()); }
```

Maszyna wykonuje instrukcje arytmetyczne wyłącznie na rejestrach – tj. kompilator musi załadować wartość zmiennej `«tally»` do rejestru, przed wykonaniem dodawania. Jak zmieni się przedział możliwych wartości zmiennej `«tally»`, gdy wystartujemy k procesów zamiast dwóch? Odpowiedź uzasadnij.

---

* ___race condition___:  sytuacja w której ≥ 2 wątku korzystają z tych samych danych w tym samym czasie i je modyfikują bez mechanizmów synchronizacji. AKA zachowanie kiedy wynik jest zależny od przeplotu operacji w środowisku wielowątkowym. Ogólnie to występuje jak mamy schemat A:`sprawdź czy 𝚇 → zadziałaj na 𝚇` i B robi coś na `𝚇` między tymi dwoma operacjami.

No to ogólnie w pseudokodowym ASMie to będzie wyglądało mniej więcej tak:

```assembly
cmp i, n        ;FOR; {tally=x}   cmp {tally=x}         {tally=x}   cmp {tally=x}   preempt
mov tally, %r_x     ; {tally=x}   mov {tally=x} preempt {tally=x+1} mov {tally=x+1} powrót
inc r1              ; {tally=x+1} inc {tally=x+1}  LUB  {tally=x+1} inc {tally=x+1}
mov %r_x, tally     ; {tally=x+1} mov {tally=x+1}       {tally=x+1} mov {tally=x+1}
```

Więc problem polega na tym, że wątki zapamiętają lokalnie starą wartość tally i na niej zrobią operacje, a potem zapiszą ją z powrotem do globala. To sytuacje są dwie: albo dwa zapamiętają starą wartość i nadpiszą globala zmodyfikowaną starą czyli jeden nie ma efektu, albo A nadpisze po czym B nadpisze, czyli będzie podwojony efekt. czyli tally może być `min=2` bo może być `A` fetch `B` fetch, `B` robi 49 operacji, A robi put `0++` czyli `tally=1`, `B` fetch, `A` robi 50 operacji, `B` robi `1++` czyli `tally=2` i koniec. `max=50*2`. No analogicznie jak jest więcej, bo mogą się szeregowo wszystkie wykonać, czyli będzie `max=50*k`, no ale to jest intuicyjne.

---
Zadanie 4 (S). Rozważmy poniższy kod ze slajdów do wykładu. Zakładamy, że kolejka `«queue»` przechowuje do n elementów. Wszystkie operacje na kolejce są **atomowe** (ang. atomic). Startujemy po jednym wątku wykonującym kod procedury `«producer»` i `«consumer»`. Procedura `«sleep»` usypia wołający wątek, a `«wakeup»` budzi wątek wykonujący daną procedurę. Wskaż przeplot instrukcji, który doprowadzi do  
(a) błędu wykonania w linii 6 i 13 (b) zakleszczenia w liniach 5 i 12.

```py
1   def producer():
2       forever:
3           item = produce()
4           if queue.full():
5               sleep()
6           queue.push(item)
7           if not queue.empty():
8               wakeup(consumer)

9   def consumer():
10      forever:
11          if queue.empty():
12              sleep()
13          item = queue.pop()
14          if not queue.full():
15              wakeup(producer)
16          consume(item)
```

---

* ___atomic operations___: inaczej operacje w trakcie których nie zmienimy wątku/procesu. Jak się wykonują to całe od początku do końca. Nie muszą być w jednym cyklu procesora.

* (a) błąd wykonania w lini 6 i 13 - chcemy doprowadzić do sytuacji w której mamy (dla linijki 6.) `{Q not full}14.{Q not full}`→`{Q full}15.{...}` i symetrycznie da 13 linijki.
  * _linijka 6_: startujemy z pełną kolejką: `11`[→³](#notes)`13`→`14`[⇛³](#notes)`3`→`4`→`6`→`7`→`8`(tutaj zakładam że wake na wątku który nie jest sleep nic nie robi)→`3`→`4`→`5`⇛`15`⇛`6`(error bo push do pełnej kolejki).
  * _linijka 13_: analogicznie i symetrycznie startując z pustej kolejki i linijki `3`→`4`→`6`→`7`⇛`11`→`13`→`14`→`15`→`16`→`11`→`12`⇛`8`→`13`(error bo pop na pustej Q)
* (b) zakleszczenie w 5 i 12 - chcemy tym razem doprowadzić do sytuacji 2 śpiących wątków, czyli pewnie coś w stylu `{consumer awake, Q full}4.{consumer awake, Q empty}`→`{consumer asleep, Q empty}5.{...}`
  * _linijka 5 i 12_: to startujemy z kolejką rozmiaru 1 i pełną. wtedy mamy `3`→`4`⇛`11`→`13`→`14`→`15`(nic bo consumer nie śpi)→`16`→`11`→`12`⇛`5`(deadlock, oba śpią)

> . "⇛" oznacza przełączenie wątku a "→" kolejną instrukcję

---
Zadanie 5. Przeczytaj rozdział 2 artykułu [„Beautiful concurrency¹”](https://goo.gl/92azcK). Na podstawie poniższego przykładu wyjaśnij czemu złożenie ze sobą poprawnych współbieżnych programów używających blokad nie musi dać poprawnego programu (tj. „locks are not composable”). Jak poprawić procedurę transfer? Czemu według autorów artykułu blokady nie są dobrym narzędziem do strukturyzowania współbieżnych programów?

```java
1   class Account {
2       int balance;
3       synchronized void withdraw(int n) { balance -= n; }
4       synchronized void deposit(int n) { balance += n; }
5   }
6
7   void transfer(Account from, Account to, int amount) {
8       from.lock(); to.lock();
9       from.withdraw(amount);
10      to.deposit(amount);
11      from.unlock(); to.unlock();
12  }
```

---

Widać, że withdraw i deposit są spoko napisane. Problemem jest transfer. Zakładając współbierzne wykonanie transfer(a,b,x) tranfer(b,a,x) można mieć deadlock bo from i to się zblokują naraz. Bez tych locków w 8 i 11 może być tak, że wątek sobie przeskoczy i mamy nagle tak, że suma pieniędzy w banku się nie zgadza. Można robić coś w stylu globalnego porządku locków (aka priorytety) ale weź tu wymyśl priorytety jak masz 20k miejsc z blokadami. No ogólnie to jest też problemowe. Ogólnie to blokady są słabe bo bużą moduarność - trzeba eksponować swoją strukturę innym (w tym wypadku transferowi) żeby można było dobrze napisać metody współbieżne. Ciężko jest rozwiązać problem jeśli się coś złapie deadlocka, bo mamy wtedy zwiechę całego systemu. Dodatkowo można łatwo zapomnieć np. o odblokowaniu mutexa, o sprawdzeniu conda, o locku w dobrej kolejności, bo nie ma bezpośrenio zależności między kodem a lockami - one istnieją tylko w głowie programisty. I teraz wyobraź sobie człowieku, że dajesz swój kod współbieżny w ręce innego programisty do debugowania...

---
Zadanie 6 (S). Poniżej znajduje się [propozycja²](https://goo.gl/mgpK4j) programowego rozwiązania problemu wzajemnego wykluczania(eng. mutual-exclusion) dla dwóch procesów. Znajdź kontrprzykład, w którym to rozwiązanie zawodzi. Okazuje się, że nawet recenzenci renomowanego czasopisma „Communications of the ACM” dali się zwieść.

```c
1   shared boolean blocked [2] = { false, false };
2   shared int turn = 0;
3
4   void P (int id) {
5       while (true) {
6           blocked[id] = true;
7           while (turn != id) {
8               while (blocked[1 - id])
9                   continue;
10              turn = id;
11          }
12          /* put code to execute in critical section here */
13          blocked[id] = false;
14      }
15  }
16  
17  void main() { parbegin (P(0), P(1)); }
```

---

przez A i B oznaczam wątek 0 i 1, więc ciąg instrukcji będzie prefixowany tym co się robi:  
`A6`[→³](#notes)`A7`→`A12`→`A13`[⇛³](#notes)`B6`→`B7`→`B8`⇛`A6`→`A7`→`A12`⇛`B10`→`B7`→`B12`(i mamy oba programy w krytycznej sekcji :< smuteczek)

---
Zadanie 7 (S, P). Podaj w pseudokodzie implementację ***semafora*** z operacjami `«init»`, `«down»` i `«up»` używając wyłącznie muteksów i zmiennych warunkowych. Dopuszczamy ujemną wartość semafora.
> Podpowiedź: `Semaphore = {critsec: Mutex, waiters: CondVar, count: int, wakeups: int}`

---

* ___semafor___: mechanizm do synchronizacji mówiący ile jest dostępnych zasobów. Zazwyczaj będąca licznikiem tych zasobów i pewną kolejką wątków. Mogą być binarne i zliczające. Ma metody:  
  * `wait(): cntr > 0 ? --cntr : Q+=thread.sleep()`
  * `post(): cntr = 0 && !Q.empty() ? Q.pop().wakeup() : ++cntr`

Wakeups służy niby do uniknięcia zagłodzenia:
> Property 3: if there are threads waiting on a semaphore when a
thread executes signal, then one of the waiting threads has to be
woken.

```Java
class Semaphore{
    Mutex mutex;
    CondVar cond;
    int cnt, wakeups; // the number of threads that have been woken but have not yet resumed execution
}

void init(Semaphore s, int val){ 
    s.cnt = val;
    s.wakeups = 0;
    s.mutex = new Mutex();
    s.cond = new CondVar();
}

void up(Semaphore s){ // AKA post() AKA signal()
    mutex_lock(s.mutex);
    s.cnt++;
    if(s.cnt <= 0){
        s.wakeups++;
        cond_broadcast(s.cond);
    }
    mutex_unlock(s.mutex);
}

void down(Semaphore s){ // AKA wait()
    mutex_lock(s.mutex)
    s.val--;
    if(s.cnt < 0){
        do{
            cond_wait(s.cond, s.mutex);
        } while(s.wakeups < 1)
        s.wakeups--;
    }
    mutex_unlock(s.mutex)
}

```

tutaj ["inspiracja"](http://greenteapress.com/thinkos/html/thinkos012.html) z ["wytłumaczeniem - str 81/82"](http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf).

---

## notes

[1] https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/beautiful.pdf  
[2] Harris Hyman, „Comments on a Problem in Concurrent Programming Control”, January 1966.  
[3] "⇛" oznacza przełączenie wątku a "→" kolejną instrukcję
