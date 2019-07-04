# Testy pre projekt SIMPLIFY-BKG do predmetu FLP 2017/2018

## O testoch
Testy pre projekt na odstránenie zbytočných symbolov do predmetu FLP. Testy overujú funkčnosť spracovania argumentov, potom správne detekovanie chybných gramatík na vstupu a správny výpis po zadaní validnej gramatiky a validných prepínačov (-i, -1, -2).

## Popis fungovania testov

Pred spustením testov je nutné vytvoriť archív na odovzdanie v tvare ```flp-fun-xlogin00.zip```. Pre spustenie testov sa používa skript ```./it_is_ok.sh xlogin00```, ktorý daný archív rozbalí, vytvori zložku s názvom, ktorý odpovedá loginu, a spustí skript ```./run_tests.sh```. Tento súbor obsahuje popis jednotlivých testov s parametrami, ktoré špecifikujú, aká akcia sa má vykonať, a či má daný test skončiť úspešne alebo nie.

Zložka s testami obsahuje dva adresáre. Prvým adresárom je ***dir_in***, ktorý obsahuje vstupy, a ***dir_ref*** obsahuje očakávané výstupy.

Ďalšou možnosťou spustenia testov je skompilovať projekt simplify-bkg, tak aby jeho výstupom bol spustiteľný súbor s názvom ```simplify-bkg``` a následne len spustiť skript ```./run_tests.sh xlogin00```.

## Adresárová štruktúra
```
├── dir_in           // zložka zo vstupnými súbormi
│   ├── test*.in
├── dir_out          // zložka obsahujúca výstupy programu
├── dir_ref          // zložka s referenčnými výstupmi
│   ├── test*-i.out
│   ├── test*-1.out
│   ├── test*-2.out
├── it_is_ok.sh      // vstupný skript pre spustenie testov
├── readme.md        // dokumentácia
├── run_tests.sh     // spustenie jednotlivých testov
```
## Rozdelenie testov
Testy sú zoskupené do troch častí. Testy, ktoré začínajú číslami:
* 00-19 sú testy na overenie argumentov
* 20-59 sú testy na overenie spracovania chybnej gramatiky
* 60-99 sú testy na overenie správnosti algoritmu a výpisu (parametre -1, -2, -i)

## Autori
Autormi testov sú Klára Nečasová (xnecas24) a Peter Tisovčík (xtisov00). Klára Nečasová vytvorila
 skript ```it_is_ok.sh``` a testy od 30-99 (vstupy a očakávané výstupy). Peter Tisovčík vytvoril skript ```run_tests.sh``` a testy od 00-29 (vstupy a očakávané výstupy).
