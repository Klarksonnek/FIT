#!/bin/bash

#pouzitie: is_it_ok.sh xlogin00
#
#  - xlogin00 je login, ktory znaci nazov archivu napr xtisov00.zip
#  - script musi byt umiestneny v rovnakom adresari ako je archiv

LOGIN=$1

if [ -z ${LOGIN} ]; then
	echo "Musi byt zadany login"
	exit 1
fi

/bin/rm -rf ${LOGIN}
unzip ${LOGIN} -d ${LOGIN}

cd ${LOGIN}

wget --no-check-certificate https://www.fit.vutbr.cz/study/courses/BMS/public/proj2014/in1.csv
wget --no-check-certificate https://www.fit.vutbr.cz/study/courses/BMS/public/proj2014/in2.csv
wget --no-check-certificate https://www.fit.vutbr.cz/study/courses/BMS/public/proj2014/in3.csv
wget --no-check-certificate https://www.fit.vutbr.cz/study/courses/BMS/public/proj2014/bts.csv

make

echo "---------------------------------------"
echo "OUTPUT:"

./p1 in1.csv && cat out.txt
./p1 in2.csv && cat out.txt
./p1 in3.csv && cat out.txt
