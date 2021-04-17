# Tema1SO-MiniPreprocessor
[Tema1 - Sisteme de Operare] Tema presupune implementarea unui mini preprocesor pentru fișiere conținând cod sursă C.  <br>
Enuntul se gaseste [aici](https://ocw.cs.pub.ro/courses/so/teme/tema-1) <br>
Scheletul de cod se gaseste [aici](https://github.com/systems-cs-pub-ro/so/tree/master/assignments/1-multi/checker/multi).
<br>

#### COMPILARE SI RULARE
```
    LINUX     (se foloseste GNUmakefile) 
      ► in directorul curent se executa comanda make, obtinandu-se executabilul so-cpp
      ► RULARE LOCALA
          ./so-cpp [-D <SYMBOL>[=<MAPPING>]]  [-I <DIR>]  [<INFILE>]  [[-o] <OUTFILE>]
      ► RULARE CHECKER - se copiaza executabilul in fisierul in care se afla ./run_all.sh (multi)
          ./run_all.sh  sau  make -f Makefile.checker

    WINDOWS   (se foloseste Makefile)
      ► din Powershell se va executa comanda nmake, obtinandu-se executabilul so-cpp.exe
      ► din Cygwin se urmeaza aceeasi pasi ca la Linux 
```

#### ORGANIZARE

Tema presupune implementarea unui mini preprocespr pentru fisiere continand cod
sursa C. 
- ```hashmap.c``` - implementarea structurii de HashMap se afla 
- ```directory.c``` - implementarea unei structuri pentru retinerea numelor directoarelor date ca
paramentru pentru directiva -I
- ```so-cpp.c``` - parsarea argumentelor si preprocesarea fisierului C


#### IMPLEMENTARE

Am implementat intreg enuntul temei. Am inceput prin implementarea structurii de HashMap
si a structurii care retine numele directoarelor (care este un array de siruri). Procesarea
incepe prin parsarea  argumentelor din linia de comanda, retinerea fisierelor de input
si output, adaugareade perechi (key, value) la intalnirea directivelor -D, retinerea
directoarelor specificate cu -I. Dupa parsare se verifica daca argumentele sunt corecte,
daca se face citirea/scrierea de la stdin/stdout. 

Se parcurge fisierul linie cu linie, adaugandu-se ```#define``` atunci cand se intalnesc.
Pentru ```#define``` imbricate, se apeleaza functia de applyDefine pe valoarea asociata cheii
inainte ca aceasta sa se introduca in hashmap. Pentru ```#define``` pe mai multe linii, 
se salveaza intr-un buffer liniile pana se intalneste o linie care nu mai contine "\" la 
final. Pentru ```#include```, se obtine directorul curent si se testeaza daca fisierul header
exista acolo. Daca nu, se testeaza pe rand directoarele incluse cu -I. Cand se gaseste
un director in care exista fisierul header, continutul acesta se parseaza prin apelarea
recursiva a functie parseContent. 

In cazul clauzelor ```#if```, se testeaza conditiile si se scriu in fisierul de iesire doar 
blocurile care corespund ramurii din if care este adevarata. Pentru ```#undef```, se sterge
perechea (key, value) corespunzatoare din HashMap. Liniile normale din fisier se
scriu pe rand, inlocuindu-se variabilele declarate cu #define din continutul lor. Se
tine cont ca pot exista chei intre ghilimele, caz in care nu se inlocuiesc.

#### BIBLIOGRAFIE 

- https://ocw.cs.pub.ro/courses/so/laboratoare/laborator-01
- https://stackoverflow.com/
