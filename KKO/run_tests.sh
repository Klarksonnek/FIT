#!/bin/bash

BIN="./gif2bmp"

DIR_GIF="dir_gif"
DIR_BMP="dir_bmp"
DIR_REF="dir_ref"
DIR_LOG="dir_log"

ERROR=0
ERR_CODE=0
SEPARATOR="-------------------------------------------"

LOGIN=$1

if [ -z ${LOGIN} ]; then
	echo "Musi byt zadany login"
	exit 1
fi

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
		echo "OK (${2})"
	else
		echo "ERR: ${ERR} (${2})"
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

testGIFtoBMP() {
    echoTest "${1}"
    echo "launching:" ${BIN} -l "${DIR_LOG}/${2%%.*}.out" -i "${DIR_GIF}/${2}" -o "${DIR_BMP}/${2%%.*}.bmp"
    ${BIN} -l "${DIR_LOG}/${2%%.*}.out"  -i "${DIR_GIF}/${2}" -o "${DIR_BMP}/${2%%.*}.bmp"
    check "${3}" "spustenie programu s argumentami"

    #remove extension from filename
    diff "${DIR_BMP}/${2%%.*}.bmp" "${DIR_REF}/${2%%.*}.bmp"
    check "OK" "diff s referencnymi bmp subormi"

    GIF_SIZE=`stat -c %s ${DIR_GIF}/${2}`
    BMP_SIZE=`stat -c %s ${DIR_REF}/${2%%.*}.bmp`

    FILE="${DIR_LOG}/${2%%.*}.out_ref"
    /bin/rm -rf ${FILE}

    echo "login = ${LOGIN}" >> ${FILE}
    echo "uncodedSize = ${BMP_SIZE}" >> ${FILE}
    echo "codedSize = ${GIF_SIZE}" >> ${FILE}
    echo "" >> ${FILE}

    diff "${DIR_LOG}/${2%%.*}.out" "${DIR_LOG}/${2%%.*}.out_ref"
    check "OK" "diff s vypocitanymi a vystupnymi velkostami obrazkov"

    echo "$SEPARATOR"
}

################################################
#testy zakladnych obrazkov

testGIFtoBMP "01 - jednoduchy obrazok" "01.gif" "OK"
testGIFtoBMP "02 - jednoduchy obrazok" "02.gif" "OK"
testGIFtoBMP "03 - jednoduchy obrazok" "03.gif" "OK"
testGIFtoBMP "04 - jednoduchy obrazok" "04.gif" "OK"
testGIFtoBMP "05 - jednoduchy obrazok" "05.gif" "OK"
testGIFtoBMP "06 - jednoduchy obrazok" "06.gif" "OK"
testGIFtoBMP "07 - jednoduchy obrazok" "07.gif" "OK"
testGIFtoBMP "08 - obrazok s priehladnym pozadim" "08.gif" "OK"
testGIFtoBMP "09 - obrazok s priehladnym pozadim" "09.gif" "OK"
testGIFtoBMP "10 - obrazok, ktory sa sklada z niekolkych subblokov" "10.gif" "OK"
testGIFtoBMP "11 - obrazok s lokalnou tabulkou farieb" "11.gif" "OK"
testGIFtoBMP "12 - obrazok s interlace" "12.gif" "OK"
testGIFtoBMP "13 - obrazok s interlace" "13.gif" "OK"
testGIFtoBMP "14 - obrazok s interlace" "14.gif" "OK"

################################################
#testy dalsich obrazkov z netu

testGIFtoBMP "20 - jednoduchy obrazok" "20.gif" "OK"
testGIFtoBMP "21 - jednoduchy obrazok" "21.gif" "OK"
testGIFtoBMP "22 - jednoduchy obrazok" "22.gif" "OK"
testGIFtoBMP "23 - jednoduchy obrazok" "23.gif" "OK"
testGIFtoBMP "24 - obrazok s priehladnym pozadim" "24.gif" "OK"
testGIFtoBMP "25 - animovany obrazok" "25.gif" "OK"
testGIFtoBMP "26 - animovany obrazok" "26.gif" "OK"
testGIFtoBMP "27 - obrazok s priehladnym pozadim" "27.gif" "OK"
testGIFtoBMP "28 - animovany obrazok" "28.gif" "OK"
testGIFtoBMP "29 - animovany obrazok" "29.gif" "OK"
testGIFtoBMP "30 - animovany obrazok" "30.gif" "OK"

testGIFtoBMP "31 - animovany obrazok" "31.gif" "OK"
testGIFtoBMP "32 - animovany obrazok" "32.gif" "OK"
testGIFtoBMP "33 - animovany obrazok" "33.gif" "OK"
testGIFtoBMP "34 - obrazok s interlace" "34.gif" "OK"
testGIFtoBMP "35 - obrazok s interlace" "35.gif" "OK"
testGIFtoBMP "36 - obrazok s interlace" "36.gif" "OK"
testGIFtoBMP "37 - obrazok s interlace" "37.gif" "OK"
testGIFtoBMP "38 - obrazok s interlace" "38.gif" "OK"
testGIFtoBMP "39 - obrazok s interlace" "39.gif" "OK"
testGIFtoBMP "40 - obrazok s interlace" "40.gif" "OK"

testGIFtoBMP "41 - obrazok s textom v obrazku" "41.gif" "OK"
testGIFtoBMP "42 - animovany obrazok" "42.gif" "OK"
testGIFtoBMP "43 - obrazok s kratkym textom" "43.gif" "OK"
testGIFtoBMP "44 - obrazok s dlhym textom" "44.gif" "OK"
testGIFtoBMP "45 - obrazok s kratkym textom a prekladanim" "45.gif" "OK"
