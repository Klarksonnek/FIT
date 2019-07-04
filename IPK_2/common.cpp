/**
 * Projekt: IPK - projekt 2: Prenos souboru
 * Modul: common.cpp 
 * Autor: Klara Necasova
 * Email: xnecas24@stud.fit.vutbr.cz
 * Datum: duben 2016
 */

#include "common.h"

/**
 * Chybova hlaseni odpovidajici chybovym kodum.
 */
const char *ERRMSG[] =
{
	"Vse v poradku.\n",
	"Chybne parametry prikazoveho radku!\n",
	"Chyba pri prekladu domenoveho jmena!\n",
	"Chyba pri vytvareni socketu!\n",
	"Chyba pri uzavirani socketu!\n",
	"Chyba pri vytvareni spojeni!\n",
	"Chyba pri zasilani zpravy serveru!\n",
	"Chyba pri otevirani vstupniho souboru!\n",
	"Chyba pri navazovani spojeni!\n",
	"Chyba pri naslouchani!\n",
	"Chyba pri prijmu zpravy!\n",
	"Chyba pri nahravani souboru na server!\n",
	"Chyba pri stahovani dat ze serveru!\n",
	"Zadan port mimo rozsah!\n",
	"Hostname nezadan!\n",
	"Chyba pro volani funkce accept()!\n",
	"Konec obsluhy!\n"
};

/**
 * Vytiskne chybove hlaseni.
 * @param errCode Chybovy kod.
 */
void printError(int errCode)
{
    cerr << ERRMSG[errCode] << endl;
}

/**
 * Prijima data (binarni data).
 * @param socketNum Socket pro komunikaci. 
 * @param fileName Jmeno souboru.
 * @param fileSize Velikost souboru.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int receiveData(int socketNum, string fileName, int fileSize) 
{
	char *rbuffer = new char[BUFFER_SIZE];
	int size = 0;
	int totalSize = 0;
	string data;

	// vystupni soubor
	FILE *f;

	while(totalSize < fileSize && (size = recv(socketNum, rbuffer, BUFFER_SIZE, 0)) > 0)
	{// prijem dat
		// ulozeni dat do bufferu
		data.append(rbuffer, size);
		totalSize += size;
	}
	if(size == -1) 
	{// chyba pri prijmu dat
		return ERCV;
	}
	if((f = fopen(fileName.c_str(), "wb+")) == NULL) 
	{// soubor se nepodarilo otevrit
		return EOPEN;
		
	}
	else 
	{// zapis dat do souboru a uzavreni souboru
		fwrite(data.c_str(), 1, totalSize, f);
		fclose(f);
    }
	return EOK;
}

/**
 * Prijima zpravy protokolu.
 * @param socketNum Socket pro komunikaci. 
 * @param data Ukazatel na zapsana data.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int receiveProtocolData(int socketNum, string* data) 
{
	char rbuffer[BUFFER_SIZE];
	int size = 0;

	while((size = recv(socketNum, rbuffer, BUFFER_SIZE, 0)) > 0)
	{// prijem dat
		// ulozeni dat do bufferu
		data->append(rbuffer, size);

		if(data->find('\0', data->length() - 1) != string::npos)
		{// ukonceni prijmu dat
			*data = data->erase(data->length() - 1);
			break;
		}
	}

	if(size == -1) 
	{// chyba pri prijmu dat
		return ERCV;
	}
	return EOK;
}

/**
 * Overi existenci souboru a jeho rezim.
 * @param name Jmeno souboru.
 * @param mode Rezim souboru.
 * @param size Velikost souboru.
 * @return Vraci 0 - soubor neexistuje, 1 - jinak 
 */
bool fileExists(const string& name,const char* mode, int* size)
{
	if(FILE *file = fopen(name.c_str(), mode)) 
	{
		// zjisteni velikosti souboru a uzavreni souboru
		fseek(file, 0L, SEEK_END);
		*size = ftell(file);
		fseek(file, 0L, SEEK_END);
		fclose(file);
		return true;
	} 
	else 
	{
		return false;
	}   
}

/**
 * Prevadi cisto typu integer na retezec (typ string). 
 * @param integer Cislo typu integer, ktere se bude prevadet.
 * @return Vraci retezec obsahujici prevedene cislo. 
 */
string intToString(int integer)
{
	return static_cast<ostringstream*>(&(ostringstream() << integer))->str();
}

/**
 * Zajistuje odeslani binarnich dat.
 * @param commSocket Socket pro komunikaci.
 * @param fileName Jmeno souboru.
 * @param fileSize Velikost souboru.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int sendData(int commSocket, string fileName, int fileSize)
{
	// soubor v binarnim rezimu
	ifstream file(fileName.c_str(), ios::in|ios::binary|ios::ate);
	// obsah souboru
	char* fileContent = NULL;
	if(file.is_open())
	{
		// nacteni obsahu souboru
		fileContent = new char[fileSize];
		file.seekg(0, ios::beg);
		file.read(fileContent, fileSize);
	}
	// uzavreni souboru
	file.close();

	if(send(commSocket, fileContent, fileSize, 0) < 0)
	{// odeslani dat klientovi
		delete [] fileContent;
		return ESEND;
	}

	delete [] fileContent;
	return EOK;
}


