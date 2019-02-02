# SO Lista 1

* Tanenbaum (wydanie czwarte): 1.1, 1.2, 1.4, 1.5
* Stallings (wydanie dziewiąte): 2.1 – 2.2, 13.3, 13.4

## **Zadanie 1**

Wyjaśnij różnice między **powłoką** (ang. shell), **system operacyjnym** i **jądrem systemu operacyjnego** (ang. kernel). W tym celu dobierz kilka przykładów powszechnie wykorzystywanego oprogramowania. Jakie są główne zadania systemu operacyjnego z punktu widzenia programisty?

---

* ***shell*** : program działający w OS-ie, dostarcza interfejs użytkownika do komunikacji z OS-em. Mogą być tekstowe i graficzne. Np. Corn, Z-shell, Bash, Powershell
* ***OS***: program ładowany przez BIOS/UEFI. Wykonuje niskopoziomowe komendy do podstawowej obsługi komputera, np. zarządza systemem plików, API dla programistów, żeby mogli pracować na sprzęcie etc. caching, paging, DMA(?).
* ***kernel*** : kawał kodu bliski hardwaru, udostępnia bazowe usługi: sterowniki urządzeń, zarządzanie pamięcią, procesami, syscalle.

---

## **Zadanie 2**

Na podstawie [dokumentacji](http://tldp.org/HOWTO/html_single/Debian-Binary-Package-Building-HOWTO/) wymień składowe **pakietu** deb ze szczególnym uwzględnieniem zawartości pliku `control`. Porównaj zarządzanie zainstalowanym oprogramowania z użyciem pakietów i instalatorów znanych z systemów nieuniksowych. Weź pod uwagę proces pobierania, weryfikacji, instalacji, konfiguracji i odinstalowania oprogramowania.

---

* ***pakiet*** : skompresowane archiwum z oprogramowaniem dla szybkiej instalacji/aktualizacji/dezinstalacji. Składowe pakietu deb są w internecie więc nie przepisuję.

| Unix | Android | Windows |
| --- | --- | --- |
| pobiera brakujące zależności | apk - pobiera wszystkie zależności | brak systemu dystrybuji. Zależności w rękach twórcy/usera |
| weryfikuje podpis cyfrowy i MD5(teraz możliwe że SHA-2 albo nowszy) | weryfikuje właściciel sklepu i SHA-2 + certyfikat | weryfikacja przez usera lub podpis cyfrowy |
| pakiet wie jak sięzainstalować, tylko potrzebne są brakujące | manager wie i instaluje | user może zarządzać instalacją |
| pakiet wie jak się odinstalować | manager umie odinstalować | user uruchamia deinstalator, który sobie (na ogół) radzi (mogą zostać wpisy w rejestrze i inny bloat)|

---

## **Zadanie 3**

Czym jest **zadanie** w **systemach wsadowych**? Jaką rolę pełni **monitor**? Na czym **polega planowanie** zadań? Zapoznaj się z rozdziałem „System Supervisor” dokumentu [IBM 7090/7094 IBSYS Operating System](http://bitsavers.org/pdf/ibm/7090/C28-6248-7_v13_IBSYS_Dec66.pdf). Wyjaśnij znaczenie poleceń języka kontroli zadań (ang. Job Control Language) użytych na rysunku 3 na stronie 13. Do jakich zastosowań używa się dziś systemów wsadowych?
> **Wskazówka**: Bardzo popularnym systemem realizującym szeregowanie zadań wsadowych jest [SLURM](https://slurm.schedmd.com/SC17/SlurmOverviewSC17.pdf).

---

* ***system wsadowy*** : na stałe rezyduje w pamięcie, nie wymaga nadzoru użytkownika. Zazwyczaj wyjście z niego jest wejściem innego, następnego urządzenia.
* ***zadanie*** : zbiór programów, który przetwarzał dane i wypluwał na wyjście
* ***monitor*** : pradziadek OS-a - interpretował język kontrolny, uruchamiał programy. Miał zegary, IO, ochronę pamięci, sterowniki.
* ***planowanie zadań*** : aka "w jakiej koleności zadanie" - na ogół FIFO, potem dodali priorytety.
* ***język kontroli zadań*** : polecenia dla monitora

Zastosowania systemów wsadowych:

* zarządzanie bazami danych np. hotelowymi, lotniskowymi
* SLURM(**S**imple **L**inux **U**tility for **R**esource **M**anagement) system kolejkowy do działać w centrach obliczeniowych
* rozdzielacz zadań w denderfarmach
* konwersje danych i filtry

---

## **Zadanie 4**

Jaka była motywacja do wprowadzenia **wieloprogramowych** systemów wsadowych? W jaki sposób wieloprogramowe systemy wsadowe wyewoluowały w systemy z **podziałem czasu** (ang. timesharing)? Podaj przykład systemu **interaktywnego**, który nie jest wieloprogramowy.

---

* ***wieloprogramowe*** : pierwsze wspierające wieloprocesowość, żeby ograniczyć czas oczekiwania na I/O. pamięć dzieliły na części i w każdej było osobne zadanie.
* ***z podziałem czasu*** : pierwsze umożliwiające interakcję w czasie działąnia programu. Wielu użytkowników na raz. Wprowadzono terminale. "Kwanty czasu" przydzielane zadaniom, przez co użytkowink myśli, że ma ciągłą interakcję z programem.
* ***system interaktywny*** : pozwala wpływać na system operacyjny podczas pracy innego programu. Interaktywny a nie wieloprogramowy był np. DOS i CP/M na Intela 8080.

---

## **Zadanie 5**

Bardzo ważną zasadą przy projektowaniu oprogramowania, w tym systemów operacyjnych, jest rozdzielenie **mechanizmu** od **polityki**. Wyjaśnij te pojęcia odnosząc się do powszechnie występujących rozwiązań, np. otwieranie drzwi klasycznym kluczem versus kartą magnetyczną.

---

* ***mechanizm a polityka*** : polityka to abstrakcja, zbiór zasad jakie mają być realizowane do zrobienia czegoś, np. to, że kolejka jest FIFO. Mechanizm to implementacja danej polityki, np. FIFO implementowane w taki sposób, że mamy listę z wskaźnikiem na czoło listy i każdy node ma wskaźnik na kolejny.

---

## **Zadanie 6**

Wymień mechanizmy sprzętowe niezbędne do implementacji **wywłaszczania** (ang. preemption). Jak użyć **algorytmu rotacyjnego** (ang. round-robin) do implementacji wielozadaniowości z wywłaszczaniem? Jakie zadania pełni **planista** (ang. scheduler) i **dyspozytor** (ang. dispatcher)? Który z nich realizuje politykę, a który mechanizm?

---

* ***wywłaszczenie*** : mechanizm pozwalający wstrzymać dany proces na rzecz innego. scheduler albo dispatcher może wstrzymać aktualny proces. Potrzebne do tego są pewne mechanizmy, m.in. przerwania. Najfajniejszym jest Timer Interrput. Inne to syscalle, żeby OS mógł odzyskać kontrolę. Trzeba zabezpieczyćmechanizm, żeby nie wyłączyć schedulera.
* ***round-robin*** : każdy program dostaje pewien kwant czasu i programy są na liście cyklicznej(scheduler - polityka). Jak kwant czasu mija to program, który się w tym czasie nie zakończył zostaje wywłaszczony przez dispatcher(mechanizm), i przeskakujemy do kolejnego. Jak sięskończy to go wywalamy z listy. Implementacja najłatwiejsza przez Timer Interrupt.

---

## **Zadanie 7**

Zapoznaj się z punktami podrozdziału [„The Elements of Operating-System Style”](http://www.catb.org/~esr/writings/taoup/html/ch03s01.html) książki „The Art of Unix Programming”. Opisz jak zaprojektować system sprzecznie z zasadami filozofii uniksowej. Czemu system operacyjny powinien (a) umożliwiać szybkie tworzenie procesów i łatwą komunikację międzyprocesową (b) przechowywać dane w plikach tekstowych, a nie binarnych (c) udostępniać szereg narzędzi programistycznych (d) oferować bogaty wybór programów działających w linii poleceń?

---

* Filozofia UNIXA :
  * wszystko jest plikiem (jak się okazuje to jednak nie tak wszystko ae nvm)
  * multitasking
  * szybkie procesy
  * komunikacja między procesami (IPC)
  * granice wewnętrzne prorgamów (ochrona pamięci, permisje)
  * brak rekordu i atrybutów
  * pliki są lepsze od binarek
  * CLI > GUI
  * user wie lepiej
  * casual programming (pakiety etc. - łatwiej dla programistów)


---

## **Zadanie 8**

Podaj główne cele projektowe systemów operacyjnych przeznaczonych dla **systemów czasu rzeczywistego** (ang. real-time), **sieci sensorów** (ang. wireless sensor networks) i **systemów wbudowanych** (ang. embedded systems). Rozważ następujące systemy: FreeRTOS5 , TinyOS (§13.4) i µCLinux (§13.3). Ich obszar zastosowań zacznie różni się od klasycznych SO – jak wpłynęło to na zakres oferowanych funkcji?
> **Wskazówka**: Takie systemy często obsługują sprzęt z małą ilością pamięci RAM i brakiem jednostki MMU lub pamięci trwałej.

---

Można łatwo wymyślić. Nie mam zamiaru się rozpisywać.

---