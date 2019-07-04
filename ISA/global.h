/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor:  global.h 
 * Autor: 	Klara Necasova
 * Email: 	xnecas24@stud.fit.vutbr.cz
 * Datum: 	listopad 2016
 */

#ifndef GLOBAL_H  
#define GLOBAL_H

#include <iostream>
#include <fstream>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

/**
 * Chybove kody.
 */
enum ecodes
{
    EOK,
    EPARAMS,
	ECOUNT,
	EDUPLICATE,
	EDIRECTION,
	EVALIDATE,
	ECONVERTION,
	ERANGE,
	EFILTER,
    EOPEN,
    EREAD
};

/**
 * Chybova hlaseni odpovidajici chybovym kodum.
 */
extern const char *ERRMSG[];

/**
 * Vytiskne chybove hlaseni.
 * @param errCode Chybovy kod.
 */
void printError(int errCode);

#endif
