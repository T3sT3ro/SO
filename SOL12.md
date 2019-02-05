# SO Lista 12

---

* Stallings (wydanie siódme): 12.7 – 12.9
* Tanenbaum (wydanie czwarte): 4.3 – 4.5, 10.6

---

**Zadanie 1**. Rozważamy następujące metody przydziału plików: ciągłą, listową, indeksowaną, i-węzeł. Dysponujesz bitmapą wolnych bloków i listą wszystkich i-węzłów. Czy i jak można naprawić poniższe błędy dla przydziału:

* ciągłego – uszkodzenie jednego z elementów pary,
* listowego – wskaźnik odnosi się do wolnego bloku lub zajętego bloku z innego pliku,
* indeksowanego – dwa indeksy w tablicy wskazują na ten sam blok,
* i-węzeł – uszkodzenie wskaźnika na blok pośredni.

---

    [pdf by Basiak M.](https://docdro.id/j345P2D)

---

**Zadanie 2**. Czemu fragmentacja plików jest szkodliwym zjawiskiem? Opisz techniki zapobiegania fragmentacji bazowane na zakresach (ang. extent) i odroczonym przydziale bloków (ang. delayed allocation). Naszkicuj algorytm defragmentacji systemu plików bazującego na i-węzłach i zakresach. Zadbaj o to, by proces nie naruszył spójności systemu plików w przypadku awarii zasilania.

---

Zmiana pozycji głowicy dysku magnetycznego (nie SSD) kosztuje!
Najkorzystniejszy przydział ciągły. Jak zapobiegać fragmentacji?

* pozostawienie miejsca za ostatnim blokiem (koniec pliku - *extent*)
* odroczony przydział bloków (ang. delayed allocation)
grupujemy w pamięci zapisy wymagające przydziału nowych bloków
* usługa defragmentacji narzędzie użytkownika lub wątek jądra

pojęcia [`wykład`](https://docdro.id/W4DG4Sv)

Algorytm:

```none
  i-węzły- drzewa
  zakres - czyli za plikiem chcemy jeszcze troche miejsca
  zakładam że mamy bitmapę, bo chyba zawsze jest(jak inaczej stwierdzić czy dany blok wolny, czy do jakiegoś pliku należy)

  Dla każdego pliku:
  Przechodząc po drzewie wiemy jak pofragmentowany, ile zajmuje w jakich zakresach bloki
  Sprawdzamy czy miejsce które chcemy zająć przez defragmentacje wolne(wliczając extent)
  Jeśli nie to sprawdzmy do kogo należy, skopiujmy blok po bloku w wolne miejsce (najpewniej na koniec dysku) i przepinamy info z i-node na nowe, wyczyśćmy stare
  Skopiujmy bloki naszego pliku kolejno przepinając i-nody i czyszcząc stare
  Nic się nie psuje bo kopiujemy i jak mamy 2 kopie to dopiero przepinamy wskaźnik(atomowe) i usuwamy starą
  Jak się tak przelecimy(he he) po plikach od początku do końca dysku to bedzie ok
```

---

**Zadanie 3**. Rozważamy liniową organizację katalogów. Pokaż jak wyszukiwać, dodawać, usuwać i zmieniać nazwę wpisów katalogów. Kiedy warto przeprowadzić operację kompaktowania katalogu? W jakich przypadkach możliwe jest odwrócenie operacji skasowania pliku (ang. undelete)?

---

**Tanenbaum rys 4.15s**
Wyszykiwanie: No słabo bo musimy wszystko przejrzeć dopóki nie znajdziemy #linówka
Usuwanie: Jak wiemy dokładnie, które entry usunąć to O(1) ale zostaje brzydka dziura która zapewne nie zostanie zapełniona całkowicie przez inny wpis
Dodawanie: znajdujemy wolne miejsce i tam wrzucamy 
Rename: Jak krótsza nazwa to ez zmieniamy, jak dłuższa to problem pewnie trzeba znaleźć nowe miejsc a stary wpis usunąć
Kompaktowanie: Albo przy każdym usuwaniu albo jak dużo dziur

Undelete: Czyli chodzi pewnie o to jak wrócić do entry
To jak nic nam miejsca nie zajęło, mamy plik i referencję do niego to ez można cofnąć.

---

**Zadanie 4 (P)**. Z użyciem programu [`debugfs`](https://bit.ly/2Cmhrnv) dla wybranej instancji systemu plików ext4 pokaż:

* fragmentację systemu plików (freefrag) i informacje o grupach przydziału (stats),
* zakresy bloków z których składa się wybrany duży plik (extents),
* że dowiązanie symboliczne może być przechowywane w i-węźle (idump),
* do jakiego pliku należy wybrany blok (blocks, icheck, ncheck),
* reprezentację liniową małego katalogu (bdump).

> UWAGA! Narzędzie `debugfs` działa domyślnie w trybie tylko do odczytu, więc możesz go bezpiecznie używać na swoim komputerze. Trybu do odczytu i zapisu używasz na własną odpowiedzialność!

---

[Prosz...](https://hastebin.com/ojiwutuwog.coffeescript) Nie wiem ile z tego jest dobrze ^^ (3 i 4 podpunkt wyglądają dziwnie). I nie chce mi się nic nie opisywać.

---

**Zadanie 5 (P)**. Korzystając z §3 artykułu [„A Directory Index for Ext2”](https://bit.ly/2suss1v) opisz strukturę danych HTree i operację wyszukiwania wpisu katalogu o zadanej nazwie. Następnie wyświetl reprezentację HTree dużego katalogu, np. `/var/lib/dpkg/info`, używając polecenia `htree` programu `debugfs`.

---

Mają maksymalnie dwa poziomy w obecnej implementacji(i to już dla dużych katalogów!), bo pozwalają indeksować ~30mln wpisów. 3 poziom zwiększałby to do 30mld. Blok indeksu 4K. Każda nazwa pliku ma swój hash i są one dzielona(po najmniejszym) na niższe poziomy indeksów. Root(+headery Htree) jest umieszczony w pierwszym bloku pliku katalogu. Liczenie hasha korzysta najpierw z headerów do sprawdzenia poprawności danych( a w razie niepowodzenia wyszukiwanie sprowadza się do liniowego przeszukania + indeks ma referencje na prawdziwe bloki katalogów ext2 dla kompatybilności), dopiero potem liczy hash nazwy. Wyszukuje binsearchem index, który zawiera dany hash(bo są posortowane i stałego rozmiaru). Po znalezeniu indeksu lookup idzie jak normalnie by nie było indeksu, przez liniowe przejście go. Jeśli go nie ma to idzie do ojca sprawdzić czy bit kolizji hashy jest ustawiony i jeśli tak, to przechodzi do kolejnego bloku w indeksie i powtarza operację. [Oto co wypluwa debugfs](https://hastebin.com/digoyazuhe.sql).

---

**Zadanie 6**. Rozważamy uniksowy system plików podobny do [ext2](https://bit.ly/2QM9xZV), tj. mamy bitmapę wolnych bloków i i-węzłów, wskaźniki na bloki pliku przechowujemy w i-węźle i w blokach pośrednich. Wymień kroki niezbędne do realizacji następujących operacji: dodanie pliku do katalogu, dopisanie kilku bloków na koniec pliku, przeniesienie pliku do innego katalogu. Zakładamy, że poszczególne kroki operacji są zawsze wdrażane w określonej kolejności dzięki zapisom synchronicznym.

---

Wydaje się zbyt łatwe wiec nw, ale w linku z rzeczy które nas obchodzą jest w sumie tyle:

* ***Inodes*** - Each file is represented by a structure, called an inode. Each inode contains the description of the file: file type, access rights, owners, timestamps, size, pointers to data blocks. The addresses of data blocks allocated to a file are stored in its inode. When a user requests an I/O operation on the file, the kernel code converts the current offset to a block number, uses this number as an index in the block addresses table and reads or writes the physical block.
* ***Directories*** - Directories are structured in a hierarchical tree. Each directory can contain files and subdirectories. Directories are implemented as a special type of files. Actually, a directory is a file containing a list of entries. Each entry contains an inode number and a file name. When a process uses a pathname, the kernel code searchs in the directories to find the corresponding inode number. After the name has been converted to an inode number, the inode is loaded into memory and is used by subsequent requests.

1.Dodanie pliku do katalogu.
Czyli jak rozumiem to mamy stworzony plik czyli też i-node który mu odpowiada.
Dodajemy nowe entry w directory z nazwą pliku i jego i-nodem.

2.Dodanie na koniec pliku.
Alokujemy tyle bloków ile nam trzeba(najlepiej za istniejącym plikiem jeśli możliwe).
Podpinamy je do i-noda pliku.
Kolejno zapisujemy do bloków z danymi offsetami

3.Przenoszenie pliku
Usuwając wpis(entry) z dir'a z którego przenosimy zapamiętujemy nazwę i i-noda
Dodajemy nowe entry w docelowym directory z nazwą pliku i jego i-nodem.

---

**Zadanie 7**. Czym różni się księgowanie metadanych od księgowania danych? Na podstawie wydruku polecenia `logdump` programu `debugfs` i opisu [struktur dyskowych ext4](https://bit.ly/2FrMAKw) opisz format dziennika. Opisz znaczenie poszczególnych bloków z których może składać się pojedyncza transakcja. Czemu operacje składowane w dzienniku muszą być idempotentne?

---

[Wypluwka z `logdump`](https://hastebin.com/miyahemola.sql)
*księgowanie([journaling](https://en.wikipedia.org/wiki/Journaling_file_system))* - najpierw dane na dziennik potem dopiero na dysk. w księgowaniu metadanych nie księguje się właściwego zapisu danych pliku. Różnice są takie, że krok 3 z [wiki](https://en.wikipedia.org/wiki/Journaling_file_system#Logical_journals) niebyłby zapisany do journala.

*indempotentne* - czyli operacje mogą być stosowane wielokrotnie bez zmiany wyniku.
> Dziennik to specjalny plik (i-node 8) przechowujący bufor cykliczny transakcji składających się z operacji idempotentnych. Wielokrotne wykonanie operacji idempotentnej nie zmienia wyniku: przydziel blok (za każdym razem inny?) vs. oznacz blok k jako używany. Inaczej jakby nam system nawalał to moglibyśmy sobie zarypać cały dysk przez coś w stylu "przydziel blok -> awaria". W trakcie działania system najpierw sekwencyjne zapisuje modyfikacje (z reguły tylko metadanych) do dziennika. Gdy tylko upewni się, że operacje zaksięgowano wdraża je w życie, po czym unieważnia odpowiednie wpisy dziennika. Po awarii fsck musi zlokalizować czoło i ogon kolejki transakcji, odtworzyć je i sprawdzić spójność metadanych. Przykładowe operacje wymagające księgowania: zmiana licznika referencji i-węzła, przydział lub zwolnienie bloku, zmiana położenia wpisu katalogu (kompaktowanie).

"Tannenbaum 10.6 str 791":
> The journal is a file managed as a circular buffer. The journal may be stored on the same or a separate device from the main file system. Since the journal operations are not "journaled" themselves, these are not handled by the same ext4 file system. Instead, a separate JBD (Journaling Block Device) is used to perform the journal read/write operations. JBD supports three main data structures: log record, atomic operation handle, and transaction. A log record describes a low-level file-system operation, typically resulting in changes within a block. Since a system call such as wr ite includes changes at multiple places—i-nodes, existing file blocks, new file blocks, list of free blocks, etc.—related log records are grouped in atomic operations. Ext4 notifies JBD of the start and end of system-call processing, so that JBD can ensure that either all log records in an atomic operation are applied, or none of them. Finally, primarily for efficiency reasons, JBD treats collections of atomic operations as transactions. Log records are stored consecutively within a transaction. JBD will allow portions of the journal file to be discarded only after all log records belonging to a transaction are safely committed to disk. Since writing out a log entry for each disk change may be costly, ext4 may be configured to keep a journal of all disk changes, or only of changes related to the file-system metadata (the i-nodes, superblocks, etc.). Journaling only metadata gives less system overhead and results in better performance but does not make any guarantees against corruption of file data. Several other journaling file systems maintain logs of only metadata operations (e.g., SGI’s XFS). In addition, the reliability of the journal can be further improved via checksumming.

[struktura dziennika](https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout#Journal_.28jbd2.29)

---
