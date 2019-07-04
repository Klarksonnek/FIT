
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                            Úpravy: Bohuslav Køena, øíjen 2015
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      DLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      DLInsertLast .... vlo¾ení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zru¹í první prvek seznamu,
**      DLDeleteLast .... zru¹í poslední prvek seznamu,
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem,
**      DLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      DLPred .......... posune aktivitu na pøedchozí prvek seznamu,
**      DLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
   L->First = NULL;
   L->Last = NULL;
   L->Act = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free.
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
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
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
** Prostøednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
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
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
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
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
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
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
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
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
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
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
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
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
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
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
    if(L->Act != NULL)
    {// overeni, zda je seznam aktivni
        // aktualizace aktivniho prvku seznamu
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
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
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
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
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním pøíkazem return.
**/
    return (L->Act != NULL ? TRUE : FALSE);
}

/* Konec c206.c*/
