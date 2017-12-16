#!/bin/bash

BMS2A="./bms2A"
BMS2B="./bms2B"

DIR_TXT="dir_txt"
WAW_DIR="dir_wav"

ERROR=0
ERR_CODE=0
SEPARATOR="-------------------------------------------"

echoTest() {
	echo -n -e "\e[36m"
	echo "$1"
	echo -n -e "\e[39m"
}

check () {
	ERR=$?
	ERR_CODE=$ERR
	OK=0

	#zistenie ci je vysledok programu Ok/ERR
	if [ $ERR -eq 0 ] ; then
		OK=1
	fi

	#zelenou sa potvrdi spravny exit code
	#cercenou je chybny navratovy kod
	if [ "$1" == "OK"  ] && [  $OK -eq 1 ] ; then
		echo -ne "\e[92m"
	elif  [ "$1" == "ERR"  ] && [  $OK -eq 0 ] ; then
		echo -ne "\e[92m"
	else
		echo -ne "\e[31m"
		ERROR=$((ERROR + 1)) #pocet chyb
	fi

	#vypis vysledku
	if [ $ERR -eq 0 ] ; then
		echo "OK"
	else
		echo "ERR: $ERR"
	fi

	#reset farby
	echo -n -e "\e[39m"
}

testArgModulation() {
    echoTest "${1}"
    $BMS2A "${2}"
    check "${3}"
    echo "$SEPARATOR"
}

testArgDemodulation() {
    echoTest "${1}"
    $BMS2B "${2}"
    check "${3}"
    echo "$SEPARATOR"
}

testModulation() {
    echoTest "${1}"
    $BMS2A "${DIR_TXT}/${2}"
    check "${3}"

    #remove extension from filename
    diff "${DIR_TXT}/${2%%.*}.wav" "${WAW_DIR}/${2%%.*}.wav"
    check "OK"
    echo "$SEPARATOR"
}

testDemodulation() {
    echoTest "${1}"
    $BMS2B "${WAW_DIR}/${2}"
    check "${3}"
    diff "${DIR_TXT}/${2%%.*}.txt" "${WAW_DIR}/${2%%.*}.txt"
    check ${4}
    echo "$SEPARATOR"
}

echo -ne "\e[31m"
echo "Testy do predmetu BMS - projekt 2"
echo "Testy su urcene pre nasledujucu modulaciu s 15 vzorkami na udaj: "
echo "Modulacia1:"
echo -e "\t00 - 3pi/4"
echo -e "\t01 - 1pi/4"
echo -e "\t10 - 5pi/4"
echo -e "\t11 - 7pi/4"
echo "Modulacia2:"
echo -e "\t00 - 5pi/4"
echo -e "\t01 - 7pi/4"
echo -e "\t10 - 3pi/4"
echo -e "\t11 - 1pi/4"
echo ""
echo "Vstup modulacie sa nachadza v adresari: '${DIR_TXT}' a jeho vystup v adresari: '${WAW_DIR}'"
echo "Vstupy demodulacie sa nachadzaju v adresari: '${WAW_DIR}' a eho vystupy v adresari: '${DIR_TXT}'"
echo ""
echo -n -e "\e[39m"

################################################
echoTest "Testy pre overenie argumentov bms2A"

testArgModulation "00 - chybajuci argument" "" "ERR"
testArgModulation "01 - privela argumentov" "jeden dva tri" "ERR"
testArgModulation "02 - neexistujuci subor ako argument" "" "ERR"
testArgModulation "03 - nevalidny vstupny subor - subor obsahuje okrem 0,1 aj ine znaky" "Makefile" "ERR"

touch /tmp/nieco.txtt
testArgModulation "05 - nevalidna koncovka txtt" "/tmp/nieco.txtt" "ERR"

touch /tmp/nieco.exe
testArgModulation "06 - nevalidna koncovka exe" "/tmp/nieco.exe" "ERR"

################################################
echoTest "Testy pre overenie argumentov bms2B"

testArgDemodulation "10 - chybajuci argument" "" "ERR"
testArgDemodulation "11 - privela argumentov" "jeden dva tri" "ERR"
testArgDemodulation "12 - neexistujuci subor ako argument" "" "ERR"
testArgDemodulation "13 - nevalidny vstupny subor - subor obsahuje okrem 0,1 aj ine znaky" "Makefile" "ERR"

touch /tmp/nieco.txtt
testArgDemodulation "15 - nevalidna koncovka txtt" "/tmp/nieco.txtt" "ERR"

touch /tmp/nieco.exe
testArgDemodulation "16 - nevalidna koncovka exe" "/tmp/nieco.exe" "ERR"
################################################
echoTest "Testy pre overenie modulacie"

testModulation "20 - Modulacia1, 24PCM: modulacia vsetkych 4 faz" "test20.txt" "OK"
testModulation "21 - Modulacia1, 24PCM: modulacia obsahujuca len synchronizaciu" "test21.txt" "OK"
testModulation "22 - ukazkovy kod zo zadania, pouziva 24PCM a 30 vzoriek na signalovu jednotku" "test22.txt" "OK"

################################################
echoTest "Testy pre demodulacie"

testDemodulation "30 - Modulacia1, 24PCM, 15 vzoriek na signalovu jednotku: demodulacia 4 faz" "test30.wav" "OK" "OK"
testDemodulation "31 - Modulacia1, 16PCM, 15 vzoriek na signalovu jednotku: demodulacia 4 faz" "test31.wav" "OK" "OK"
testDemodulation "32 - Modulacia1, 32PCM, 15 vzoriek na signalovu jednotku: demodulacia 4 faz" "test32.wav" "OK" "OK"

testDemodulation "33 - Modulacia2, 24PCM, 15 vzoriek na signalovu jednotku: demodulacia 4 faz" "test33.wav" "OK" "OK"
testDemodulation "34 - Modulacia1, 24PCM, 15 vzoriek na signalovu jednotku: chybajuca synchronizacia" "test34.wav" "ERR" "ERR"
testDemodulation "35 - Modulacia1, 24PCM, 25 vzoriek na signalovu jednotku: demodulacia 4 faz" "test35.wav" "OK" "OK"

testDemodulation "36 - Modulacia1, 24PCM, 15 vzoriek na signalovu jednotku: wav obsahuje len synchronizaciu" "test36.wav" "OK" "OK"
testDemodulation "37 - ukazkovy kod zo zadania" "test37.wav" "OK" "OK"
