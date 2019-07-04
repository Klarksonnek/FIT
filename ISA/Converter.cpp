/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor: 	Converter.cpp
 * Autor: 	Klara Necasova
 * Email: 	xnecas24@stud.fit.vutbr.cz
 * Datum: 	listopad 2016
 */

#include "Converter.h"
/**
 * Prevede retezec do hexadecimalni podoby.
 * @param data Retezec.
 * @param len Delka retezce.
 * @return Vraci retezec v hexadecimalni podobe.
 */
string Converter::strToHexStr(unsigned char* data, int len)
{
    stringstream ss;
    ss << hex;
    int i = 0;
    ss << setfill('0') << setw(2) << (int)data[i] ;
    for(i = 1; i < len; ++i)
    {
        ss << ":" << setfill('0') << setw(2) << (int)data[i];
    }
    return ss.str();
}

/**
 * Prevede cislo typu uint16_t na retezec.
 * @param num Cislo typu uint16_t.
 * @return Vraci cislo prevedene na retezec.
 */
string Converter::uint16ToString(uint16_t num)
{
    stringstream ss;
    ss << num;
    return ss.str();
}

/**
 * Prevede IPv4 adresu na retezec.
 * @param ipv4Address IPv4 adresa.
 * @return Vraci IPv4 adresu prevedenou na retezec.
 */
string Converter::ipv4ToString(uint32_t ipv4Address)
{
    char buffer[IPv4_BUFF_LEN] = {0};
    inet_ntop(AF_INET, (const void *)&ipv4Address, buffer, IPv4_BUFF_LEN);
    	
    return buffer;
}

/**
 * Prevede IPv6 adresu na retezec.
 * @param file Soubor, ze ktereho se nacte IPv6 adresa.
 * @return Vraci IPv6 adresu prevedenou na retezec.
 */
string Converter::ipv6ToString(FILE* file)
{
    stringstream ss;
    unsigned char IPv6Addr[IPv6_BUFF_LEN] = {0};
    for(int i = 0; i < IPv6_BUFF_LEN; i++)
    {// nacitame po jednotlivych bytech 
		if(fread(&IPv6Addr[i], sizeof(unsigned char), 1, file) != 1)
		{
			return "";
		}
		// prevod do hexadecimalni podoby 
		ss << setfill('0') << setw(2) << hex << (uint32_t) (uint8_t)IPv6Addr[i];
		ss << (((i+1) % 2 == 0 && i != 0 && i != 15) ? ":" : "");
    }
    return ss.str();
}
