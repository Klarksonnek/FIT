/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor: 	Converter.h
 * Autor: 	Klara Necasova
 * Email: 	xnecas24@stud.fit.vutbr.cz
 * Datum: 	listopad 2016
 */

#ifndef CONVERTER_H  
#define CONVERTER_H

#include <sstream>
#include <iomanip>
#include "global.h"

#define IPv4_BUFF_LEN			17
#define IPv6_BUFF_LEN			16

/**
 * Trida Converter zajistujici prevod mezi formaty.
 */
class Converter
{
	public:
		string strToHexStr(unsigned char* data, int len);
		string uint16ToString(uint16_t num);
		string ipv4ToString(uint32_t ipv4Address);
		string ipv6ToString(FILE* file);
};

#endif
