#! /usr/bin/env python3
# -*- coding: utf-8 -*-
#CSV:xnecas24

# knihovny
import sys
import os
import argparse
import string
import re
import io
# osetreni kolize nazvu modulu CSV s nazvem skriptu
if os.path.abspath(sys.argv[0])[0:-7] in sys.path:
	sys.path.remove(os.path.abspath(sys.argv[0])[0:-7])
import csv

# Vypise chybovou hlasku a ukonci skript s danou navratovou hodnotou.
# @params msg Chybove hlaseni.
# @params err Chybovy kod.
def printECode(msg, err):
	sys.stderr.write(msg)
	sys.exit(err)

# Redefinice metody error() pro zajisteni spravneho navratoveho kodu.
class customArgumentParser(argparse.ArgumentParser):
	def error(self, _):
		sys.stderr.write("Neplatny prepinac.\n")
		sys.exit(1)

# Vypise text napovedy.
def printHelpMsg():
	sys.stdout.write(
	"--help                Vypise na standardni vystup napovedu skriptu a skonci.\n"+
	"--input=filename      Vstupni CSV soubor.\n"+
	"--output=filename     Textovy vystupni XML soubor.\n"+
	"-n                    Nebude se generovat XML hlavicka na vystup skriptu.\n"+
	"-r=root-element       Jmeno paroveho korenoveho elementu obalujici vysledek.\n"+
	"-s=separator          Nastaveni separatoru bunek na kazdem radku vstupniho CSV souboru.\n"+
	"-h=subst              Prvni radek CSV souboru slouzi jako hlavicka, podle nej se odvozuji jmena elementu XML.\n"+
	"-c=column-element     Prefix jmena elementu column-elementX, ktery bude obalovat nepojmenovane bunky, X je inkrementalni citac od 1.\n"
	"-l=line-element       Jmeno elementu, ktery obaluje zvlast kazdy radek vstupniho CSV souboru.\n"+
	"-i                    Zajisti vlozeni atributu index s ciselnou hodnotou do elementu line-element, tento parametr\n"+
	"                      se musi kombinovat s parametrem -l\n"+
	"--start=n             Inicializace inkrementalniho citace pro parametr -i na zadane kladne cele cislo vcetne nuly, implicitne n=1\n"+
	"-e, --error-recovery  Zotaveni z chybneho poctu sloupcu na neprvnim radku, kazdy chybejici sloupec bude doplnen prazdnym polem\n"
	"                      prebyvajici sloupce budou ignorovany.\n" + 
	"--missing-field=val   Mozno pouzit pouze v kombinaci s --error-recovery, resp. -e, pokud nejaka vstupni bunka chybi, bude zde doplnena\n"+
	"                      hodnota val.\n"+
	"--all-columns         Mozno pouzit pouze v kombinaci s --error-recovery, resp. -e, sloupce, ktere jsou v nekorektnim CSV souboru navic, nejsou\n"+
	"                      ignorovany, ale jsou take vlozeny do vysledneho XML souboru.\n"+
	"                      V kombinaci s parametrem -h - sloupce, ke kterym nebyla 1. radkem definovna hlavicka se budou\n"+
	"                      znacit column-elementX, X je poradi sloupce na danam radku.\n")

# Overi, zda nevznikl nevalidni XML element.
# @params elem XML element.
def validateXMLElement(elem):
	if re.match('^[X|x][M|m][L|l]', elem):
		return -1
	for i, char in enumerate(elem):
		if(i == 0): 
			# odpovida NameStartChar
			pattern = re.compile('[a-zA-Z\_\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u02FF\u0370-\u037D\u037F-\u1FFF\u200C-\u200D\u2070-\u218F\u2C00-\u2FEF\u3001-\uD7FF\uF900-\uFDCF\uFDF0-\uFFFD]')
			if not re.match(pattern, char):
				return -1
		elif(i > 0):
			# odpovida NameChar 
			pattern = re.compile('[a-zA-Z\_\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u02FF\u0370-\u037D\u037F-\u1FFF\u200C-\u200D\u2070-\u218F\u2C00-\u2FEF\u3001-\uD7FF\uF900-\uFDCF\uFDF0-\uFFFD\-\.0-9\u00B7\u0300-\u036F\u203F-\u2040]')
			if not re.match(pattern, char):
				return -1
	return 0 

# Konvertuje problematicke znaky v obsahovych bunkach CSV souboru.
# @params elem XML element.
# @return Vraci konvertovany element.
def convertChars(elem):
	elem = elem.replace("&", "&amp;")
	elem = elem.replace("<", "&lt;")
	elem = elem.replace(">", "&gt;")
	elem = elem.replace('\"', "&quot;")
	elem = elem.replace("\'", "&apos;")
	return elem

# Generuje odsazeni.
# @params spaceCnt Pocet mezer.
# @return Vraci retezec mezer.
def generateIndent(spaceCnt):
	indent = ' ' * spaceCnt
	return indent

# Zpracuje argumenty prikazove radky.
def getParams():
	params = customArgumentParser(add_help = False)
	# definice pripustnych parametru
	params.add_argument('--help', action = 'store_true', default = False, dest = 'help')
	params.add_argument('--input', action = 'store', dest = 'inputFile')
	params.add_argument('--output', action = 'store', dest = 'outputFile')
	params.add_argument('-n', action = 'store_true', default = False, dest = 'nHeader')
	params.add_argument('-r', action = 'store', dest = 'rootElem')
	params.add_argument('-s', action = 'store', dest = 'separ')
	params.add_argument('-h', action = 'store', dest = 'subst', nargs = '?', const='-')
	params.add_argument('-c', action = 'store', dest = 'colElem')
	params.add_argument('-l', action = 'store', dest = 'lineElem')
	params.add_argument('-i', action = 'store_true', dest = 'index', default = False)
	params.add_argument('--start', action = 'store', type = int, dest = 'startN')
	params.add_argument('-e', '--error-recovery', action = 'store_true', default = False, dest = 'errRec')
	params.add_argument('--missing-field', action = 'store', dest = 'missingVal')
	params.add_argument('--all-columns', action = 'store_true', dest = 'allCols', default=False)
        
	# parsovani argumentu
	result = params.parse_args()
   
	# --help
	if result.help == True:
		if len(sys.argv) == 2:
			printHelpMsg()
			sys.exit(0)
		else:
			printECode("Chybne parametry prikazove radky: --help nemuze byt zadano s vice prepinaci!\n", 1)
	
	# osetreni parametru -i
	if result.index == True and result.lineElem == None:
		printECode("Chybne parametry prikazove radky: -i musi byt zadano s parametrem -l!\n", 1)
	
	# osetreni parametru --start
	if result.startN != None and (result.index == False or result.lineElem == None):
		printECode("Chybne parametry prikazove radky: --start musi byt zadano s parametrem -i a -l!\n", 1)
		
	# osetreni parametru --missing-field
	if result.missingVal != None and result.errRec == False:
		printECode("Chybne parametry prikazove radky: --missing-field musi byt zadano s parametrem --error-recovery (resp. -e)!\n", 1)
	
	# nastaveni implicitni hodnoty parametru --missing-field
	if result.errRec == True and result.missingVal == None:
		result.missingVal = ''
	
	# osetreni parametru --all-columns
	if result.allCols == True and result.errRec == False:
		printECode("Chybne parametry prikazove radky: --all-columns musi byt zadano s parametrem --error-recovery (resp. -e)!\n", 1)
	
	# TODO: Overeni, zda nebyl zadan jeden prepinac vicekrat
	
	if result.rootElem != None:
		if validateXMLElement(result.rootElem) == -1:
			printECode("Nevalidni XML znacka!\n", 30)
		
	# -s
	if result.separ == None:
		result.separ = ','
	elif result.separ == 'TAB':
		result.separ = '\t'
	elif len(result.separ) != 1:
		printECode("Nepovolena hodnota oddelovace CSV bunek!\n", 1)

	# -c
	if result.colElem == None:
		result.colElem = 'col'
	else:
		if validateXMLElement(result.colElem) == -1:
			printECode("Nevalidni XML znacka!\n", 30)
	# -l
	if result.lineElem == None:
		result.lineElem = 'row';
	else:
		if validateXMLElement(result.lineElem) == -1:
			printECode("Nevalidni XML znacka!\n", 30)
	# -i
	if result.startN == None:
		result.startN = 1;
	elif result.startN < 0:
		printECode('Nepovolena hodnota oddelovace CSV bunek!\n', 1)
	
	return result

# Prevede CSV soubor do XML formatu.
# @params params Parametry prikazoveho radku.
# @params headerList Prvni radek CSV souboru (hlavicka).
# @params body Telo CSV souboru.
# @params colCnt Pocet sloupcu CSV souboru.
def CSVToXML(params, headerList, body, colCnt):
	xmlOutput = ""
	spaceCnt = 0
	# generovani XML hlavicky
	if params.nHeader == False:
		xmlOutput += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

	# generovani oteviraciho korenoveho elementu
	if params.rootElem != None:
		xmlOutput += '<{:s}>\n'.format(params.rootElem)
		spaceCnt += 4

	# parametr -h nezadan - generovani nazvu sloupcu
	if params.subst == None:
		headerList = []
		for x in range(1, colCnt + 1):
			headerList.append('{:s}{:d}'.format(params.colElem, x))

	if colCnt == 0:
		x = 1
		# generovani atributu index
		if params.index == True:
			indexStr = ' index="{:d}"'.format(params.startN)
			params.startN += 1;
		else:
			indexStr = ''
		xmlOutput += '{:s}<{:s}{:s}>\n'.format(generateIndent(spaceCnt), params.lineElem, indexStr)
		spaceCnt += 4
		xmlOutput += '{:s}<{:s}{:d}>\n'.format(generateIndent(spaceCnt), params.colElem, x)
		xmlOutput += '{:s}</{:s}{:d}>\n'.format(generateIndent(spaceCnt), params.colElem, x)
		spaceCnt -= 4
		xmlOutput += '{:s}<{:s}{:s}>\n'.format(generateIndent(spaceCnt), params.lineElem, indexStr)

	# pruchod pres jednotlive radky CSV souboru
	for row in body:
		# aktualni pocet sloupcu na danem radku
		colCntAct = len(row)
		# overeni odpovidajiciho poctu sloupcu
		if colCntAct != colCnt and params.errRec == False:
			printECode("Vstup obsahuje spatny pocet sloupcu!\n", 32)
		# generovani atributu index
		if params.index == True:
			indexStr = ' index="{:d}"'.format(params.startN)
			params.startN += 1;
		else:
			indexStr = ''
		# generovani oteviraciho radkoveho elementu
		xmlOutput += '{:s}<{:s}{:s}>\n'.format(generateIndent(spaceCnt), params.lineElem, indexStr)
		spaceCnt += 4
		
		# pruchod pres jednotlive sloupce CSV souboru na danem radku
		for i, col in enumerate(row):
			# obvykle generovani bunek XML souboru
			if i < colCnt:
				xmlOutput += '{:s}<{:s}>\n'.format(generateIndent(spaceCnt), headerList[i])
				spaceCnt += 4
				xmlOutput += convertChars('{:s}{:s}\n'.format(generateIndent(spaceCnt), col))
				spaceCnt -= 4
				xmlOutput += '{:s}</{:s}>\n'.format(generateIndent(spaceCnt), headerList[i])
				# nedostatecny pocet bunek na danem radku
				if i == colCntAct - 1:
					# pocet bunek k doplneni 
					fieldsToAdd = colCnt - colCntAct
					for j in range(i + 1, fieldsToAdd + i + 1):
						xmlOutput += '{:s}<{:s}>\n'.format(generateIndent(spaceCnt), headerList[j])
						spaceCnt += 4
						xmlOutput += convertChars('{:s}{:s}\n'.format(generateIndent(spaceCnt), params.missingVal))
						spaceCnt -= 4
						xmlOutput += '{:s}</{:s}>\n'.format(generateIndent(spaceCnt), headerList[j])
			# pokud byl zadan prepinac --all-columns, jsou generovany i prebyvajici sloupce
			elif params.allCols:
				xmlOutput += '{:s}<{:s}{:d}>\n'.format(generateIndent(spaceCnt), params.colElem, i + 1)
				spaceCnt += 4
				xmlOutput += convertChars('{:s}{:s}\n'.format(generateIndent(spaceCnt), col))
				spaceCnt -= 4
				xmlOutput += '{:s}</{:s}{:d}>\n'.format(generateIndent(spaceCnt), params.colElem, i + 1)
		spaceCnt -= 4
		# generovani uzaviraciho radkoveho elementu
		xmlOutput += '{:s}</{:s}>\n'.format(generateIndent(spaceCnt), params.lineElem)
		
	# generovani uzaviraciho korenoveho elementu
	if params.rootElem != None: 
		xmlOutput += '</{:s}>\n'.format(params.rootElem)

	# zapis do souboru
	print(xmlOutput, end = '', file = params.outputFile)
	return

#-------------------------------------------------------------------------
# zpracovani parametru prikazove radky
result = getParams()

# overeni, zda nebyl nektery prepinac zadan vicekrat
argCheck = []
# pruchod pres argumenty prikazove radky
for s in sys.argv:
	# prepinac --error-recovery nahradime za -e
	if(s == '--error-recovery'):
		sReplaced = re.sub('--error-recovery', '-e', s)
	# odstranime hodnoty jednotlivych prepinacu za znakem '=' (vcetne znaku '=')
	else:
		sReplaced = re.sub('\=.*$','', s)
	# ziskane prepinace vlozime do seznamu 
	argCheck.append(sReplaced)

# pokud delka seznamu neodpovida kardinalite mnoziny vytvorene ze seznamu argCheck 
# (mnozina neobsahuje duplicitni prepinace)
# potom byl nektery z prepinacu zadan vicekrat -> chyba 
if len(argCheck) != len(set(argCheck)):
	printECode("Chybne parametry prikazove radky: nektery prepinac zadan vicekrat!\n", 1)

	
# otevreni vstupniho souboru pro cteni
if result.inputFile != None:
	try:
		if os.stat(result.inputFile).st_size < 0:
			sys.exit(0);
	except OSError:
		printECode("Neexistujici zadany soubor nebo chyba otevreni souboru pro cteni.\n", 2)
	try:
		result.inputFile = open(result.inputFile, 'r', newline = "", encoding = 'utf-8')
	except IOError:
		printECode("Neexistujici zadany soubor nebo chyba otevreni souboru pro cteni.\n", 2)
else:
	result.inputFile = io.TextIOWrapper(sys.stdin.buffer, encoding='utf-8')
	
# otevreni vystupniho souboru pro zapis
if result.outputFile != None:
	try:
		result.outputFile = open(result.outputFile, 'w', newline = "", encoding = 'utf-8')
	except IOError:
		printECode("Chyba pri pokusu o otevreni noveho nebo existujiciho souboru pro zapis.\n", 3)
else:
	result.outputFile = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')

headerList = ""

# zpracovani hlavicky CSV souboru
header = csv.reader(result.inputFile, delimiter = result.separ)

# nacteni prvniho radku CSV souboru
for i, headerList in enumerate(header):
	if i == 0:
		break;

# prepinac -h
if result.subst != None:
	# nahrazeni nepovolenych znaku hlavicky
	for i, word in enumerate(headerList):
		word = re.sub('[ \t\n\r\f\v,;|^~\u0000-\u0008\u000b\u000c\u000e-\u001f\u007f-\u0084\u0086-\u009f\ud800-\udfff\ufdd0-\ufddf]', result.subst, word)
		# kontrola validity XML elementu
		if validateXMLElement(word) == -1:
			printECode("Nevalidni XML znacka!\n", 31)
		headerList[i] = word

else:
	result.inputFile.seek(0)

# zjisteni poctu sloupcu
colCnt = len(headerList)

if colCnt == 0 and result.subst != None:
	printECode("Nevalidni XML element!\n", 31);

# nacteni tela CSV souboru
body = csv.reader(result.inputFile, delimiter = result.separ)

# prevod CSV souboru do formatu XML
CSVToXML(result, headerList, body, colCnt)



