#!/bin/bash

BMS2A="./bms2A"

INPUT_DIR="input"
REF_OUTPUT="ref_output"

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

test() {
    echoTest "${1}"
    $BMS2A "${2}"
    check "${3}"
    echo "$SEPARATOR"
}

testWAW() {
    echoTest "${1}"
    $BMS2A "${INPUT_DIR}/${2}"
    check "${3}"
    diff "${INPUT_DIR}/${2}.waw" "${REF_OUTPUT}/${2}.waw"
    check "OK"
    echo "$SEPARATOR"
}

echo -ne "\e[31m"
echo "Testy do predmetu BMS - projekt 2"
echo "Testy su urcene pre nasledujucu modulaciu s 15 vzorkami na udaj: "
echo -e "\t00 - 3pi/4"
echo -e "\t01 - 1pi/4"
echo -e "\t10 - 5pi/4"
echo -e "\t11 - 7pi/4"
echo ""
echo "Vstup modulacie sa nachadza v adresari: '${INPUT_DIR}'"
echo "Referencne vystupy sa nachadzaju v adresari: '${REF_OUTPUT}'"
echo "Vygenerovany vystup sa nachadza v adresari: '${INPUT_DIR}'"
echo -e "\n\n"
echo -n -e "\e[39m"

################################################
test "00 - chybajuci argument" "" "ERR" 1
test "01 - privela argumentov" "jeden dva tri" "ERR" 1
test "02 - neexistujuci subor ako argument" "" "ERR" 1
test "03 - nevalidny vstupny subor - subor obsahuje okrem 0,1 aj ine znaky" "Makefile" "ERR" 1

################################################

testWAW "10 - zakladna modulacia" "test10.txt" "OK" 0

