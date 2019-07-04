/**
 * Projekt: IPK - projekt 2: Prenos souboru
 * Modul: server.cpp 
 * Autor: Klara Necasova
 * Email: xnecas24@stud.fit.vutbr.cz
 * Datum: duben 2016
 */

// hlavickovy soubor se spolecnymi funkcemi
#include "common.h"

/**
 * Struktura pro ulozeni informaci z prikazove radky.
 */
typedef struct
{
	// cislo portu
	int port;
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
	char* pEnd = NULL;
	if(argc != 3)
	{
		return EPARAMS;
	}
	else if(strcmp(argv[1], "-p") == 0)
	{// cislo portu
		params->port = strtoul(argv[2], &pEnd, 10);
		if(*pEnd != '\0')
		{
			return EPARAMS;
		}
		if(params->port < 0 || params->port > 65535)
		{
			return EPORT;
		}
	}
	else
	{
		return EPARAMS;
	}
	return EOK;
}

/**
 * Ukoncuje procesy, ktere zustanou neukonceny. 
 */
void deleteZombie(int n)
{
	// ID procesu
	pid_t pid; 
	 // stav procesu
	int status;

	// cyklus ukoncuje procesy zombie
	while((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
	}  
}

/**
 * Zajistuje stazeni souboru.
 * @param commSocket Socket pro komunikaci.
 * @param fileName Jmeno souboru.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int handleDownload(int commSocket, string fileName)
{
	string response;
	int fileSize;
	int err;

	if(fileExists(fileName, "r", &fileSize))
	{// overeni existence souboru, rezimu a zjisteni jeho velikosti
		// odpoved - je mozne zahajit stahovani
		response = "D:YES#" + intToString(fileSize);

		if(send(commSocket, response.c_str(), response.length()+1, 0) < 0)
		{// zaslani odpovedi klientovi
			return ESEND;
		}

		// vstupni soubor
		ifstream file(fileName.c_str(), ios::in|ios::binary|ios::ate);
		if(!file.is_open())
		{// overeni, zda je soubor otevren
			return EOPEN;
		}
		response.clear();
		if((err = receiveProtocolData(commSocket, &response)) != EOK)
		{// prijem zpravy od klienta - potvrzeni spojeni
			return err;
		}

		if(response.compare("ACK") == 0)
		{// spojeni s klientem je potvrzeno
			if((err = sendData(commSocket, fileName,fileSize)) != EOK)
			{// odeslani souboru klientovi
				return err;
			}	
		}
	}
	else
	{// odpoved - neni mozne zahajit stahovani
		response = "D:NO";

		if(send(commSocket, response.c_str(), response.length() + 1, 0) < 0)
		{// odeslani dat klientovi
			return ESEND;
		}
	}
	return EOK;
}

/**
 * Zajistuje nahrani souboru na server.
 * @param commSocket Socket pro komunikaci.
 * @param fileName Jmeno souboru.
 * @param fileSizeClient Velikost souboru od klienta.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int handleUpload(int commSocket, string fileName,int fileSizeClient)
{
	string response;
	int err;
	int fileSizeServer = 0;

	if(fileExists(fileName, "wb+", &fileSizeServer))
	{
		// pokud bude moznost do souboru zapsat, odesleme na klienta + odpoved
		response = "U:YES";

		if(send(commSocket, response.c_str(), response.length() + 1, 0) < 0)
		{// odeslani dat klientovi
			return EUPLOAD;
		}

		response.clear();
		if((err = receiveData(commSocket, fileName, fileSizeClient)) != EOK)
		{// prijem dat od klienta
			return err;
		}
		
		response = "ACK";
		if(send(commSocket, response.c_str(), response.length() + 1, 0) < 0)
		{// odeslani pozadavku na server
			return ESEND;
		}
	}
	else
	{
		response = "U:NO";

		if(send(commSocket, response.c_str(), response.length() + 1, 0) < 0)
		{// odeslani dat klientovi
			return EUPLOAD;
		}		
	}
	return EOK;
}

/**
  * Hlavni funkce serveru.
  * @param commSocket Socket pro komunikaci.
  * @return Vraci EOK nebo prislusny chybovy kod.
  */
int doWork(int commSocket)
{
	string request;
	int err;
	string fileName; 
	int fileSize = 0;
	
	if((err = receiveProtocolData(commSocket, &request)) != EOK)
	{// prijme pozadavek klienta: download nebo upload
		return err;
	}

	if(request[0] == '<' && request[request.length() - 1] == '?')
	{// overeni formatu pozadavku - stahovani
		fileName = request.substr(1, request.length() - 2);
		if((err = handleDownload(commSocket, fileName)) != EOK)
		{
			return err;
		}
	}
	else if(request[0] == '>') 
	{// overeni formatu pozadavku - nahravani
		fileName = request.substr(1, request.find('?') - 1);
		istringstream(request.substr(request.find('#') + 1)) >> fileSize;
		if((err = handleUpload(commSocket, fileName,fileSize)) != EOK)
		{
			return err;
		}
	}
	else
	{
		return END;
	}
	return EOK; 
}

/**
 * Vytvori spojeni.
 * @param socketListen Informace o spojeni.
 * @param socketNum Cislo portu.
 * @param params Parametry prikazoveho radku.
 * @return Vraci EOK nebo prislusny chybovy kod.
 */
int createConnection(sockaddr_in socketListen, int *socketNum, TParams *params)
{
	int err;

	if((*(socketNum) = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{// vytvoreni socketu
		return ESOCKET;
	}

	//memset(&socketListen,0,sizeof(socketListen));
	// rodina protokolu
	socketListen.sin_family = PF_INET;
	// program nepotrebuje znat presnou IP adresu 
	socketListen.sin_addr.s_addr = INADDR_ANY;     
	// cislo portu
	socketListen.sin_port = htons(params->port);

	if((err = bind(*(socketNum), (struct sockaddr*)& socketListen, sizeof(socketListen))) < 0) 
	{// navazani spojeni
		return EBIND;
	}

	if((err = listen(*(socketNum),10)) < 0)
	{// naslouchani
		return ELISTEN;
	}

	return EOK;
}

/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
	//stuktura s parametry prikazove radky
	TParams params;
	// chybovy kod
	int err;
	//informace o socketu, na kterem posloucha server
    sockaddr_in socketListen;
	//informace o socketu, na kterem bude probihat komunikace
    sockaddr_in socketClient;
	socklen_t socketClientLength = sizeof(socketClient);
    // cislo socketu
    int socketNum;
	int commSocket;
	// identifikator potomka
	int pid;

	if(GetParam(argc, argv, &params) != EOK)
	{// zpracovani parametru prikazove radky
		printError(EPARAMS);
		return -1;
	}

	if((err = createConnection(socketListen, &socketNum, &params)) != EOK)
	{// vytvoreni spojeni
		printError(err); 
		return -1;
	}

	// nastaveni prace se signaly
	signal(SIGCHLD, deleteZombie);

	while (1)
	{
		if((commSocket = accept(socketNum, (struct sockaddr*) &socketClient, &socketClientLength)) < 0)
		{// komunikace s klientskou casti
			printError(EACCEPT);
			return -1;
		}

		if((pid = fork()) == 0)
		{// zajisteni konkurentnosti serveru - soucastna obsluha vice klientu
			if(close(socketNum) < 0)
			{
				return ESOCKET_CLS;
			}
			while(1)
			{
				if((err = doWork(commSocket)) != EOK)
				{
					if(err != END)
					{
						printError(err);
					}
					break;
				}
			}
			if(close(commSocket) < 0)
			{
				return ESOCKET_CLS;
			}
			exit(-1);
		}
		else 
		{
			if(close(commSocket) < 0)
			{
				return ESOCKET_CLS;
			}
		}
	}
	return 0;
}

