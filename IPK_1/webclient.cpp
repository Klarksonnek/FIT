#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>

// defaultni port
#define DEFAULT_PORT 80
// velikost bufferu
#define BUFFER_SIZE 1024
// maximalni pocet presmerovani
#define REDIR_MAX 5

using namespace std;

/**
 * Struktura pro ulozeni URL.
 */
typedef struct
{
    // jmeno hosta
    string hostName;
    // cislo portu
    int port;
    // vse za jmenem hosta
    string object;
    // jmeno vystupniho souboru
    string fileName;
	// verze HTTP
	string version;
}TURLInfo;

/**
 * Struktura pro ulozeni stazenych informaci.
 */
typedef struct
{
    //hlavicka
    string head;
    // telo
    string body;
    // URL adresa hosta pri presmerovani
    string location;
    // stavovy kod HTTP
    int code;
}TDownloadContent;

/**
 * Chybove kody.
 */
enum ecodes
{
    EOK,
    EPARAMS,
    EURL,
    EREG_COMPILE,
    EHOST,
    ESOCKET,
	ESOCKET_CLS,
    ECONNECT,
    ESEND,
	EHTTP_VER,
	ENOT_FOUND,  
	EREDIR, 
	EOPEN,
	ELOCATION
};

/**
 * Chybova hlaseni odpovidajici chybovym kodum.
 */
const char *ERRMSG[] =
{
	"Vse v poradku.\n",
    "Chybne parametry prikazoveho radku!",
    "Chybne zadana URL adresa!",
    "Chyba kompilace regularniho vyrazu!",
    "Chyba pri prekladu domenoveho jmena!",
    "Chyba pri vytvareni socketu!\n",
	"Chyba pri uzavirani socketu!\n",
    "Chyba pri vytvareni spojeni!\n",
    "Chyba pri zasilani zpravy serveru!\n",
	"Chyba: 505 HTTP Version Not Supported",
	"Chyba: 404 Not Found",
	"Presmerovani!\n",
	"Chyba pri otevirani vstupniho souboru!\n",
	"Nenalezena adresa presmerovani!\n",
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
 * Vytvori asociativni pole specialnich znaku.
 * @param specChars Asociativni pole znaku.
 * @return Vraci asociativni pole specialnich znaku.
 */
void createMap(map<string,string> &specChars)
{
    specChars["<"] = "%3C";
    specChars[">"] = "%3E";
    specChars["\""] = "%22";
    specChars["#"] = "%23";
    specChars["{"] = "%7B";
    specChars["}"] = "%7D";
    specChars["|"] = "%7C";
    specChars["\\"] = "%5C";
    specChars["^"] = "%5E";
    specChars["~"] = "%7E";
    specChars["["] = "%5B";
    specChars["]"] = "%5D";
    specChars["`"] = "%60";
    specChars[" "] = "%20";
    specChars[";"] = "%3B";
}

/**
 * Prevede znak na retezec.
 * @param src Znak.
 * @return Vraci retezec.
 */
string charToStr(char src)
{
    stringstream ss;
    string s;

    if(src == ' ')
    {
        s = " ";
        return s;
    }
    ss << src;
    ss >> s;
    return s;
}

/**
 * Prekoduje specialni znaky.
 * @param src Zdrojovy retezec.
 * @param specChars Asociativni pole znaku.
 * @return Vraci prekodovany retezec.
 */
string EncodeChars(string src, map<string, string> &specChars)
{
    // Vypis obsahu mapy
    /*map<string,string>::iterator it;
    cout << "specChars contains:\n";
    for (it = specChars.begin(); it != specChars.end(); it++)
    cout << it->first << " => " << it->second << '\n';*/
    int i = 0;
    map<string,string>::iterator it;
    while(src[i] != '\0')
    {// dokud nejsme na konci retezce - projdeme ho po znaku
        // ziskani klice do asociativniho pole (string)
        string tmp = charToStr(src[i]);
        if((it = specChars.find(tmp)) != specChars.end())
        {// pokud je klic nalezen, je prekodovan dany znak
            src.replace(i, 1, specChars.find(tmp)->second);
            // posun o 3 pozice kvuli prekodovani znaku (%xy)
            i = i + 3;
        }
        else
        {// klic nebyl nalezen
            i++;
        }
    }
    return src;
}

/**
 * Zpracovava URL adresu.
 * @param webSite Analyzovana URL adresa.
 * @param url Struktura pro ulozeni casti URL adresy.
 * @return Chybovy kod.
 */
int parseURL(string webSite, TURLInfo *url)
{
    // pole struktur indexu zacatku a konce jednotlivych casti RV (hostName, port, ...)
    regmatch_t patternMatch[10];
    // udava pocet casti RV (pocet dvojic zavorek)
    regex_t regexBrackets;
    // RV pro validaci URL
    const char *pattern = "^http://([^/?:#]+)(:[0-9]+)?([/](([^?#]*)([?][^#]*)?(#(.*))?)?)?$";
	static bool saveFileName = true;

    if(regcomp(&regexBrackets, pattern, REG_EXTENDED) != 0)
    {// kompilace RV
        return EREG_COMPILE;
    }

    if((regexec(&regexBrackets, webSite.c_str(), 10, patternMatch, 0)) == 0)
    {// validace URL pomoci RV
        //patternMatch[0] - cela URL adresa
        if((patternMatch[0].rm_eo - patternMatch[0].rm_so) == 0)
        {// zbytecne?, TODO: zkusit zadat prazdny retezec
            return EURL;
        }
        // ziskani hostname
        if((patternMatch[1].rm_eo - patternMatch[1].rm_so) == 0)
        {// zbytecne?, TODO: zkusit zadat prazdny retezec
            return EHOST;
        }
        url->hostName.append(webSite, patternMatch[1].rm_so, patternMatch[1].rm_eo - patternMatch[1].rm_so);
		//cout << "URL:" << url->hostName << endl;

        if((patternMatch[2].rm_eo - patternMatch[2].rm_so) == 0)
        {// implicitni port 80
            url->port = DEFAULT_PORT;
        }
        else
        {// ziskani portu
            string portTmp;
            portTmp.append(webSite, patternMatch[2].rm_so + 1, patternMatch[2].rm_eo - patternMatch[2].rm_so);
            url->port = atoi(portTmp.c_str());
        }

        if((patternMatch[3].rm_eo - patternMatch[3].rm_so) == 0)
        {// implicitni nazev souboru
			url->object = "/";
			if(saveFileName)
			{
				url->fileName = "index.html";
				saveFileName = false;
			}        
        }
        else
        {// ziskani objektu a nazvu souboru
            url->object.append(webSite, patternMatch[3].rm_so, patternMatch[3].rm_eo - patternMatch[3].rm_so);
			if(saveFileName)
			{
				// zjisteni nazvu vystupniho souboru (nazev souboru je za poslednim lomitkem)
				unsigned int posStart = url->object.find_last_of("/") + 1;
				unsigned int posEnd = url->object.find_first_of("?");
				url->fileName = url->object.substr(posStart, posEnd - posStart);
				if(url->fileName == "")
				{
					url->fileName = "index.html";
				}
				saveFileName = false;
			}
        }
    }
    regfree(&regexBrackets);
    return EOK;
}

/**
 * Vytvori spojeni.
 * @param socketInfo Informace o spojeni.
 * @param host Informace o hostovi.
 * @param socketNum Cislo portu.
 * @param url Informace o URL adrese.
 * @return Vraci chybovy kod.
 */
int createConnection(sockaddr_in socketInfo, hostent *host, int *socketNum, TURLInfo *url)
{
    // preklad adresa -> IP
    host = gethostbyname(url->hostName.c_str());
    if (host == NULL)
    {
        return EHOST;
    }
    if ((*(socketNum) = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {// vytvoreni socketu
        return ESOCKET;
    }

    // rodina protokolu
    socketInfo.sin_family = AF_INET;
    // cislo portu
    socketInfo.sin_port = htons(url->port);
    // zkopirovani informaci o hostovi
    memcpy(&socketInfo.sin_addr, host->h_addr, host->h_length);
    if(connect(*(socketNum),(struct sockaddr *)&socketInfo, sizeof(socketInfo)) < 0)
    {// vytvoreni spojeni
        return ECONNECT;
    }
    return EOK;
}

/**
 * Stahuje zadany objekt.
 * @param siteContent Pro ulozeni stazeneho obsahu.
 * @param recvBuffer Prijimaci buffer.
 * @param socketNum Cislo portu.
 * @param url Informace o URL adrese.
 * @return Vraci chybovy kod.
 */
int download(string *siteContent, char *recvBuffer, int socketNum, TURLInfo *url)
{
    string get;

    // vytvoreni pozadavku pro server
	get.append("GET " + url->object);
	get.append(" " + url->version + "\r\n");
	get.append("Host: " + url->hostName + "\r\n");
	get.append("Connection: close\r\n");
	get.append("Content-Encoding: gzip\r\n\r\n");

    if(send(socketNum, get.c_str(), get.size() + 1, 0) < 0)
    {// posle pozadavek na server
        return ESEND;
    }

    int size = 0;
    while(((size = recv(socketNum, recvBuffer, BUFFER_SIZE, 0)) > 0))
    {// prijme odpoved od serveru
        siteContent->append(recvBuffer, size);
    }

    // vypis odpovedi
    //cout << *siteContent << endl;
	
	if(close(socketNum) < 0)
	{// uzavreni socketu
		return ESOCKET_CLS;
	}

    return EOK;
}

/**
 * Parsuje prijatou zpravu, rozdeli ji na hlavicku a telo.
 * @param content Pro ulozeni naparsovaneho obsahu. 
 * @param siteContent Prijata zprava.
 */
void parseDownloadedContent(TDownloadContent *content, string siteContent)
{
    // zjisteni konce hlavicky
    unsigned int position = siteContent.find("\r\n\r\n");
    // ziskani hlavicky
    content->head = siteContent.substr(0, position);
    //cout << content->head << endl;
    // ziskani tela
    content->body = siteContent.substr(position + 4);
    //cout << content->body << endl;
    // kod
    content->code = atoi(content->head.substr(9, 3).c_str());
}

/**
 * Zjisti adresu pri presmerovani.
 * @param content Naparsovany obsah. 
 * @return Vraci chybovy kod. 
 */
int redirLocation(TDownloadContent *content)
{
	string locDetect = "Location: ";
	unsigned int locPosition = content->head.find(locDetect);
	if(locPosition == string::npos)
	{
		return ELOCATION;
	}
	unsigned int enterPosition = content->head.find("\r\n", locPosition);
	content->location = content->head.substr(locPosition + locDetect.length(), enterPosition - locPosition - locDetect.length());
	//cout << locPosition << endl;
	//cout << content->location << endl;
	return EOK;
}

/**
 * Ulozi ziskana data do souboru.
 * @param body Telo zpravy. 
 * @param fileName Nazev souboru. 
 * @return Vraci chybovy kod.
 */
int saveContent(string body, string fileName)
{
	ofstream file;
	char *pEnd = NULL;
	const char *s = body.substr(0, body.find("\n")).c_str();
	strtoul(s, &pEnd, 16);
	
	// zpracovani chunk zpravy
	if(*pEnd == 0 && body.find("\n") != 0) 
	{
		int chunked = 0;
		int index;
		string chunkedOutput = "";
		do
		{	// zjisteni velikosti chunku
			index = body.find("\n"); 
			// prevod cisla hex na dec
			chunked = strtoul(body.substr(0, index).c_str(), NULL, 16);
			// odstraneni chunku a enteru
			body.erase(0, index + 1);
			// ulozeni udaju
			chunkedOutput = chunkedOutput + body.substr(0, chunked);
			// odstraneni udaju a \n\r
			body.erase(0, chunked + 2); 
		}while(chunked > 0);
		body = chunkedOutput;
	}

	// ulozeni ziskanych udaju do souboru
	// trunc - mazani predchazejiciho obsahu souboru, binary - binarni soubor
	file.open(fileName.c_str(), ios::out | ios::binary);
	if(file.is_open())
	{// otevreni soubor
		file.write(body.c_str(), body.length());
		// uzavreni souboru
		file.close();
	}  
	else
	{
		return EOPEN;
	}
	return EOK;
}

/**
 * Rozpozna stavovy kod HTTP.
 * @param code Kod. 
 * @reutrn Vraci odpovidajici chybovy kod.
 */
int errorDetection(int code)
{
	if((code >= 200 and code <= 208) || code == 226)
    {// vse ok
		return EOK;
	}
	else if(code == 301 || code == 302)
	{// presmerovani (redirection)
		return EREDIR;
	}
	else if(code == 404)
	{// Not Found
		return ENOT_FOUND;
	}
	else if((code > 404 and code < 512) || code == 400)
	{// HTTP Version Not Supported
		return EHTTP_VER;
	}
	return EOK;
}

/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
    if(argc != 2)
    {// chybny pocet parametru
        printError(EPARAMS);
        return -1;
    }

    string webSite = argv[1];
    TURLInfo url;
    int err;
    //informace o socketu hosta
    sockaddr_in socketInfo;
    // host
    hostent *host = NULL;
    // cislo socketu
    int socketNum;
    // prijimaci buffer
    char recvBuffer[BUFFER_SIZE];
    // obsah stranky
    string siteContent;
    // informace o stazenem objektu
    TDownloadContent content;
    int redirCount = 0;
    // asociativni pole znaku
    map<string, string> specChars;
    createMap(specChars);

	url.version = "HTTP/1.1";
	while(redirCount < REDIR_MAX)
	{
		// parsovani URL
		if((err = parseURL(webSite, &url)) != EOK)
		{
			printError(err);
			return -1;
		}
		// prekodovani objektu
		url.object = EncodeChars(url.object, specChars);
		url.hostName = EncodeChars(url.hostName, specChars);

		/*Vypis asociativniho pole.
		map<string,string>::iterator it;
		cout << "specChars contains:\n";
		for (it = specChars.begin(); it != specChars.end(); it++)
		cout << it->first << " => " << it->second << '\n';*/

		// url.object
		//cout << EncodeChars("www.fit.vutbr.cz/<a>b\"c#d{e}f|g\\h^i~j[k]l`m n;o", specChars) << endl;

		// vytvoreni spojeni
		if((err = createConnection(socketInfo, host, &socketNum, &url)) != EOK)
		{
			printError(err);
			return -1;
		}

		// stazeni objektu
		if(download(&siteContent, recvBuffer, socketNum, &url) != EOK)
		{
			printError(ESEND);
			return -1;
		}
		
		// zpracovani informaci o stazenem objektu
		parseDownloadedContent(&content, siteContent);
		
		// rozpoznani stavoveho kodu HTTP
		err = errorDetection(content.code);
		if(err == EOK)
		{
			// vse ok - ulozeni udaju
			if(saveContent(content.body, url.fileName) != EOK)
			{
				printError(EOPEN);
				return -1;
			}
			return EOK;
		}
		else if(err == EREDIR)
		{// presmerovani
			if(redirLocation(&content) == ELOCATION)
			{
				printError(ELOCATION);
				return -1;
			}
			webSite = content.location;
			redirCount++;
		}
		else if(err == EHTTP_VER)
		{// HTTP verze neni podporovana -> zmena na verzi 1.0
			url.version = "HTTP/1.0";
		}
		// TODO: kdy inkrementovat redir?
		
		// vycisteni retezcu
		siteContent.clear();
		url.hostName.clear();
		url.object.clear();
	}
    return EOK;
}



