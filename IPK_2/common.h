/**
 * Projekt: IPK - projekt 2: Prenos souboru
 * Modul: common.cpp 
 * Autor: Klara Necasova
 * Email: xnecas24@stud.fit.vutbr.cz
 * Datum: duben 2016
 */

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cerrno>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <limits.h>

#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <signal.h>
#include <cstring>


// velikost bufferu 
#define BUFFER_SIZE 1024

using namespace std;

/**
 * Chybove kody.
 */
enum ecodes
{
	EOK,
	EPARAMS,
	EHOST,
	ESOCKET,
	ESOCKET_CLS,
	ECONNECT,
	ESEND, 
	EOPEN,
	EBIND,
	ELISTEN,
	ERCV,
	EUPLOAD,
	EDOWNLOAD,
	EPORT,
	EHOST_NAME,
	EACCEPT,
	END
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

/**
 * Prijima data (binarni data).
 * @param socketNum Socket pro komunikaci. 
 * @param fileName Jmeno souboru.
 * @param fileSize Velikost souboru.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int receiveData(int socketNum,string fileName, int fileSize);

/**
 * Prijima zpravy protokolu.
 * @param socketNum Socket pro komunikaci. 
 * @param data Ukazatel na zapsana data.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int receiveProtocolData(int socketNum,string* data);

/**
 * Overi existenci souboru a jeho rezim.
 * @param name Jmeno souboru.
 * @param mode Rezim souboru.
 * @param size Velikost souboru.
 * @return Vraci 0 - soubor neexistuje, 1 - jinak 
 */
bool fileExists(const string& name,const char* mode, int* size);

/**
 * Prevadi cisto typu integer na retezec (typ string). 
 * @param integer Cislo typu integer, ktere se bude prevadet.
 * @return Vraci retezec obsahujici prevedene cislo. 
 */
string intToString(int integer);

/**
 * Zajistuje odeslani binarnich dat.
 * @param commSocket Socket pro komunikaci.
 * @param fileName Jmeno souboru.
 * @param fileSize Velikost souboru.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int sendData(int commSocket, string fileName, int fileSize);

#endif
