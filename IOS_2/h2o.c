/*
 * Soubor:  h2o.c
 * Projekt: 2. uloha pro predmet IOS (Operacni systemy)
 *          Building H2O Problem
 * Autor:   Klara Necasova (xnecas24@stud.fit.vutbr.cz)
 * Datum:   Kveten 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
//#include <errno.h>

// jmeno logovaciho souboru
#define FILENAME "h2o.out"

// maximalni cas (generovani kysliku, vodiku, provadeni funkce bond())
#define MAXTIME 5001


/**
 * Struktura obsahujici hodnoty parametru prikazove radky.
 */
typedef struct
{
	int hydrogenCount; // pocet procesu reprezentujicich vodik
	int oxygenCount;   // pocet procesu reprezentujicich kyslik
	int oxygenTime;    // max. doba, po kerou se generuje proces pro kyslik
	int hydrogenTime;  // max. doba, po kerou se generuje proces pro vodik
	int bondTime;      // max. doba provadeni funkce bond()
	int error;         // pro indikaci chyby
} TParams;

/**
 * Struktura pro sdilenou pamet.
 */
typedef struct
{
	sem_t sWriteToFile;
 	sem_t sAtomMutex;
 	sem_t sWaitForBondingEnd;
	sem_t oxyQueue;
	sem_t hydroQueue;
	sem_t bondedBarrierMutex;
	sem_t bondedBarrier;
	sem_t finishedBarrierMutex;
	sem_t finishedBarrier;

	int hydrogenCount;
	int oxygenCount;
	int actionCounter;
	int bondedBarrierCnt;
	int finishedBarrierCnt;
} TSharedMem;

/** Typy procesu - vodik/kyslik. */
enum atomtypes
{
	HYDROGEN,
	OXYGEN,
};

/** Kody pro identifikaci vystupni zpravy. */
enum msgcodes
{
    STARTED,
    WAITING,
    READY,
    BEGIN_BONDING,
    BONDED,
    FINISHED,
};

/** Hlaseni odpovidajici kodum pro identifikaci vystupni zpravy.  */
const char *STATEMSG[] =
{
  "started\n",
  "waiting\n",
  "ready\n",
  "begin bonding\n",
  "bonded\n",
  "finished\n",
};

/** Kody chyb programu. */
enum tecodes
{
  EOK,          /**< Bez chyby. */
  EPARAMS,      /**< Chybny prikazovy radek. */
  ECONVERTION,  /**< Chyba pri prevodu retezce na cislo. */
  ERANGE,       /**< Chybny rozsah. */
  EFOPEN,       /**< Chyba pri otevirani souboru. */
  ESHMEM,       /**< Chyba pri praci se sdilenou pameti. */
  EFORK,        /**< Chyba pri volani funkce fork(). */
  EWAIT,        /**< Chyba pri volani funkce wait(). */
  EUNKNOWN,     /**< Neznama chyba. */
};

/** Chybova hlaseni odpovidajici chybovym kodum ve vyctu tecodes. */
const char *ECODEMSG[] =
{
  "Vse v poradku.\n",
  "Chybne parametry prikazoveho radku!\n",
  "Nezdaril se prevod retezce na cislo!\n",
  "Parametry prikazovaho radku jsou mimo rozsah!\n",
  "Soubor se nezdarilo otevrit!\n",
  "Chyba pri praci se sdilenou pameti!\n",
  "Chyba pri vytvareni procesu!\n",
  "Chyba pri volani funkce wait!\n",
  "Neznama chyba!\n",
};

/**
 * Vytiskne hlaseni odpovidajici chybovemu kodu.
 * @param ecode Kod chyby programu.
 */
void printECode(int ecode)
{
  if(ecode < EOK || ecode > EUNKNOWN)
  {
    ecode = EUNKNOWN;
  }
  fprintf(stderr, "%s", ECODEMSG[ecode]);
}


/**
 * Zpracuje argumenty prikazoveho radku a vrati je ve strukture TParams.
 * Pokud je format argumentu chybny, ukonci program s chybovym kodem.
 * @param argc Pocet argumentu.
 * @param argv Pole textovych retezcu s argumenty.
 * @return Vraci analyzovane argumenty prikazoveho radku.
 */
TParams getParams(int argc, char *argv[])
{
    TParams params;
    // inicializace struktury
	params.bondTime = 0;
	params.hydrogenCount = 0;
    params.hydrogenTime = 0;
    params.oxygenCount = 0;
    params.oxygenTime = 0;
    params.error = EOK;

    if(argc != 5)
    {// neodpovidajici pocet argumentu
        params.error = EPARAMS;
        return params;
    }

    char* pEnd;
    /// pocet procesu kysliku
    params.oxygenCount = strtol(argv[1], &pEnd, 10);
	if(*pEnd != '\0')
    {
        params.error = ECONVERTION;
    }
    if(params.oxygenCount <= 0)
    {
        params.error = ERANGE;
    }
    // pocet atomu vodiku = 2 * pocet_atomu_kysliku
	params.hydrogenCount = params.oxygenCount*2;

    /// max doba generovani - vodik
    params.hydrogenTime = strtol (argv[2], &pEnd, 10);
    if(*pEnd != '\0')
    {
        params.error = ECONVERTION;
    }
    if(params.hydrogenTime < 0 || params.hydrogenTime >= MAXTIME)
    {
        params.error = ERANGE;
    }
    /// max doba generovani - kyslik
    params.oxygenTime = strtol (argv[3], &pEnd, 10);
    if(*pEnd != '\0')
    {
        params.error = ECONVERTION;
    }
    if(params.oxygenTime < 0 || params.oxygenTime >= MAXTIME)
    {
        params.error = ERANGE;
    }
    /// max doba provadeni funkce bond()
    params.bondTime = strtol (argv[4], &pEnd, 10);
    if(*pEnd != '\0')
    {
        params.error = ECONVERTION;
    }
    if(params.bondTime < 0 || params.bondTime >= MAXTIME)
    {
        params.error = ERANGE;
    }
    return params;
}

/**
 * Vytvori sdilenou pamet.
 * @param shmif ID sdilene pameti.
 * @param path Cesta k souboru - pouzije se pro vygenerovani klice (funkce ftok()).
 * @return Vraci chybovy kod, jinak EOK.
 */
int createSharedMemory(int* shmid, const char *path)
{
    // zisk jedinecneho klice
	key_t key = ftok(path, 0x01);
	if((*shmid = shmget(key, sizeof(TSharedMem), IPC_CREAT | 0666)) == -1)
	{// zisk sdilene pameti
		printECode(ESHMEM);
		return 2;
	}
	return EOK;
}

/**
 * Inicializuje sdilenou pamet.
 * @param shmid ID sdilene pameti.
 * @param tsm Ukazatel na strukturu reprezentujici sdilenou pamet.
 * @return Vraci chybovy kod, jinak EOK.
 */
int initSharedMemory(int shmid, TSharedMem **tsm)
{
	if((*tsm = shmat(shmid, NULL, 0)) == (void *) -1)
	{// zisk odkazu na sdilenou pamet
		printECode(ESHMEM);
		return 2;
	}

    // inicializace struktury
	TSharedMem *tsmptr = *tsm;
	// inicializace sdilenych promennych
	tsmptr->actionCounter = 0;
	tsmptr->hydrogenCount = 0;
	tsmptr->oxygenCount = 0;
	tsmptr->bondedBarrierCnt = 0;
	tsmptr->finishedBarrierCnt = 0;

    // inicializace semaforu
	if(sem_init(&(tsmptr->sWriteToFile), 1, 1) == -1 ||
       sem_init(&(tsmptr->sAtomMutex), 1, 1) == -1 ||
       sem_init(&(tsmptr->oxyQueue), 1, 0) == -1 ||
       sem_init(&(tsmptr->hydroQueue), 1, 0) == -1 ||
       sem_init(&(tsmptr->bondedBarrierMutex), 1, 1) == -1 ||
       sem_init(&(tsmptr->bondedBarrier), 1, 0) == -1 ||
       sem_init(&(tsmptr->finishedBarrierMutex), 1, 1) == -1 ||
       sem_init(&(tsmptr->finishedBarrier), 1, 0) == -1)
    {
        printECode(ESHMEM);
		return 2;
    }
	return EOK;
}

/**
 * Uvolni sdilenou pamet.
 * @param tsm Ukazatel na strukturu reprezentujici sdilenou pamet.
 * @param shmid ID sdilene pameti.
 * @return Vraci chybovy kod, jinak EOK.
 */
int freeSharedMemory(TSharedMem* tsm, int shmid)
{
    // odstraneni semaforu
	if(sem_destroy(&(tsm->sWriteToFile)) == -1 ||
       sem_destroy(&(tsm->sAtomMutex)) == -1 ||
       sem_destroy(&(tsm->oxyQueue)) == -1 ||
       sem_destroy(&(tsm->hydroQueue)) == -1 ||
       sem_destroy(&(tsm->bondedBarrierMutex)) == -1 ||
       sem_destroy(&(tsm->bondedBarrier)) == -1 ||
       sem_destroy(&(tsm->finishedBarrierMutex)) == -1 ||
       sem_destroy(&(tsm->finishedBarrier)) == -1)
    {
        printECode(ESHMEM);
		return 2;
    }

	if(shmctl(shmid, IPC_RMID, NULL) == -1)
	{// odstraneni segmentu sdilene pameti (IPC_RMID)
		printECode(ESHMEM);
		return 2;
	}
	return EOK;
}

/**
 * Funkce pro zapis do souboru.
 * @param tsm Ukazatel na strukturu reprezentujici sdilenou pamet.
 * @param type Typ atomu (vodik/kyslik).
 * @param id Udava poradi atomu daneho typu.
 * @param msgType Typ zpravy pro vypis.
 * @param f Popisovac souboru.
 * @return Vraci chybovy kod, jinak EOK.
 */
int writeToFile(TSharedMem* tsm, int type, int id, int msgType, FILE *f)
{
    // zamkne se semafor pro vylucny pristup k souboru
    sem_wait(&tsm->sWriteToFile);
    // zvysime citac akci
    tsm->actionCounter++;

    // zapis do souboru
    fprintf(f, "%d\t: %c %d\t: %s", tsm->actionCounter, type == OXYGEN ? 'O' : 'H', id + 1, STATEMSG[msgType]);

    // odemkne se semafor pro vylucny pristup k souboru
    sem_post(&tsm->sWriteToFile);
    return EOK;
}

/**
 * Funkce pro tvorbu molekuly vody (h2O).
 * @param tsm Ukazatel na strukturu reprezentujici sdilenou pamet.
 * @param type Typ atomu (vodik/kyslik).
 * @param bondTime Maximalni cas trvani funkce bond().
 * @param id Udava poradi atomu daneho typu.
 * @param f Popisovac souboru.
 * @return Vraci chybovy kod, jinak EOK.
 */
void bond(TSharedMem* tsm, int bondTime, int type, int id, FILE *f)
{
    // zapis prislusneho hlaseni do souboru
    writeToFile(tsm, type, id, BEGIN_BONDING, f);
    // simulace zpozdeni
    if(bondTime == 0)
    {
       usleep(0);
    }
    else
    {
        usleep((rand() % bondTime) * 1000);
    }
}

/**
 * Funkce reprezentujici zivotni cyklus atomu.
 * @param tsm Ukazatel na strukturu reprezentujici sdilenou pamet.
 * @param params Ukazatel na strukturu s parametry prikazove radky.
 * @param type Typ atomu (vodik/kyslik).
 * @param id Udava poradi atomu daneho typu.
 * @param shmid ID sdilene pameti.
 * @param f Popisovac souboru.
 * @return Vraci chybovy kod, jinak EOK.
 */
int atomLifeCycle(TSharedMem* tsm, TParams* params, int type, int id, FILE *f)
{
    if(type == OXYGEN)
    {/// KYSLIK
        // uzamkneme kritickou sekci (KS)
    	sem_wait(&tsm->sAtomMutex);
    	// zapis do souboru - started
    	writeToFile(tsm, type, id, STARTED, f);
        // inkrementace citace molekul kysliku
        tsm->oxygenCount+=1;

        if(tsm->hydrogenCount >= 2)
        {// podminka nutna pro vytvoreni molekuly h2O
            // zapis do souboru - ready
            writeToFile(tsm, type, id, READY, f);
            // uvolneni 2 vodiku
            sem_post(&tsm->hydroQueue);
            sem_post(&tsm->hydroQueue);
            tsm->hydrogenCount -= 2;
            // uvolneni 1 kysliku
            sem_post(&tsm->oxyQueue);
            tsm->oxygenCount -= 1;
        }
        else
        {// neni dostatek vodiku pro vytvoreni molekuly h2O - uvolnime KS
            // zapis do souboru - waiting
            writeToFile(tsm, type, id, WAITING, f);
            // uvolneni kriticke sekce
            sem_post(&tsm->sAtomMutex);
        }
        // bariera pred volanim funkce bond()
        sem_wait(&tsm->oxyQueue);

        // volani funkce bond()
        bond(tsm, params->bondTime, type, id, f);

        // bariera pro synchronizaci vypisu bonded - vypis ve spravnem poradi
        sem_wait(&tsm->bondedBarrierMutex);
            tsm->bondedBarrierCnt++;
        sem_post(&tsm->bondedBarrierMutex);
        if(tsm->bondedBarrierCnt % 3 == 0)
        {// cekame na 3 atomy
            sem_post(&tsm->bondedBarrier);
        }

        sem_wait(&tsm->bondedBarrier);
        sem_post(&tsm->bondedBarrier);

        // zapis do souboru - bonded
        writeToFile(tsm, type, id, BONDED, f);
        // uvolneni KS
        sem_post(&tsm->sAtomMutex);
    }
    /////////////////////////////////////////////////////////////
    if(type == HYDROGEN)
    {/// VODIK
        // uzamkneme kritickou sekci (KS)
    	sem_wait(&tsm->sAtomMutex);
    	// zapis do souboru - started
    	writeToFile(tsm, type, id, STARTED, f);
        // inkrementace citace molekul vodiku
        tsm->hydrogenCount += 1;

        if(tsm->hydrogenCount >= 2 && tsm->oxygenCount >= 1)
        {// podminka nutna pro vytvoreni molekuly h2O
            // zapis do souboru - ready
            writeToFile(tsm, type, id, READY, f);
            // uvolneni 2 vodiku
            sem_post(&tsm->hydroQueue);
            sem_post(&tsm->hydroQueue);
            tsm->hydrogenCount -= 2;
            // uvolneni 1 kysliku
            sem_post(&tsm->oxyQueue);
            tsm->oxygenCount -= 1;
        }
        else
        {// neni dostatek molekul pro vytvoreni molekuly h2O - uvolnime KS
            // zapis do souboru - waiting
            writeToFile(tsm, type, id, WAITING, f);
            // uvolneni KS
            sem_post(&tsm->sAtomMutex);
        }
        // bariera pred volanim funkce bond()
        sem_wait(&tsm->hydroQueue);
        // volani funkce bond()
        bond(tsm, params->bondTime, type, id, f);

        // bariera pro synchronizaci vypisu bonded - vypis ve spravnem poradi
        sem_wait(&tsm->bondedBarrierMutex);
            tsm->bondedBarrierCnt++;
        sem_post(&tsm->bondedBarrierMutex);
        if(tsm->bondedBarrierCnt % 3 == 0)
        {// cekame na 3 atomy
           sem_post(&tsm->bondedBarrier);
        }

        sem_wait(&tsm->bondedBarrier);
        sem_post(&tsm->bondedBarrier);

    	// zapis do souboru - bonded
        writeToFile(tsm, type, id, BONDED, f);
    }

	// bariera pro synchronizaci vypisu finished - cekani na vytvoreni molekul ze vsech atomu
	sem_wait(&tsm->finishedBarrierMutex);
        tsm->finishedBarrierCnt++;
	sem_post(&tsm->finishedBarrierMutex);
	if(tsm->finishedBarrierCnt == params->hydrogenCount + params->oxygenCount)
    {// pokud se pocet dokoncenych molekul rovna souctu atomu vodiku a kysliku -> konec
        sem_post(&tsm->finishedBarrier);
    }
	sem_wait(&tsm->finishedBarrier);
	sem_post(&tsm->finishedBarrier);

    // zapis do souboru - finished
    writeToFile(tsm, type, id, FINISHED, f);

    return EOK;
}

/**
 * Funkce pro generovani potomku daneho typu.
 * @param tsm Ukazatel na strukturu reprezentujici sdilenou pamet.
 * @param params Ukazatel na strukturu s parametry prikazove radky.
 * @param type Typ atomu (vodik/kyslik).
 * @param shmid ID sdilene pameti.
 * @param f Popisovac souboru.
 * @return Vraci chybovy kod, jinak EOK.
 */
int generateAtoms(TSharedMem* tsm, TParams* params, int type, int shmid, FILE *f)
{
	pid_t pid = -1;
	int childCount = 0;

	if(type == OXYGEN)
	{/// KYSLIK
		for(int i=0; i < params->oxygenCount; i++)
		{// vytvoreni daneho poctu atomu kysliku
            // simulace zpozdeni
		    if(params->oxygenTime == 0)
            {
               usleep(0);
            }
            else
            {
                usleep((rand() % params->oxygenTime) * 1000);
            }
            if((pid = fork()) == 0)
            {// vytvareni potomku
                if((tsm = shmat(shmid, NULL, 0)) == (void *) -1)
                {// prirazeni segmentu sdilene pameti pro dany proces
                    printECode(ESHMEM);
                    return 2;
                }
                // zivotni cyklus atomu
                atomLifeCycle(tsm, params, type, i, f);
                return EOK;
            }
            else if(pid == -1)
            {// overeni uspesnosti funkce fork()
                printECode(ESHMEM);
                freeSharedMemory(tsm, shmid);
                return 2;///???
            }
            else
            {// inkrementace citace poctu potomku
               childCount++;
            }
		}// end for
	}
	else
	{/// VODIK
		for(int i=0; i<params->hydrogenCount; i++)
		{// vytvoreni daneho poctu atomu vodiku
            // simulace zpozdeni
            if(params->hydrogenTime == 0)
            {
               usleep(0);
            }
            else
            {
                usleep((rand() % params->hydrogenTime) * 1000);
            }
            if ((pid = fork()) == 0)
            {// vytvoreni potomku
                if((tsm = shmat(shmid, NULL, 0)) == (void *) -1)
                {// prirazeni segmentu sdilene pameti pro dany proces
                    printECode(ESHMEM);
                    return 2;
                }
                // zivotni cyklus atomu
                atomLifeCycle(tsm, params, type, i, f);
                return EOK; ///???
            }
            else if (pid == -1)
            {// overeni uspesnosti funkce fork()
                printECode(ESHMEM);
                freeSharedMemory(tsm, shmid);
                return 2;

            }
            else
            {// inkrementace citace poctu potomku
               childCount++;
            }
		}// end for
	}
    ////////////////////////////////////////
    if(pid > 0)
    {// rodicovsky proces ceka na dokonceni potomku
        for(int i = 0; i < childCount; i++)
        {
            pid_t child_pid;
            pid=wait(&child_pid);
            if(pid == -1)
            {
                printECode(EWAIT);
                return 2;
            }
        }
    }
    return EOK;
}

/**
 * Funkce pro vytvoreni 2 pomocnych procesu (generatoru) pro vodik a kyslik.
 * @param tsm Ukazatel na strukturu reprezentujici sdilenou pamet.
 * @param params Ukazatel na strukturu s parametry prikazove radky.
 * @param shmid ID sdilene pameti.
 * @param f Popisovac souboru.
 * @return Vraci chybovy kod, jinak EOK.
 */
int createGenerators(TSharedMem* tsm, TParams* params, int shmid, FILE *f)
{
	pid_t pid = -1;

	if((pid = fork()) == 0)
	{/// VODIK
	    if((tsm = shmat(shmid, NULL, 0)) == (void *) -1)
        {// prirazeni segmentu sdilene pameti pro dany proces
            printECode(ESHMEM);
            return 2;
        }
        // generovani atomu vodiku
		if(generateAtoms(tsm, params, HYDROGEN, shmid, f) != EOK)
        {
            return 2;
        }
		return EOK;
	}
	else if(pid < 0)
	{// overeni uspesnosti funkce fork()
		printECode(EFORK);
		return 2;
	}

	if((pid = fork()) == 0)
	{/// KYSLIK
	    if((tsm = shmat(shmid, NULL, 0)) == (void *) -1)
        {// prirazeni segmentu sdilene pameti pro dany proces
            printECode(ESHMEM);
            return 2;
        }
        // generovani atomu kysliku
		if(generateAtoms(tsm, params, OXYGEN, shmid, f) != EOK)
        {
            return 2;
        }
		return EOK;
	}
	else if(pid < 0)
	{// overeni uspesnosti funkce fork()
		printECode(EFORK);
		return 2;
	}
    /////////////////////////////////////
    if(pid > 0)
    {// rodic ceka na svoje potomky (pomocne procesy)
        for(int i = 0; i < 2; i++)
        {
            pid_t child_pid;
            pid=wait(&child_pid);
            if(pid == -1)
            {
                printECode(EWAIT);
                return 2;
            }
        }
    }
    return EOK;
}


/**
 * Hlavni program.
 */
int main(int argc, char**argv)
{
    FILE *f;
    TParams params;
    /// zpracovani parametru prikazove radky
    params = getParams(argc, argv);
    if(params.error != EOK)
    {
        printECode(params.error);
        return 1;
    }

	TSharedMem *tsm = NULL;
	int shmid = -1;
    /// vytvoreni sdilene pameti
	if(createSharedMemory(&shmid, argv[0]) != EOK)
    {
        printECode(ESHMEM);
        freeSharedMemory(tsm, shmid);
        return 2;
    }
    /// inicializace sdilene pameti
	if(initSharedMemory(shmid, &tsm) != EOK)
    {
        printECode(ESHMEM);
        freeSharedMemory(tsm, shmid);
        return 2;
    }
	// inicializace generatoru pseudonahodnych cisel
	srand(time(NULL));
	f = fopen(FILENAME, "w");
	if(f == NULL)
    {// overeni uspesnosti otevreni souboru
        printECode(EFOPEN);
        freeSharedMemory(tsm, shmid);
        return 2;
    }
    // zakaz bufferovani
    setbuf(f, NULL);
    setbuf(stderr, NULL);
	/// generovani atomu
	if(createGenerators(tsm, &params, shmid, f) != EOK)
    {
        printECode(EFOPEN);
        freeSharedMemory(tsm, shmid);
        fclose(f);
        return 2;
    }
    /// uvolenni zdroju
	if(freeSharedMemory(tsm, shmid) != EOK)
    {
        printECode(EFOPEN);
        freeSharedMemory(tsm, shmid);
        fclose(f);
        return 2;
    }
	fclose(f);
    return EOK;
}
