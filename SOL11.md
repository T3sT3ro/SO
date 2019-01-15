# SO Lista 11

* Tanenbaum (wydanie czwarte): 4.1 – 4.3, 10.6
* Stallings (wydanie dziewiąte): 12.1 – 12.6

---
**Zadanie 1**. Na podstawie rozdziału §12.2 podręcznika Stallings wyjaśnij różnice między organizacją **sterty** i **pliku sekwencyjnego**. Rozważmy poniższe przypadki użycia:

* Zaproponuj binarny format przechowywania dziennika operacji składających się z listy par klucz-wartość złożonych ze znaków alfanumerycznych. Podaj semantykę kodów ASCII o numerach 28–31.
* Dodajemy rekordy do pliku przechowującego użytkowników systemu. Jak zachować porządek używając pliku dziennika lub listy dwukierunkowej? Jak przyspieszyć wyszukiwanie z użyciem indeksu?

---

![Pile vs Sequential](https://i.imgur.com/eue4GOD.png)

* ***sterta***: Najprostszy format - zmienna długość rekordów dopisywanych w kolejności przychodzenia. Rekord sam opisuje swoją strukturę - każde pole powinno mieć swoje własne opisy pól i ich wartości. Do wyszukania rekordu z pewnym polem trzeba przejrzeć wszystkie rekordy (aż do natrafienia). Do wyszukania wszystkich takich rekordów cały plik musi zostać przeglądnięty.
* ***plik sekwencyjny***: Każdy rekord ma taką samą długość i strukturę. Nie trzeba przechowywać w związku z tym informacji o polach w rekordach, bo wiadomo z góry gdzie się zaczynają i jaką mają długość. trzeba jeszcze na początku pliku przechowa nagłówek z metadanymi mówiącymi o strukturze rekordów. Każdy rekord ma swój unikalny klucz (na ogół jest jako pierwszy), a te przechowywane są w alfabetycznej kolejności. Problematyczne jest dodawanie i usuwanie rekordów ze względu na konieczność sortowania (przesuwanie dużych obszarów pamięci). Jeśli nie chcemy męczyć się z kopiowaniem, to można zamiast wymuszania kolejności rekordów na fizycznym pliku trzymać to jako deque, ale tracimy wtedy możliwość randomowego instant wyszukiwania. Można by to usprawnić robiąc np. skip listę odpowiedniej głębokości, ale to są pewne metadane na które potrzebujemy przeznaczyćmiejsce. Modyfikacja randomowych rekordów jest za to fajna bez deque, bo można sobie w O(log n) szukać binarnie klucza rekordu.
* [ASCII 28-31](https://bit.ly/2RbW9U3):

  |ASCII|meaning|description|
  |---|---|---|
  |`28`|file separator|End of file. Or between a concatenation of what might otherwise be separate files.|
  |`29`|group separator|Between sections of data. Not needed in simple data files.|
  |`30`|record separator|End of a record or row.|
  |`31`|unit separator|Between fields of a record, or members of a row.|

* *DZIENNIK OPERACJI*: dziennik może być przechowywany łatwo jako sterta, bo nigdy nie cofamy się w pliku tylko dopisujemy do końca. Czyli mamy plik (numerki to ASCII): `time[31]opcode[31]key[31]value[31][30]...[28]...`. Każde pole kończy się `[31]`, każdy rekord `[30]` a każde zakończenie dziennika `[28]`. Struktura pól jest znana. Można dodać dodatkowe informacje jak np. `LEVEL`, `DESCRIPTION` etc. Jeśli natomiast jedna operacja ma przyjmować więcej par klucz-wartość (polecenie niejasne) to można dodać na końcu każdego `key[31]value[31]` znak `[29]`. Czemu sterta? Bo nie wiadomo jakie są wielkości klucza i wartości.
* *UŻYTKOWNICY SYSTEMU*: No sterta to się na to nie nadaje jeśli mamy ciągle zmieniać dane użytkownikom. Co to wgl znaczy "zachować porządek" ??? Jeśli ma to znaczyć kolejność dodawania użytkowników, to można zrobić dwuetapowe dodawania - najpierw na stertę(na dziennik ale boli bo dziennik musi być spójny jako ciąg danych na fizycznym nośniku, albo deque który nie musi być spójny), potem zmergować z plikiem sekwencyjnym. Jak uzbieramy wystarczającą ilość użytkowników, albo minie odpowiednio dużo czasu. Index to jak mówiłem wcześniej binary searchem po kluczach.

---
**Zadanie 2**. Rozważamy **hierarchiczną strukturę katalogów**. Czym różnią się ścieżka **absolutna**, **relatywna** i **znormalizowana**? Względem którego katalogu obliczana jest ścieżka relatywna? Jak zmienić ten katalog? Wyjaśnij czym są **punkty montażowe**, a następnie na podstawie [`mount(8)`](https://bit.ly/2Orvk8k) wyjaśnij znaczenie i zastosowanie następujących atrybutów punktów montażowych: `noatime`, `noexec` i `sync`.

---

* ***hierarchiczna struktura***: każdy katalog(poza korzeniem) ma swojego ojca. Inaczej drzewo.
* ***ścieżki***:
  * *absolutna*: podana cała ścieżka od katalogu root czyli np. `/usr/share/lib/*.obj`. Btw, `/` to katalog domowy w linuxie, więc żeby mieć absolutną trzeba dać go na początku.
  * *relatywna*: ścieżka relatywna do aktualnego katalogu(`pwd`). Można się odnosić do aktualnego katalogy za pomocą kropki:`.`. Zmieniamy `pwd` z użyciem `cd`(change directory). Wpisywać bez `/` na początku.
  * *znormalizowana*: ze względu na to, że każdy katalog zawiera fikcyjny link do siebie(`.`) i ojca(`..`) mogą wystąpić upośledzone formaty oznaczajace te same ścieżki `/usr/share/.././././.` i `/usr` to to samo. Ta druga to znormalizowana. A i oczywiście musi być absolutna.
* ***mount point***: katalog w aktualnym systemie plików, w którym dodatkowe systemy plików są aktualnie podpięte - bo można je podpinać właśnie komendą `mount` (HAHA! a na windowsie jeśli nie niemożliwe to na pewno jest to upośledzone jak karp z ogonem na czole). Defaultowe mount pointy są zlistowane w `/etc/fstab`. Parametry [`mount(8)`](https://bit.ly/2Orvk8k):
  * *`noatime`*: w systemie plików ext2 jest wartość atime (access). Kosztowna do aktualizacji. Ta opcja ją wyłącza i może pomóc w wydajności.
  * *`noexec`*: zapobiega wykonywaniu binarek na zamontowanym systemie plików. Wpływa na bezpieczeństwo
  * *`sync`*: wszystkie zmiany dla tego systemu plików są automatem flushowane na dysk bez buforowania + synchronicznie. Spadek wydajności dla HDD i innych wolnych nośników.

---
**Zadanie 3**. Wymień wszystkie **typy plików** w systemie Linux. Na podstawie strony [File system calls](https://goo.gl/e3n4yq) podaj listę istotnie różnych wywołań systemowych realizujących operacje na **plikach zwykłych** i **plikach urządzeń**. Które z operacji nie mają sensu dla plików o **dostępie sekwencyjnym** i dlaczego? Wyjaśnij znaczenie wszystkich **atrybutów pliku**, które można odczytać wywołaniem [`stat(2)`](https://bit.ly/2SExZ0W).

---

* ***typy plików***: z racji złożoności systemu ważne jest mieć różne typy plików które zawiearją różne informacje jako warstwę abstrakcji od danych. W sumie jest ich siedem: ***regularne***(tekstowe, obrazki, binarki, biblioteki dzielone), katalogi, ***urządzenie znakowe***(pozwalają się komunikować z zewnętrznym hardarem np. drukarki?), ***urządzenie blokowe***(jak poprzednio ale bardziej dyski, pamięć), lokalne gniazdo(do komunikacji międzyprocesowej np. X window server [ten co ssie](https://bit.ly/2lCnBqv)), nazwany potok, dowiązanie symboliczne.
> "to rodzaj zasobu jądra, które zostało udostępnione użytkownikowi przez interfejs interakcji z plikiem." - *Cahir*
* ***syscale***: Po listę odsyłam pod link, wystarczy czytać na bierząco i mieć trochę mózgu do analizowanie co już było ujęte. Co do różnych: truncate nie ma sensu na pliku urządzeń bo to by im ograniczało pamięć. regularny `CREAT`, `MEMFD_CREATE`(anon), `MK*`  też raczej nie mają sensu. Dla regularnych syscalle `*SYNC` nie mają sensu. Dla plików sekwencyjnych(po kolei jak lista) sensy nie mają żadne syscalle odnoścnie pisania pod offset `PWRITE`, zmiany kursora. Możliwe, że więcej(np. `FALLOCATE`), nie chce mi się więcej sprawdzać.
* ***atrybuty***: metadane do plików(wewnątrz i-node), pozwalają na lepsze nimi zarządzanie np. append only, no dump, immutable, undeletable, no copy-on-write etc. [`stat(2)`](https://bit.ly/2SExZ0W) może zobaczyć tylko te co struktura `stat` podana w manie. Przepisywać nie będę.

---
**Zadanie 4**. Jak zrealizować poniższe scenariusze wykorzystując flagi wywołania systemowego [`open(2)`](https://bit.ly/2FfGEDv)?

* Dopisywanie do pliku komunikatów diagnostycznych z wielu procesów z uniknięciem wyścigów.
* Nowy program nie może otrzymać otwartego pliku od procesu, który wywołał `execve(2)`.
* Umożliwiamy otworzenie nowego pliku dopiero wtedy, gdy zakończymy do niego zapis.
* Istnienie danego pliku wyraża fakt założenia blokady na zasób współdzielony przez procesy.

---

Flagi [`open(2)`](https://bit.ly/2FfGEDv) potrzebne do tych scenariuszy:

* `O_APPEND`
* `O_CLOEXEC` i tyle. xD
* polecenie ma mało sensu, ale jeśli może chodzi o to, że zapisujemy i zamykamy i chcemy otworzyć na nowo to `O_EXCL`+`O_CREAT`? / `O_TMPFILE`
* czyli jak plik jest to procesy są zablokowane??? o ch*j chodzi? Możliwe, że `O_PATH` i skorzystać z właściwości blokujących na samym deskryptorze pliku. Albo `O_EXCL` (`O_EXCL | O_CREAT` ?)

---
**Zadanie 5**. Zawartość pliku katalogu składa się z rekordów (**i-węzeł**, nazwa, typ-pliku). Czemu użytkownik nie ma bezpośredniego dostępu do pliku katalogu przy pomocy wywołań read i write? Podaj listę istotnie różnych wywołań systemowych realizujących operacje na katalogach. Czy zawartość katalogu jest posortowana? Kiedy wykonywana jest operacja **kompaktowania**?

---

* [***i-node***](https://bit.ly/2Ay107I): metadane pliku w systemie plików (między innymi ext2) bez jego danych i nazwy. Deskryptory wskazują na i-node. Mogą zawierać atrybuty, metadane(access/modify/change time) i uprawnienia.
* *katalog* - Tannenbaum 4.2-4.3: zdaje mi się, że read i write nie mają dostępu do plików katalogu ze względu na to, że jest to API do odczytywania informacji z pliku a nie ze struktur danych ten plik reprezentujący. Katalog to ciąg rekordów opisujących zawartość a nie jakiś tam pospolity .txt. To, że go widzimy to tylko udogodnienie, nie możemy go zmieniać bo to jest struktura danych systemu plików. Od tego są inne komendy, np. `readdir()`(Jakbyśmy mogli zmieniać to byśmy mogli przez przypadek coś przypadkowo ostro zjebać, bo wszystko na linuxie jest plikiem - foldery też). Co do zawartości: to jest plik indeksowany, więc nie jest sortowany alfabetycznie, ale ma jakąś wewnętrzną strukturę przyspieszającą wyszukiwanie (zbiór asocjacyjny nazwa-inode/coś innego). Może to być np. B+ tree. Kompaktowanie będzie wykonywane, żeby pozbyć się dziur jeśli wywalimy jakieś pliki z katalogu (bo nazwy(ASCII) są przechowywane w ciągu, więc mamy fragmentację danych jeśli coś wywalimy). Można to łatać jak Tannenbaum na obrazku 4-15. Na stacku piszą, że można tylko na odmontowanym systemie robić kompaktowanie.
* [*dir syscalls*](https://bit.ly/2VzsUsD): `MKDIR*`, `RMDIR`, `GETCWD`, `*CHDIR`, `CHROOT`, `GETDENTS*`, `LOOKUP_DCOOKIE`

---
**Zadanie 6**. Wyjaśnij różnice w sposobie implementacji **dowiązań twardych** (ang. hard link) i **symbolicznych** (ang. symbolic link). Podaj listę istotnie różnych wywołań systemowych realizujących operacje na dowiązaniach. Jak za pomocą dowiązania symbolicznego stworzyć w systemie plików pętlę? Kiedy jądro systemu operacyjnego ją wykryje? Czemu nie można utworzyć dowiązania twardego do pliku znajdującego się w innym systemie plików?

---

* ***hard link***: wskaźniki na i-węzły i są wliczane w licznik referencji pliku. Nie mają swojego własnego i-noda Reprezentują różne nazwy tego samego pliku w obrębie jednego systemu plików. Nie mogą [przekraczać granicy systemu plików](https://bit.ly/2Ax95cy)(bo nie wie nic o innym systemie plików więc i gdzie jest przechowywany faktyczny, fizyczny plik), wiązać do folderów(bo by powodowały pętle i burzyły drzewiastą strukturę) albo linkować do różnych partycji. Pozwalają uruchamiać np. skrypty z innych miejsc. Ale to są struktury danych! Po usunięciu(refs=0) kasują plik, a inaczej nie. Dodanie hard linka do hard linka zadziała jak dodanie hardlinka na plik. Taki trochę Union-Find którego ojcem jest i-node.
* ***symbolic link***: mają tylko ścieżkę do pliku(nazwę pliku, która potem wskazuje na inode) i nie dodają swoich danych do i-noda pliku (ref-cnt zostaje ten sam). Mają swojego własnego i-noda. Symlinki mogą wskazywać na foldery albo pliki w innych systemach (NFS). sym-linki nie usuwają pliku jeśli zostaną usunięte. Mogą tworzyć pętle. Mają inny numer inode jeśli zlistowane `ls -i`. Symlink na symlink to lista. Można zrobić pętlę, bo można `ln -s ...` zmieniać na co wskazują.
* [*syscalle na linkach*](https://bit.ly/2sbBOiE): `LINK`, `SYMLINK`, `UNLINK`, `READLINK`. Kernel może ewentualnie wykryć pętlę jeśli będzie musiał przejść za dużo linków(przepełnienie stosu bo przechodzenie rekurencyjne po linkach albo osiągnięcie limitu linków do przejścia) i wywali wtedy `ELOOP` errora.

---
**Zadanie 7**. Jaką rolę pełnią **uprawnienia** `«rwx»` dla katalogów w systemach uniksowych? Opisz krótko zastosowanie dodatkowych uprawnień: `«set-uid»`, `«set-gid»` i `«sticky»`. Przedstaw algorytm określania dostępu do pliku dla zadanego **właściciela** i **grupy**. Podaj przykład, w którym standardowy system kontroli dostępu jest zbyt ograniczony i należy użyć **ACL** (ang. access control list).

---

* ***uprawnienia(perms)***: mówią o tym co można robić z plikami: `r`:czytać, `w`:pisać, `x`:uruchamiać. Dla katalogów jest to odpowiednio: zawartość może być wyświetlona, można modyfikować zawartość(musi być razem z `x`), można do niej wejść `cd`. `x` jest dodatkowo dziedziczony(jeśli ojcu nie ma to u mnie nic nie można). `set-gid` powoduje, że każde tworzone wpisy stają się własnością grupy będącej właścicielem katalogu, `set-uid` analogicznie, ale jest ignorowane na wielu UNIXach i Linuxach ze wzgledów bezpieczeństwa(na FreeBSD działa). `sticky` to taki kontrolny bit, rozważany zawsze na końcu i mający najwięcej do gadania. Jeśli jest ustawiony to prawa to usuwania i zmieniania nazwy ma tylko właściciel. A właściciel to ten co tworzył plik(jego uid) a grupa to jego aktualna, główna grupa którą miał podczas tworzenia.

```python
# the code should work atomically for grant-revoke pair
# uid/gid - file properties
# uperm, gperm, operm - user,group,others permissions
# ruid/rgid - requesting user's ID/groupID
if ruid == uid:
    grant( uperm )
else if rgid.contains(gid):
    grant( gperm )
else:
    grant( operm )
if sticky and ruid != uid:
    revoke( DELETE|RENAME )
```

* ***ACL***: Bardziej zaawansowany system kontroli uprawnień, w których każdy plik może mieć zlistowanych użytkowników i grupy i odpowiednie uprawnienia dla każdego z osobna. Fajne w przypadku jak jest jakaś hierarchia użytkowników w formie drzewa i my chcemy dać dostęp odpowiednim ugrupom do edycji pewnych danych, np. niech grupa `engineers` i `testers` będą w osobnych branchach firmy np. jedni są w grupie `Berlin` inni w grupie `Sydney`, ale oboje pracują nad tymczasowym projektem `game`. Wtedy żeby nie robić galimatiasu i wprowadzać osobnej grupy `gameTesters` można wybiórczo dodać permisje do edytowania folderu `game/tests` tylko tym grupom. Zakładając teraz, że przychodzi pewien praktykant do grupy `testers` nie chcemy żeby coś spieprzył, więc chcemy mu odebrać np. uprawnienia do usuwania testów ale pozostawić wszystkie inne uprawnienia grupy `testers`. Normalnie nie dałoby się tego fajnie zrobić. z ACL wystarczy walnąć coś w stylu `game/tests BOB:-DELETE`. [Algorytm ACL](https://bit.ly/2CV2mep).

---