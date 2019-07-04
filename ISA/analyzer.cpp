/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor: 	analyzer.cpp 
 * Autor: 	Klara Necasova
 * Email: 	xnecas24@stud.fit.vutbr.cz
 * Datum: 	listopad 2016
 */

#include "global.h"
#include "ProcessParams.h"
#include "PcapReader.h"

/**
 * Chybova hlaseni odpovidajici chybovym kodum. 
 */
const char *ERRMSG[] =
{
    "Vse v poradku.\n",
    "Chybne parametry prikazoveho radku!\n",
	"Chybny pocet parametru!\n",
	"Zadan duplicitni parametr!\n",
	"Neni zadana pozice v komunikaci!\n",
	"Chyba pri validaci adresy!\n",
	"Chyba pri prevodu portu na cislo!\n",
	"Port mimo rozsah!\n",
	"Typ filtru neni podporovan!\n",
    "Chyba pri otevirani souboru!\n",
    "Chyba pri cteni ze souboru!\n"
};

/**
 * Napoveda.
 */
const char *HELPMSG =
  "Program: Analyzator sitoveho provozu\n"
  "Autor: Klara Necasova (xnecas24)\n"
  "Datum: Listopad 2016\n"
  "Pouziti: ./analyzer [-i soubor] [ -f typFiltru ] [ -v hodnotaFiltru ] [ -s ] [ -d ]\n"
  "Popis parametru:\n"
  "  -i soubor (povinny parametr) vstupni soubor ve formatu libpcap\n"
  "  -f typFiltru (povinny parametr) urcuje, dle ktere polozky se pocita objem dat\n"
  "     Pripustne hodnoty jsou: mac, ipv4, ipv6, tcp, udp\n"
  "  -v hodnotaFiltru (povinny parametr) udava hodnotu filtru, hodnotou filtru muze byt i top10\n"
  "  -s filtr se aplikuje na zdrojove adresy \n"
  "  -d filtr se aplikuje na cilove adresy \n"
  "  Poznamka: minimalne jeden z parametru -s ci -d musi byt zadan\n";

/**
 * Vytiskne chybove hlaseni.
 * @param errCode Chybovy kod.
 */
void printError(int errCode)
{
    cerr << ERRMSG[errCode] << endl;
}

/**
 * Vypise napovedu.
 */
void printHelp()
{
  cout << HELPMSG;
}

/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
	ProcessParams param;
	PcapParser reader;
	FILE *file;
	int result = 0;

	if((result = param.GetParams(argc, argv)) != EOK)
	{// zpracovani parametru prikazove radky
		printError(result);
		printHelp();
		return -1;
	}

	// otevreni souboru pro cteni 
	file = fopen(param.cFileName.c_str(), "rb");
	if(file == NULL)
	{ 
		printError(EOPEN);
		return -1;
	}

	// cteni PCAP souboru 
	if(reader.readPcapFile(&param, file) != EOK)
	{
		printError(EREAD);
		fclose(file);
		return -1;	
	}
	
	// tisk statistik 
	reader.printStatistics(&param);
	// zavreni souboru 
	fclose(file);
	
	return 0;
}

