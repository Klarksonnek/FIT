#!/bin/bash
# simplify-bkg
# Klara Necasova (xnecas24)
# Peter Tisovcik (xtisov00)

HS_BIN="./simplify-bkg"

# directory containing input files
DIR_IN="dir_in"

# directory containing output files
DIR_OUT="dir_out"

# directory containing reference output files
DIR_REF="dir_ref"

SHOW_OUTPUT=false

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

	#check if result is OK or ERROR
	if [ $ERR -eq 0 ] ; then
		OK=1
	fi

	#correct exit code is in green
	#incorrect exit code is in red
	if [ "$1" == "OK"  ] && [  $OK -eq 1 ] ; then
		echo -ne "\e[92m"
	elif  [ "$1" == "ERR"  ] && [  $OK -eq 0 ] ; then
		echo -ne "\e[92m"
	else
		echo -ne "\e[31m"
		ERROR=$((ERROR + 1)) #number of errors
	fi

	#print result
	if [ $ERR -eq 0 ] ; then
		echo "OK (${2})"
	else
		echo "ERR: ${ERR} (${2})"
	fi

	#reset of colour
	echo -n -e "\e[39m"
}

testArg() {
    echoTest "${1}"
    echo "run: " ${HS_BIN} ${2}
    ${HS_BIN} ${2}
    check "${3}" "run program with arguments"
    echo "$SEPARATOR"
}

testArgOutput() {
    echoTest "${1}"
    echo "run: " ${HS_BIN} ${2} "${DIR_IN}/${3}"
    ${HS_BIN} ${2} "${DIR_IN}/${3}" > "${DIR_OUT}/${3%%.*}${2}.out"
    check "${4}" "run program with arguments"

    if [ "${SHOW_OUTPUT}" == "true" ]; then
        cat "${DIR_OUT}/${3%%.*}${2}.out"
    fi

    #remove extension of filename
    if [ "${4}" == "OK"  ] ; then
        diff "${DIR_REF}/${3%%.*}${2}.out" "${DIR_OUT}/${3%%.*}${2}.out"
        check "OK" "compare input and reference files"
    fi

    echo "$SEPARATOR"
}

testApp() {
	testArgOutput "${1}" "-i" "${2}" "${3}"
	testArgOutput "${1}" "-1" "${2}" "${3}"
	testArgOutput "${1}" "-2" "${2}" "${3}"
}

echo "before testing, it is necessary to copy directories dir_in, dir_out, dir_ref"
echo "and compiled simplify-bkg application to the same folder as this script"

################################################
# Tests for argument check
# - test number 00-19
################################################

testArg "00 - invalid option \"-o\"" "-o" "ERR"
testArg "01 - duplicate option \"-i\" without file" "-i -i" "ERR"
testArg "02 - duplicate option \"-i\" with file" "-i -i filename" "ERR"
testArg "03 - duplicate option \"-1\" without file" "-1 -1" "ERR"
testArg "04 - duplicate option \"-1\" with file" "-1 -1 filename" "ERR"
testArg "05 - duplicate option \"-2\" without file" "-2 -2" "ERR"
testArg "06 - duplicate option \"-2\" with file" "-2 -2 filename" "ERR"
testArg "07 - duplicate invalid option \"-o\" without file" "-o -o" "ERR"
testArg "08 - duplicate invalid option \"-o\" with file " "-o -o filename" "ERR"
testArg "09 - too much options without file" "-i -1" "ERR"
testArg "10 - too much options with file" "-i -2 filename" "ERR"
testArg "11 - valid option, file tmp does not exist" "-i tmp" "ERR"
testArg "12 - valid option, file tmp does not exist" "-1 tmp" "ERR"
testArg "13 - valid option, file tmp does not exist" "-2 tmp" "ERR"

touch file
testArg "14 - valid option, duplicate existing file" "-i file file" "ERR"
rm file

################################################
# Tests for detection of invalid grammars
# - test number 20-59
################################################

testArgOutput "20 - empty file" "-i" "test20.in" "ERR"
testArgOutput "21 - invalid nonterminal \"-\"" "-i" "test21.in" "ERR"
testArgOutput "22 - undefined start symbol \"S\"" "-i" "test22.in" "ERR"
testArgOutput "23 - missing start symbol" "-i" "test23.in" "ERR"
testArgOutput "24 - redundant comma for separating of nonterminals" "-i" "test24.in" "ERR"
testArgOutput "25 - invalid start symbol \"S,\"" "-i" "test25.in" "ERR"
testArgOutput "26 - invalid delimiter of left and right side of rule \"B-->b\"" "-i" "test26.in" "ERR"
testArgOutput "27 - terminal \"b\" in set of nonterminals" "-i" "test27.in" "ERR"
testArgOutput "28 - nonterminal \"B\" in set of terminals" "-i" "test28.in" "ERR"
testArgOutput "29 - invalid start symbol \"a\"" "-i" "test29.in" "ERR"
testArgOutput "30 - invalid right side of rule: undefined terminal \"c\"" "-i" "test30.in" "ERR"
testArgOutput "31 - invalid left side of rule: terminal \"a\"" "-i" "test31.in" "ERR"
testArgOutput "32 - invalid left side of rule: undefined nonterminal \"C\"" "-i" "test32.in" "ERR"
testArgOutput "33 - missing left side of rule \"->b\"" "-i" "test33.in" "ERR"
testArgOutput "34 - missing right side of rule \"S->\"" "-i" "test34.in" "ERR"
testArgOutput "35 - pair of symbols \"AA\" in set of nonterminals" "-i" "test35.in" "ERR"
testArgOutput "36 - pair of symbols \"bb\" in set of terminals" "-i" "test36.in" "ERR"
testArgOutput "37 - pair of symbols \"Ss\" as start symbol" "-i" "test37.in" "ERR"
testArgOutput "38 - pair of symbols \"SS\" on left side of rule" "-i" "test38.in" "ERR"

################################################
# Tests for check of option -i, -1, -2
# - test number 60-99
################################################

testApp "60 - valid grammar" "test60.in" "OK"
testApp "61 - valid grammar" "test61.in" "OK"
testApp "62 - valid grammar" "test62.in" "OK"
testApp "63 - valid grammar" "test63.in" "OK"
testApp "64 - valid grammar" "test64.in" "OK"
testApp "65 - valid grammar" "test65.in" "OK"
testApp "66 - valid grammar" "test66.in" "OK"
testApp "67 - valid grammar" "test67.in" "OK"
testApp "68 - valid grammar" "test68.in" "OK"
testApp "69 - valid grammar" "test69.in" "OK"
testApp "70 - valid grammar" "test70.in" "OK"
testApp "71 - valid grammar" "test71.in" "OK"
testApp "72 - valid grammar" "test72.in" "OK"
testApp "73 - valid grammar" "test73.in" "OK"
testApp "74 - valid grammar" "test74.in" "OK"
