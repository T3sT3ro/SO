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

---

**Zadanie 2**. Czemu fragmentacja plików jest szkodliwym zjawiskiem? Opisz techniki zapobiegania fragmentacji bazowane na zakresach (ang. extent) i odroczonym przydziale bloków (ang. delayed allocation). Naszkicuj algorytm defragmentacji systemu plików bazującego na i-węzłach i zakresach. Zadbaj o to, by proces nie naruszył spójności systemu plików w przypadku awarii zasilania.

---

Zmiana pozycji głowicy dysku magnetycznego (nie SSD) kosztuje!
Najkorzystniejszy przydział ciągły. Jak zapobiegać fragmentacji?

* pozostawienie miejsca za ostatnim blokiem (koniec pliku - *extent*)
* odroczony przydział bloków (ang. delayed allocation) grupujemy w pamięci zapisy wymagające przydziału nowych bloków  
* usługa defragmentacji narzędzie użytkownika lub wątek jądra

---

**Zadanie 3**. Rozważamy liniową organizację katalogów. Pokaż jak wyszukiwać, dodawać, usuwać i zmieniać nazwę wpisów katalogów. Kiedy warto przeprowadzić operację kompaktowania katalogu? W jakich przypadkach możliwe jest odwrócenie operacji skasowania pliku (ang. undelete)?

---

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

---

**Zadanie 7**. Czym różni się księgowanie metadanych od księgowania danych? Na podstawie wydruku polecenia `logdump` programu `debugfs` i opisu [struktur dyskowych ext4](https://bit.ly/2FrMAKw) opisz format dziennika. Opisz znaczenie poszczególnych bloków z których może składać się pojedyncza transakcja. Czemu operacje składowane w dzienniku muszą być idempotentne?

---

[Wypluwka z `logdump`](https://hastebin.com/miyahemola.sql)

---