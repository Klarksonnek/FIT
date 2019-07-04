.. article:: apps_demo_msp_linka
   :author: Klara Necasova <xnecas24 AT stud.fit.vutbr.cz>
   :updated: 01.12.2016

==============================================================
MSP430: Řízení robota na výrobní lince (lze využít port uC/OS-II či FreeRTOS)
==============================================================
Zadání projektu
===================
Popis prostředí: 

1) výrobní linka je tvořena:
- lisem L, 
- přímým zásobním pásem (feed belt, FB, o délce 5 m, se samočinným, nepřetržitým posuvem k L konstantní rychlostí 10 cm/s) -- a SMAZAT -- 
- přímým odkládacím pásem (deposit belt, DB, o délce 5 m, se samočinným, nepřetržitým posuvem od L konstantní rychlostí 10 cm/s), se vzájemně vodorovnými směry posuvu
- rotačním robotem se dvěma vzájemně kolmými rameny (A, B) s úchopem a rovinou rotace vodorovnou s podlahou výrobní linky (tandemový pohyb umožňující mj. současně vzít/položit předměty - např. vzít předmět z FB ramenem A a vložit na L předmět ramenem B).

2) místo pro odebírání předmětů z FB, místo pro odkládání/odebírání předmětů na/z L a místo pro odkládání předmětů na DB představují vrcholy rovnoramenného pravoúhlého trojúhelníka se základnou mezi FB a DB a rameny mezi FB, L a DB, L. 

Požadavky na řízení: 

1) robot je schopen rotace vpravo/vlevo o násobky základního úhlu 90 stupňů 
- (řízení vstupem LEFT/*RIGHT - typu bool určujícím směr rotace, přičemž rotací vpravo/vlevo je myšlen pohyb ve/proti směru hodinových ručiček, tj. CW/CCW a vstupem STEPS udávajícím úhel rotace jako násobek základního úhlu v rozmezí 0 až 4)
- rychlost rotace robota je konstantní: 1 stupeň / 10 ms, 
2) po zásobním pásu přijíždí materiál k obrábění, 
3) přidání nového materiálu na začátek FB je signalizováno čidlem (signál IN), 
4) dojede-li materiál na konec FB, musí z něj být vzat na něj tam již čekajícím robotickým ramenem a přenesen nad L; jinak dochází k pádu materiálu na podlahu, 
5) zpracování materiálu na L trvá 100 ms, 
6) na L může být přítomen pouze jeden kus materiálu a materiál je z L možno vzít až po jeho zpracování,
7) vzetí/položení materiálu ramenem A/B trvá 1 s (řízení hodnotami GETA, PUTA, GETB, PUTB typu bool; pro provedení příslušné akce musí být hodnota GETx či PUTx v 1 a současně musí platit GETx=not(PUTx)), 
8) další příkaz může robot obdržet a začít vykonávat až po dokončení předchozího, 
9) uplynulo-li od poslední signalizace IN více než nebo právě 5000 ms a na FB není materiál, systém přejde do klidového stavu, ve kterém zůstane, dokud neobdrží signál přes IN, 
10) volba klidového a počátečního stavu robota je součástí zadání projektu.
    
Vstupy a výstupy řadiče:
	
Vstupy řadiče zahrnují vývody: 
- IN - signalizuje, že byl přidán materiál na FB,
- POS - údaj o aktuální poloze (natočení) ramene A nabývající jedné z hodnot 0 (A nad koncem FB), 1 (A nad L), 2 (A nad začátkem DB), 3 (A rovnoběžně s FB a DB v největší vzdálenosti od L).

Výstupy řadiče zahrnují vývody: 
- LEFT/*RIGHT - udává směr rotace ramen robota, 
- STEPS - úhel rotace o násobek základního úhlu (90 stupňů), rozmezí 0 až 4, 
- GETA, PUTA, GETB, PUTB - uchopení/položení materiálu ramenem robota (ramena A/B). 

.. figure :: vyrobni_linka.png
	:align: center
	
Řešení projektu
===================
Pro implementaci projektu bylo využito jádro FreeRTOS (RTOS = Real Time Operating System). 
Celkem bylo vytvořeno 6 asynchronních úloh, které budou níže detailněji popsány.
Pro řešení projektu byla využita demo aplikace pro FreeRTOS, která je k dispozici v SVN repozitáři FITKitu, konkrétně:
FITkit / trunk / apps / demo_msp / free_rtos / mcu / main.c

1) Klávesnice 
Vlákno každých 10 ms kontroluje stav klávesnice FITkitu. Pokud bylo stisknuto tlačítko "A" (Add), je nastaven signál IN do 
jedničky, čímž je signalizováno vložení materiálu na pás FB. Následně je volána funkce FBInsert(), která realizuje samotné vložení materiálu na pás FB.

2) Pásy 
Vlákno zajišťuje posun materiálu po pásech FB či DB. Vlákno se vyvolá tak často, jak je potřeba posouvat materiál po pásech. 
Pro reprezentaci prázdného pásu slouží znaky "=", pokud je pás obsazen materiálem, zobrazí se vyplněný obdélník. 

Vlákno mimo jiné kontroluje, zda materiál nespadl z pásu FB a také nastavuje stav robota, aby mohl být materiál z pásu FB odebrán. Pokud bude potřeba odebrat materiál z lisu, zavolá funkci, která zajistí otočení ramen robota k pásu FB. 

3) Robot 
a) stav NO_ACTION
- jedná se o počáteční stav konečného automatu 
- v tomto stavu robot čeká, až bude na pásu FB nějaký předmět
- na začátku je robot v poloze 0 -  rameno A nad koncem pásu FB
- jakmile je k dispozici předmět na pásu FB, přechází do stavu WAITING_FB

b) stav WAITING_FB
- pokud lis již zpracoval materiál, robot je ve stavu WAITING_FB a pokud již není na pásu FB další předmět, přechází robot do stavu GET_AB
 
c) stav GET_AB
- v tomto stavu dochází k uchopení materiálu a rotaci ramen robota o 90 stupňů, zároveň se kontroluje, zda byl lis plný 
- pokud ano, je stav lisu nastaven tak, aby byl prázdný a zároveň je nastaven signál udávající skutečnost, že je rameno B připraveno k odebrání materiálu z lisu
- následuje přechod do stavu PUT_AB

- v tomto stavu dochází k uchopení materiálu z pásu FB a rotaci ramen robota o 90 stupňů
- pokud je lis plný, přejde do stavu prázdný a zároveň je nastaven signál udávající skutečnost, že je rameno B připraveno, materiál může být umístěn na pás DB
- následuje přechod do stavu PUT_AB

d) stav PUT_AB
- pokud je rameno B připraveno, tak dochází k odebrání materiálu z lisu a jeho odložení na odkládací pás DB 
- dochází k rotaci ramen zpět do pozice, kdy je rameno A nad koncem pásu FB 
- poté robot přechází do stavu WAITING_FB

4) Lis
a) stav EMPTY 
- jedná se o počáteční stav lisu 
- signalizuje, že je lis prázdný a nezpracovává aktuálně žádný materiál 

b) stav WORKING
- lis zpracovává materiál 
- zpracování materiálu je modelováno pomocí zpoždění 
- po uplynutí potřebné doby přechází lis do stavu FULL
- vyprázdnění lisu (nastavení jeho stavu na prázdný - EMPTY) se děje ve vláknu pro obsluhu robota, konkrétně ve stavu GET_AB 

c) FULL
- materiál byl zpracován, lis je plný 

- pokud nebyl na pás FB vložen žádný materiál po dobu rovnu nebo větší než 5000 ms, robot přejde do klidového stavu NO_ACTION  
 
5) LCD displej 
Toto vlákno zajišťuje správné vykreslení grafických částí systému na LCD displej FITkitu. 
V počátečním stavu je na LCD displej vypsán následující text (viz obrázek níže).

.. figure :: pocatecni_stav.png
	:align: center

Jednotlivé grafické prvky jsou popsány níže na obrázku.

.. figure :: provoz.png
	:align: center
	
- zásobní pás (FB)   - znak "=" - volný pás, vyplněný obdélník - předmět
- odkládací pás (DB) - znak "=" - volný pás, vyplněný obdélník - předmět
- lis - reprezentován čtveřicí nevyplněných obdélníků, pokud je lis zaplněn, obdélníky jsou vyplněny
- ramena robota - reprezentována úsečkami 

Všechny informace o stavu aplikace jsou vypisovány na terminál (viz níže přiložený obrázek).
TODO: obrazek
.. figure :: terminal.png
	:align: center 

6) Terminál 
- vlákno pro obsluhu terminálu nebylo upravováno, bylo převzato z demo aplikace pro FreeRTOS

Kritické sekce 
===================
Pro správnou činnost ramen robota byly použity synchronizační postupy, které zajistí výlučné provádění daných akcí.
FreeRTOS nabízí funkce pro práci s kritickými sekcemi, konkrétně se jedná o funkce vTaskSuspendAll() a xTaskResumeAll().
Aby byl přesun ramen robota modelován správně, bylo zapotřebí volat funkci switchArms() v rámci kritické sekce. 

Spuštění aplikace 
===================
1) Přeložit aplikaci
2) Naprogramovat aplikaci a spustit terminál
3) Aplikace je připravena, materiál se na pás vkládá pomocí tlačítka "A"