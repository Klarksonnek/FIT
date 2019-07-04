#! /bin/sh

# Predmet: IPK (Pocitacove komunikace a site)
# Projekt: Prenos souboru
# Datum: duben 2016  
# Jmeno a prijmeni: Klara Necasova 
# Email: xnecas24@stud.fit.vutbr.cz

# spravne ukonceni programu
OK=0

# chybne ukonceni programu
ERR=1

# kontrola exit kodu
# $1 - navratovy kod
check() 
{
	#echo $1
	if [ "$1" = "$OK" ] ; then
		echo "OK"
		return $OK
	else
		echo "ERR"
		return $ERR
	fi
}

# Oddelovac vypisu 
SEPARATOR="======================================"

echo $SEPARATOR

# Kompilace projektu
echo "Kompilace projektu"
make

# Kopie zkompilovanych souboru do prislusnych slozek
cp client CLIENT/

echo $SEPARATOR

# Test argumentu - server
echo "TEST ARGUMENTU - SERVER"
echo $SEPARATOR
echo "Test argumentu (server): zadne argumenty"
./server
check $?

echo "Test argumentu (server): chybny prepinac"
./server -q 1011
check $?

echo "Test argumentu (server): prilis mnoho argumentu"
./server -p 1011 -x 
check $?

echo "Test argumentu (server): port nezadan"
./server -p
check $?

echo "Test argumentu (server): zaporne cislo portu"
./server -p -1011
check $?

echo "Test argumentu (server): prilis velke cislo portu"
./server -p 65590
check $?

# Test argumentu - klient
echo "TEST ARGUMENTU - KLIENT"
echo $SEPARATOR
echo "Test argumentu (client): zadne argumenty"
./client
check $?

echo "Test argumentu (client): chybny prepinac"
./client -h localhost -p 1011 -w soubor2.txt
check $?

echo "Test argumentu (client): prilis mnoho argumentu"
./client -h localhost -p 1011  -d soubor2.txt soubor3.txt   
check $?

echo "Test argumentu (client): zaporne cislo portu"
./client -h localhost -p -1011 -u soubor1.txt
check $?

echo "Test argumentu (client): prilis velke cislo portu"
./client -h localhost -p 65695 -u soubor1.txt
check $?

# ===============================
echo $SEPARATOR
echo "TEST KOMUNIKACE - klient a server v rozdilnych slozkach"

# Spusteni serveru na pozadi
#cd SERVER
./server -p 52796 &
SERVER_PID=$!
sleep 1

#cd ..
cd CLIENT

# Nahrani souboru soubor1.txt na server 
echo "Nahrani souboru soubor1.txt na server"
./client -h localhost -p 52796 -u soubor1.txt
check $?

# Stazeni souboru soubor2.txt ze serveru
echo "Stazeni souboru soubor2.txt ze serveru"
./client -h localhost -p 52796 -d soubor2.txt
check $?

# Pokus o nahrani neexistujiciho souboru na server
echo "Pokus o nahrani neexistujiciho souboru na server"
./client -h localhost -p 52796 -u nofile
check $?

# Pokus o stazeni neexistujiciho souboru ze serveru
echo "Pokus o stazeni neexistujiciho souboru ze serveru"
./client -h localhost -p 52796 -d nofile 
check $?

# Nahrani obrazku fit_new.gif na server 
echo "Nahrani souboru fit_new.gif na server"
./client -h localhost -p 52796 -u fit_new.gif
check $?

# Stazeni obrazku fit_old.gif ze serveru
echo "Stazeni souboru fit_old.gif ze serveru"
./client -h localhost -p 52796 -d fit_old.gif
check $?

# Nahrani souboru IMG/vut_new.gif na server
echo "Nahrani souboru IMG/vut_new.gif na server"
./client -h localhost -p 52796 -u IMG/vut_new.gif
check $?
 
# ===============================
echo $SEPARATOR
echo "TEST KOMUNIKACE - klient a server ve stejnych slozkach"
cd ..

# Spusteni klientu na pozadi 
# Nahrani souboru soubor1.txt na server 
echo "Nahrani souboru soubor1.txt na server"
./client -h localhost -p 52796 -u soubor1.txt
check $?

# Stazeni souboru soubor2.txt ze serveru
echo "Stazeni souboru soubor2.txt ze serveru"
./client -h localhost -p 52796 -d soubor2.txt
check $?

# Pokus o nahrani neexistujiciho souboru na server
echo "Pokus o nahrani neexistujiciho souboru na server"
./client -h localhost -p 52796 -u nofile  
check $?

# Pokus o stazeni neexistujiciho souboru ze serveru
echo "Pokus o stazeni neexistujiciho souboru ze serveru"
./client -h localhost -p 52796 -d nofile
check $?

# Nahrani obrazku fit_new.gif na server 
echo "Nahrani souboru fit_new.gif na server"
./client -h localhost -p 52796 -u fit_new.gif
check $?

# Stazeni obrazku fit_old.gif ze serveru
echo "Stazeni souboru fit_old.gif ze serveru"
./client -h localhost -p 52796 -d fit_old.gif
check $?

# Nahrani souboru IMG/vut_new.gif na server
echo "Nahrani souboru IMG/vut_new.gif na server"
./client -h localhost -p 52796 -u IMG/vut_new.gif
check $?

# Ukonceni serveru 
kill $SERVER_PID

echo $SEPARATOR
echo "Navraceni do puvodniho stavu"
# Odstraneni spustitelnych souboru
rm -f client server 
rm -f CLIENT/client

# Odstraneni souboru v testovacich slozkach 
rm -f CLIENT/soubor2.txt
rm -f CLIENT/fit_old.gif
rm -f vut_new.gif

echo "Konec testovani"

# Konec souboru test.sh

