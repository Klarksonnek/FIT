/**
 * Projekt: IPK - projekt 2: Prenos souboru
 * Modul: client.cpp 
 * Autor: Klara Necasova
 * Email: xnecas24@stud.fit.vutbr.cz
 * Datum: duben 2016
 */

#include "common.h"

#define DOWNLOAD 1
#define UPLOAD 2

/**
 * Struktura pro ulozeni informaci z prikazove radky.
 */
typedef struct
{
	// jmeno hosta
	string hostName;
	// cislo portu
	int port;
	// jmeno vystupniho souboru
	string fileName;
	// pozadovana akce: 1 (cteni - download), 2 (zapis - upload)
	int action;
}TParams;

/**
 * Zpracuje parametry prikazove radky.
 * @param argc Pocet parametru prikazove radky.
 * @param argv Pole obsahujici argumenty prikazove radky v textove podobe.
 * @param params Struktura pro ulozeni argumentu prikazove radky. 
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int GetParam(int argc, char **argv, TParams *params)
{
	char *pEnd = NULL;
	if(argc != 7)
	{
		return EPARAMS;
	}
	if(strcmp(argv[1], "-h") == 0 && strcmp(argv[3], "-p") == 0)
	{
		// hostname
		params->hostName = argv[2];
		if(params->hostName == "")
		{
			return EHOST_NAME;
		}
		// cislo portu 
		params->port = strtoul(argv[4], &pEnd, 10);
		if(*pEnd != '\0')
		{
			return EPARAMS;
		}
		if(params->port < 0 || params->port > 65535)
		{
			return EPORT;
		}
		if(strcmp(argv[5], "-d") == 0)
		{// akce - cteni 
			params->action = DOWNLOAD;
		}
		else if(strcmp(argv[5], "-u") == 0)
		{// akce - zapis 
			params->action = UPLOAD;
		}
		else
		{
			return EPARAMS;
		}
		// jmeno vystupniho souboru
		params->fileName = argv[6];
	}
	else
	{
		return EPARAMS;
	}
	return EOK;
}

/**
 * Zajistuje stazeni souboru ze serveru.
 * @param socketNum Cislo socketu.
 * @param fileName Jmeno souboru.
 * @return Vraci EOK nebo prislusny chybovy kod.
 */
int handleDownload(int socketNum, string fileName)
{
	int err;
	string response;

	// pozadavek pro stazeni daneho souboru
	string request = "<" + fileName + "?"; 

	if(send(socketNum, request.c_str(), request.length() + 1, 0) < 0)
	{// odeslani pozadavku na server
		return ESEND;
	}
	
	if((err = receiveProtocolData(socketNum, &response)) != EOK)
	{// prijem odpovedi od serveru
		return err; 
	}

	if(response.find("D:YES") != string::npos)
	{// potvrzeni pozadavku
		// delka souboru
		int fileSize = 0; 
		// zjisteni delky souboru, ktery se bude stahovat
		istringstream(response.substr(response.find('#') + 1)) >> fileSize;

		// pozadavek na zahajeni stahovani dat ze serveru
		request = "ACK";

		if(send(socketNum, request.c_str(), request.length() + 1, 0) < 0)
		{// odeslani pozadavku na server
			return ESEND;
		}

		if((err = receiveData(socketNum, fileName, fileSize)) != EOK)
		{// prijem dat od serveru
			return err;
		}
	}
	else
	{// chyba pri stahovani dat ze serveru
		return EDOWNLOAD;
	}
	return EOK;
}

/**
 * Zajistuje nahrani souboru na server.
 * @param socketNum Cislo socketu.
 * @param absolutePath Absolutni cesta k souboru.
 * @return Vraci EOK nebo prislusny chybovy kod.
 */
int handleUpload(int socketNum, string absolutePath)
{
	int fileSize = 0;
	int err = 0;
	string response;

	if(fileExists(absolutePath, "r", &fileSize))
	{// zjisteni velikosti souboru a overeni, zda k nemu lze pristoupit
		// zjisteni nazvu souboru
		string fileName = absolutePath.substr(absolutePath.find_last_of('/') + 1);

		// pozadavek pro nahrani daneho souboru
		string request = ">" + fileName + "?#" + intToString(fileSize);

		if(send(socketNum, request.c_str(), request.length() + 1, 0) < 0)
		{// odeslani pozadavku na server
			return ESEND;
		}

		if((err = receiveProtocolData(socketNum, &response)) != EOK)
		{// prijem odpovedi od serveru
			return err; 
		}

		if(response.compare("U:YES") == 0)
		{
			if((err = sendData(socketNum, absolutePath, fileSize)) != EOK)
			{// odeslani souboru na server
				return err;
			}
			response.clear();
			if((err = receiveProtocolData(socketNum, &response)) != EOK)
			{// prijem zpravy od serveru - potvrzeni spojeni
				return err;
			}
			
			if(response.compare("ACK") != 0)
			{// spojeni se serverem je potvrzeno
				return EUPLOAD;
			}
		}
		else
		{
			return EUPLOAD;
		}
		return EOK;
	}
	else
	{
		return EUPLOAD;
	}
}

/**
 * Vytvori spojeni.
 * @param socketInfo Informace o spojeni.
 * @param host Informace o hostovi.
 * @param socketNum Cislo portu.
 * @param params Parametry prikazove radky.
 * @return Vraci EOK  nebo prislusny chybovy kod.
 */
int createConnection(sockaddr_in socketInfo, hostent *host, int *socketNum, TParams *params)
{
    // preklad adresa -> IP
    host = gethostbyname(params->hostName.c_str());
    if(host == NULL)
    {
        return EHOST;
    }
    if((*(socketNum) = socket(PF_INET, SOCK_STREAM, 0)) <	0)
    {// vytvoreni socketu
        return ESOCKET;
    }
	// inicializace struktury
	bzero((char*)&socketInfo,sizeof(socketInfo));
    // rodina protokolu
    socketInfo.sin_family = PF_INET;
    // cislo portu
    socketInfo.sin_port = htons(params->port);
    // zkopirovani informaci o hostovi
    memcpy(&socketInfo.sin_addr, host->h_addr, host->h_length);
    if(connect(*(socketNum), (struct sockaddr *)&socketInfo, sizeof(socketInfo)) < 0)
    {// vytvoreni spojeni
        return ECONNECT;
    }
    return EOK;
}

/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
	// stuktura s parametry prikazove radky
	TParams params;
	// informace o socketu hosta
	sockaddr_in socketInfo;
	// host
	hostent *host = NULL;
	// cislo socketu
	int socketNum;
	// chybovy kod
	int err;

	if(GetParam(argc, argv, &params) != EOK)
	{// zpracovani parametru prikazove radky
		printError(EPARAMS);
		return -1;
	}

	if((err = createConnection(socketInfo, host, &socketNum, &params)) != EOK)
	{// vytvoreni spojeni se serverem
		printError(err);
		return -1;
	}
	// vyhodnoceni akce klienta
	if(params.action == DOWNLOAD)
	{// stahovani dat ze serveru
		if((err = handleDownload(socketNum, params.fileName)) != EOK)
		{
			printError(err);
			return -1;
		}	
	}
	if (params.action == UPLOAD)
	{// nahravani dat na server
		if((err = handleUpload(socketNum, params.fileName)) != EOK)
		{
			printError(err);
			return -1;
		}
	}
	
	return 0;
}

