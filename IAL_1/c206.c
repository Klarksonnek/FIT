
/* c206.c **********************************************************}
{* T�ma: Dvousm�rn� v�zan� line�rn� seznam
**
**                   N�vrh a referen�n� implementace: Bohuslav K�ena, ��jen 2001
**                            P�epracovan� do jazyka C: Martin Tu�ek, ��jen 2004
**                                            �pravy: Bohuslav K�ena, ��jen 2015
**
** Implementujte abstraktn� datov� typ dvousm�rn� v�zan� line�rn� seznam.
** U�ite�n�m obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prom�nnou
** typu tDLList (DL znamen� Double-Linked a slou�� pro odli�en�
** jmen konstant, typ� a funkc� od jmen u jednosm�rn� v�zan�ho line�rn�ho
** seznamu). Definici konstant a typ� naleznete v hlavi�kov�m souboru c206.h.
**
** Va��m �kolem je implementovat n�sleduj�c� operace, kter� spolu
** s v��e uvedenou datovou ��st� abstrakce tvo�� abstraktn� datov� typ
** obousm�rn� v�zan� line�rn� seznam:
**
**      DLInitList ...... inicializace seznamu p�ed prvn�m pou�it�m,
**      DLDisposeList ... zru�en� v�ech prvk� seznamu,
**      DLInsertFirst ... vlo�en� prvku na za��tek seznamu,
**      DLInsertLast .... vlo�en� prvku na konec seznamu,
**      DLFirst ......... nastaven� aktivity na prvn� prvek,
**      DLLast .......... nastaven� aktivity na posledn� prvek,
**      DLCopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DLCopyLast ...... vrac� hodnotu posledn�ho prvku,
**      DLDeleteFirst ... zru�� prvn� prvek seznamu,
**      DLDeleteLast .... zru�� posledn� prvek seznamu,
**      DLPostDelete .... ru�� prvek za aktivn�m prvkem,
**      DLPreDelete ..... ru�� prvek p�ed aktivn�m prvkem,
**      DLPostInsert .... vlo�� nov� prvek za aktivn� prvek seznamu,
**      DLPreInsert ..... vlo�� nov� prvek p�ed aktivn� prvek seznamu,
**      DLCopy .......... vrac� hodnotu aktivn�ho prvku,
**      DLActualize ..... p�ep�e obsah aktivn�ho prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal�� prvek seznamu,
**      DLPred .......... posune aktivitu na p�edchoz� prvek seznamu,
**      DLActive ........ zji��uje aktivitu seznamu.
**
** P�i implementaci jednotliv�ch funkc� nevolejte ��dnou z funkc�
** implementovan�ch v r�mci tohoto p��kladu, nen�-li u funkce
** explicitn� uvedeno n�co jin�ho.
**
** Nemus�te o�et�ovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam
** p�ed� n�kdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodn� komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou��v� pojem procedura.
** Proto zde pou��v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozorn�n� na to, �e do�lo k chyb�.
** Tato funkce bude vol�na z n�kter�ch d�le implementovan�ch operac�.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* glob�ln� prom�nn� -- p��znak o�et�en� chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L p�ed jeho prvn�m pou�it�m (tzn. ��dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�d�t nad ji� inicializovan�m
** seznamem, a proto tuto mo�nost neo�et�ujte. V�dy p�edpokl�dejte,
** �e neinicializovan� prom�nn� maj� nedefinovanou hodnotu.
**/
   L->First = NULL;
   L->Last = NULL;
   L->Act = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zru�� v�echny prvky seznamu L a uvede seznam do stavu, v jak�m
** se nach�zel po inicializaci. Ru�en� prvky seznamu budou korektn�
** uvoln�ny vol�n�m operace free.
**/
   // pomocny ukazatel
   tDLElemPtr tmp;
  
   while(L->First != NULL)
   {// dokud neprojdeme cely seznam
       // ulozime si ukazatel na prvni prvek seznamu
       tmp = L->First;
       // aktualizujeme ukazatel na zacatek seznamu
       L->First = L->First->rptr;
       // odstranime prvek ze seznamu
       free(tmp);
   }
   // po skonceni cyklu je nastaven L->First na NULL, je nutne donastavit
   // obdobne L->Act a L->List
   L->Act = NULL;
   L->Last = NULL;
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vlo�� nov� prvek na za��tek seznamu L.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
    // novy prvek
    tDLElemPtr newItem;
    if((newItem = malloc(sizeof(struct tDLElem))) == NULL)
    {// overeni alokace pameti
        DLError();
        return;
    }
    // naplneni noveho prvku daty
    newItem->data = val;
    // navazani prvku do seznamu
    newItem->rptr = L->First;
    newItem->lptr = NULL;
    if(L->First != NULL)
    {// navazani prvku do seznamu
        L->First->lptr = newItem;
    }
    else
    {// pokud vkladame prvni prvek, aktualizujeme ukazatel na posledni prvek seznamu
        L->Last = newItem; 
    }
    // aktualizujeme ukazatel na zacatek seznamu
    L->First = newItem;
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vlo�� nov� prvek na konec seznamu L (symetrick� operace k DLInsertFirst).
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
    // novy prvek
    tDLElemPtr newItem;
    if((newItem = malloc(sizeof(struct tDLElem))) == NULL)
    {// overeni alokace pameti
        DLError();
        return;
    }
    // naplneni noveho prvku daty
    newItem->data = val;
    // navazani prvku do seznamu
    newItem->rptr = NULL;
    newItem->lptr = L->Last;
    if(L->Last != NULL)
    {// navazani prvku do seznamu
        L->Last->rptr = newItem;
    }
    else
    {// pokud vkladame prvni prvek, aktualizujeme ukazatel na prvni prvek seznamu
        L->First = newItem;
    }
    // aktualizujeme ukazatel na konec seznamu
    L->Last = newItem;
}

void DLFirst (tDLList *L) {
/*
** Nastav� aktivitu na prvn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastav� aktivitu na posledn� prvek seznamu L.
** Funkci implementujte jako jedin� p��kaz (nepo��t�me-li return),
** ani� byste testovali, zda je seznam L pr�zdn�.
**/
    L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/
    if(L->First == NULL)
    {// pokud je seznam prazdny - chyba
        DLError();
        return;
    }
    // zisk dat
    *val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu posledn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci DLError().
**/
    if(L->First == NULL)
    {// pokud je seznam prazdny - chyba
        DLError();
        return;
    }
    // zisk dat
    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zru�� prvn� prvek seznamu L. Pokud byl prvn� prvek aktivn�, aktivita
** se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/
    if(L->First != NULL)
    {// overeni, zda neni seznam prazdny
        // pomocny ukazatel
        tDLElemPtr tmp;
        // ulozime si ukazatel na prvni prvek
        tmp = L->First;
        if(L->First == L->Act)
        {// pokud je prvni prvek aktivni, tak se aktivita seznamu ztraci
            L->Act = NULL;
        }
        // v kazdem pripade je prvnim prvkem nasledujici
        L->First = L->First->rptr;
        if(L->First != NULL)
        {// predchazejici prvek bude odstranen, levy ukazatel prvniho prvku nastaven na NULL
             L->First->lptr = NULL;
        }
        else
        {// po odstraneni prvku bude seznam prazdny, aktualizace konce seznamu
            L->Last = NULL;
        }
        // zruseni prvniho prvku seznamu
        free(tmp);
    }
}

void DLDeleteLast (tDLList *L) {
/*
** Zru�� posledn� prvek seznamu L. Pokud byl posledn� prvek aktivn�,
** aktivita seznamu se ztr�c�. Pokud byl seznam L pr�zdn�, nic se ned�je.
**/
    if(L->First != NULL)
    {// overeni, zda neni seznam prazdny
        // pomocny ukazatel
        tDLElemPtr tmp;
        // ulozime si ukazatel na posledni prvek
        tmp = L->Last;
        if(L->Last == L->Act)
        {// pokud je posledni prvek aktivni, tak se aktivita seznamu ztraci
            L->Act = NULL;
        }
        // v kazdem pripade je poslednim prvkem predchazejici
        L->Last = L->Last->lptr;
        if(L->Last != NULL)
        {// nasledujici prvek bude odstranen, pravy ukazatel posledniho prvku nastaven na NULL
           L->Last->rptr = NULL;
        }
        else
        {// po odstraneni prvku bude seznam prazdny, aktualizace zacatku seznamu
            L->First = NULL;
        }
        // zruseni posledniho prvku seznamu
        free(tmp);
    }
}

void DLPostDelete (tDLList *L) {
/*
** Zru�� prvek seznamu L za aktivn�m prvkem.
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** posledn�m prvkem seznamu, nic se ned�je.
**/
    if((L->Act != NULL) && (L->Act != L->Last))
    {// overeni, zda je seznam L aktivni a test, zda neni aktivni posledni prvek seznamu
        // pomocny ukazatel
        tDLElemPtr tmp;
        // ulozime si ukazatel na odstranovany prvek
        tmp = L->Act->rptr;
        // premosteni (smer doprava) odstranovaneho prvku
        L->Act->rptr = tmp->rptr;
        if(L->Act->rptr != NULL)
        {// premosteni (smer doleva) odstranovaneho prvku
            tmp->rptr->lptr = L->Act;
        }
        else
        {// pokud je odstranovany prvek posledni, aktualizace ukazatele L->Last
            L->Last = L->Act;
        }
        // zruseni prvku seznamu
        free(tmp);
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zru�� prvek p�ed aktivn�m prvkem seznamu L .
** Pokud je seznam L neaktivn� nebo pokud je aktivn� prvek
** prvn�m prvkem seznamu, nic se ned�je.
**/
    if((L->Act != NULL) && (L->Act != L->First))
    {// overeni, zda je seznam L aktivni a test, zda neni aktivni prvni prvek seznamu
        // pomocny ukazatel
        tDLElemPtr tmp;
        // ulozime si ukazatel na odstranovany prvek
        tmp = L->Act->lptr;
        // premosteni (smer doleva) odstranovaneho prvku
        L->Act->lptr = tmp->lptr;
        if(L->Act->lptr != NULL)
        {// premosteni (smer doprava) odstranovaneho prvku
            tmp->lptr->rptr = L->Act;
        }
        else
        {// pokud je odstranovany prvek prvni, aktualizace ukazatele L->First
            L->First = L->Act;
        }
        // zruseni prvku seznamu
        free(tmp);
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo�� prvek za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
   if(L->Act != NULL)
   {// overeni, zda neni seznam prazdny
       // novy prvek
       tDLElemPtr newItem;
       if((newItem = malloc(sizeof(struct tDLElem))) == NULL)
       {// overeni alokace pameti
           DLError();
           return;
       }
       // naplneni noveho prvku daty
       newItem->data = val;
       // navazani prvku do seznamu za aktivni prvek
       newItem->lptr = L->Act;
       newItem->rptr = L->Act->rptr;
       L->Act->rptr = newItem;
       if(L->Act == L->Last)
       {// pokud je aktivni prvek poslednim prvkem, aktualizace L->Last
           L->Last = newItem;
       }
       else
       {// navazani prvku do seznamu za aktivni prvek
           newItem->rptr->lptr = newItem;      
       }
   }
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vlo�� prvek p�ed aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se ned�je.
** V p��pad�, �e nen� dostatek pam�ti pro nov� prvek p�i operaci malloc,
** vol� funkci DLError().
**/
   if(L->Act != NULL)
   {// overeni, zda neni seznam prazdny
       // novy prvek
       tDLElemPtr newItem;
       if((newItem = malloc(sizeof(struct tDLElem))) == NULL)
       {// overeni alokace pameti
           DLError();
           return;
       }
       // naplneni noveho prvku daty
       newItem->data = val; 
       // navazani prvku do seznamu pred aktivni prvek
       newItem->rptr = L->Act;
       newItem->lptr = L->Act->lptr;
       L->Act->lptr = newItem;
       if(L->Act == L->First)
       {// pokud je aktivni prvek prvnim prvkem, aktualizace L->First
           L->First = newItem;
       }
       else
       {// navazani prvku do seznamu pred aktivni prvek
           newItem->lptr->rptr = newItem;
       }
   }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prost�ednictv�m parametru val vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, vol� funkci DLError ().
**/
    if(L->Act == NULL)
    {// pokud seznam neni aktivni - chyba
        DLError();
        return;
    }
    *val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** P�ep�e obsah aktivn�ho prvku seznamu L.
** Pokud seznam L nen� aktivn�, ned�l� nic.
**/
    if(L->Act != NULL)
    {// overeni, zda je seznam aktivni
        // aktualizace aktivniho prvku seznamu
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na posledn�m prvku se seznam stane neaktivn�m.
**/
    if(L->Act != NULL)
    {// overeni, zda je seznam aktivni
        if(L->Act == L->Last)
        {// pokud je aktivni prvek poslednim prvkem, aktivita seznamu se ztraci
            L->Act = NULL;
        }
        else
        {// posun aktivity na nasledujici prvek seznamu
            L->Act = L->Act->rptr;
        }
    }
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na p�edchoz� prvek seznamu L.
** Nen�-li seznam aktivn�, ned�l� nic.
** V�imn�te si, �e p�i aktivit� na prvn�m prvku se seznam stane neaktivn�m.
**/
    if(L->Act != NULL)
    {// overeni, zda je seznam aktivni
        if(L->Act == L->First)
        {// pokud je aktivni prvek prvnim prvkem, aktivita seznamu se ztraci
            L->Act = NULL;
        }
        else
        {// posun aktivity na predchozi prvek seznamu
            L->Act = L->Act->lptr;
        }
    }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivn�, vrac� nenulovou hodnotu, jinak vrac� 0.
** Funkci je vhodn� implementovat jedn�m p��kazem return.
**/
    return (L->Act != NULL ? TRUE : FALSE);
}

/* Konec c206.c*/
