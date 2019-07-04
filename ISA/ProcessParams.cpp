/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor: 	ProcessParams.cpp
 * Autor: 	Klara Necasova
 * Email: 	xnecas24@stud.fit.vutbr.cz
 * Datum: 	listopad 2016
 */

#include "ProcessParams.h"

/**
 * Zpracuje parametry prikazove radky.
 * @param argc Pocet parametru prikazove radky.
 * @param argv Pole obsahujici argumenty prikazove radky v textove podobe.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int ProcessParams::GetParams(int argc, char **argv)
{
    if(argc < 8 || argc > 9)
    {// nevyhovujici pocet parametru
        return ECOUNT;
    }
	
    int arg;
    bool res;
    cDst = false;
    cSrc = false;
    cFileName = "";
    cFilterType = "";
    cFilterValue = "";
    
    while((arg = getopt(argc, argv, "i:f:v:sd")) != -1)
    {
        switch(arg)
		{
			// parametr i (nazev souboru)
            case 'i':
				if(cFileName != "")
				{
					return EDUPLICATE;
				}
				cFileName = optarg;
                break;
			// parametr f (typ filtru)	
            case 'f':
                if(strcmp(optarg, "mac") == 0 || strcmp(optarg, "ipv4") == 0 || strcmp(optarg, "ipv6") == 0 || strcmp(optarg, "tcp") == 0 || strcmp(optarg, "udp") == 0)
                {
					if(cFilterType != "")
					{
						return EDUPLICATE;
					}
                    cFilterType = optarg;
                    break;
                }
                else
                {
                    return EFILTER;
                }
			// parametr v (hodnota filtru)	
            case 'v':
				if(cFilterValue != "")
				{
					return EDUPLICATE;
				}
                cFilterValue = optarg;
				break;
			// parametr s (filtr se aplikuje na zdrojove adresy)
            case 's':
				if(cSrc)
				{
					return EDUPLICATE;
				}
                cSrc = true;
                break;
			// parametr d (filtr se aplikuje na cilove adresy)
            case 'd':
				if(cDst)
				{
					return EDUPLICATE;
				}
                cDst = true;
                break;
			default:
            	return EPARAMS;
        }   
    }
	
    if(!cSrc && !cDst)
    {// pokud neni zadan ani jeden z parametru -s nebo -d -> chyba 
        return EDIRECTION;
    }
    if(strcmp(cFilterValue.c_str(), "top10") == 0)
    {// osetreni filtru top10
		return EOK;
    }
   	else if(strcmp(cFilterType.c_str(), "mac") == 0)
	{// MAC
		// validace a normalizace MAC adresy
	    res = this->validateMACAddr(cFilterValue.c_str());
	    if(res == false)
	    {
			return EVALIDATE;
	    }
	    cFilterValue = normalizeMAC(cFilterValue);  
	}
	else if(strcmp(cFilterType.c_str(), "ipv4") == 0)
	{// IPv4
		// validace a normalizace IPv4 adresy
	    res = this->validateIPv4Addr(cFilterValue.c_str());
	    if(res == false)
	    {
			return EVALIDATE;
	    }
	    cFilterValue = normalizeIPv4(cFilterValue);
	}
	else if(strcmp(cFilterType.c_str(), "ipv6") == 0)
	{// IPv6
		// validace a normalizace IPv6 adresy
	    res = this->validateIPv6Addr(cFilterValue.c_str());
	    if(res == false)
	    {
			return EVALIDATE;
	    }
	    cFilterValue = normalizeIPv6(cFilterValue);
	}
	else if(strcmp(cFilterType.c_str(), "udp") == 0 || strcmp(cFilterType.c_str(), "tcp") == 0)
	{// TCP a UDP
	    char *ptr = NULL;
	    unsigned long port;
		// zisk portu 
	    port = strtoul(cFilterValue.c_str(), &ptr, 10);
	    if(*ptr != '\0')
	    {// overeni prevodu retezce na cislo
			return ECONVERTION;
	    }
	    if(port < 0 || port > 65535)
	    {// overeni rozsahu cisla portu
			return ERANGE;
	    }
	}
    return EOK;
}

/**
 * Vytiskne zpracovane parametry prikazove radky.
 */
void ProcessParams::print()
{
    cout << "cFileName: " << cFileName << endl;
    cout << "cFileType: " << cFilterType << endl;
    cout << "cFileValue: " << cFilterValue << endl;
    cout << "cSrc: " << cSrc << endl;
    cout << "cDst: " << cDst << endl;
}

/**
 * Validuje adresu IPv4.
 * @param addr IPv4 adresa.
 * @return Vraci true, pokud byla validace uspesna, jinak false. 
 */
bool ProcessParams::validateIPv4Addr(const char *addr)
{
    struct in_addr res;
    if(inet_pton(AF_INET, addr, &res) == 1)
    {
        return true;
    }
    return false;
}

/**
 * Validuje adresu IPv6.
 * @param addr IPv6 adresa.
 * @return Vraci true, pokud byla validace uspesna, jinak false. 
 */
bool ProcessParams::validateIPv6Addr(const char *addr)
{
    struct in6_addr res;
    if(inet_pton(AF_INET6, addr, &res) == 1)
    {
        return true;
    }
    return false;
}

/**
 * Validuje MAC adresu.
 * @param addr MAC adresa.
 * @return Vraci true, pokud byla validace uspesna, jinak false. 
 */
bool ProcessParams::validateMACAddr(const char *addr)
{
    string MACAddr(addr);
    if(MACAddr.length() != MAC_LENGTH)
    {
        return false;
    }
    for(size_t i = 0; i < MAC_LENGTH; i++)
    {
        if((i % MAC_SEPARATOR_POS != MAC_BYTE_LENGTH && !isxdigit(addr[i]))
        || (i % MAC_SEPARATOR_POS == MAC_LENGTH && addr[i] != MAC_SEPARATOR))
		{
			return false;
		}
    }
    return true;
}

/**
 * Normalizuje adresu IPv4.
 * @param addrIPv4 IPv4 adresa.
 * @return Vraci normalizovanou adresu. 
 */
string ProcessParams::normalizeIPv4(string &addrIPv4)
{
	struct in_addr addr; 
	inet_aton(addrIPv4.c_str(), &addr);
	return string(inet_ntoa(addr));
}

/**
 * Normalizuje adresu IPv6.
 * @param addrIPv6 IPv6 adresa.
 * @return Vraci normalizovanou adresu. 
 */
string ProcessParams::normalizeIPv6(string &addrIPv6)
{
	struct sockaddr_in6 sa;
	char str[INET6_ADDRSTRLEN] = {0};;

	inet_pton(AF_INET6, addrIPv6.c_str(), &(sa.sin6_addr));
	inet_ntop(AF_INET6, &(sa.sin6_addr), str, INET6_ADDRSTRLEN);
	return string(str);
}

/**
 * Normalizuje adresu MAC.
 * @param addrMAC MAC adresa.
 * @return Vraci normalizovanou adresu. 
 */
string ProcessParams::normalizeMAC(string addrMAC)
{
	for(int i = 0; i < addrMAC.length(); i++)
	{
		addrMAC[i] = tolower(addrMAC[i]);
	}
	return addrMAC;
}
