/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor: 	ProcessParams.h
 * Autor: 	Klara Necasova
 * Email: 	xnecas24@stud.fit.vutbr.cz
 * Datum: 	listopad 2016
 */

#ifndef PROCESSPARAMS_H  
#define PROCESSPARAMS_H

#include "global.h"
#include <cstdlib>
#include <iomanip>
#include <sstream>

// pro validaci MAC adresy
#define MAC_LENGTH          17
#define MAC_BYTE_LENGTH     2
#define MAC_SEPARATOR_POS   3
#define MAC_SEPARATOR       'c'

/**
 * Struktura reprezentujici zaznam statistiky.
 */
typedef struct
{
    string addr; // adresa
    int value_1; // value_1 Hodnota vyjadrujici soucet bajtu od vrstvy L2.
    int value_2; // value_2 Hodnota vyjadrujici soucet bajtu od konce hlavicky dane vrstvy, hodnota zavisi na zvolenem filtru.
}statisticsValues;

/**
 * Trida pro zpracovani parametru prikazove radky.
 */
class ProcessParams
{
    public:
		void printHelp();
		int GetParams(int argc, char **argv);
		void print();
        bool validateMACAddr(const char *addr);
        bool validateIPv4Addr(const char *addr);
        bool validateIPv6Addr(const char *addr);
		string normalizeIPv4(string &addrIPv4);
		string normalizeIPv6(string &addrIPv6);
		string normalizeMAC(string addrMAC);	
    public:
		string cFileName;
    	string cFilterType;
    	string cFilterValue;
		bool cSrc;
		bool cDst;
		vector<statisticsValues> statistics;
};

#endif 
