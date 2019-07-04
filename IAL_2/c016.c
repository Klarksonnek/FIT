
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                      Úpravy: Radek Hranický, říjen 2014
**                      Úpravy: Radek Hranický, listopad 2015
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
   if(ptrht != NULL)
   {
	    for(int i = 0; i < HTSIZE; i++)
	    { // inicializujeme jednotlive polozky tabulky
			    (*ptrht)[i] = NULL;
			}
	 }
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
  if(ptrht == NULL)
   {// pokud je tabulka prazdna, vracime NULL 
       return NULL;
	 }
	 // zjisteni indexu do tabulky pomoci hashovaci funkce
   int index = hashCode(key);
   tHTItem *item = (*ptrht)[index];
   while((item != NULL) && (item->key != key))
   {// vyhledavani v tabulce - prohledavame linearni seznam dany indexem index
    // prohledavani konci, pokud nalezneme polozku s danym klicem nebo pokud
    // dosahneme konce seznamu
       item = item->ptrnext;
	 }
	 return item;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
    if(ptrht == NULL)
    {// pokud je tabulka prazdna, nic se nedeje
		    return;
		}
    // pokusime se vyhledat polozku dle klice
    tHTItem* tmp = htSearch(ptrht, key);
    
    if(tmp == NULL)
    {// polozka v tabulce neni - vytvorime novou
		    if((tmp = malloc(sizeof(tHTItem))) != NULL)
		    {// osetreni zda byla alokace pameti uspesna
				    tmp->key = key;
				    tmp->data = data;
				    int index = hashCode(key);
				    // zapojeni polozky na zacatek seznamu
				    tmp->ptrnext = (*ptrht)[index];
				    (*ptrht)[index] = tmp;
				}
				else
				{
				    return;
				}
		}
		else
		{// polozka se v tabulce jiz nachazi - aktualizujeme jeji data 
		    tmp->data = data;
		}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
    if(ptrht == NULL)
    {// pokud je tabulka prazdna, nic se nedeje
		    return NULL;
		}
		// pokusime se vyhledat polozku dle klice
    tHTItem* tmp = htSearch(ptrht, key);
    if(tmp == NULL)
    {// pokud polozka nebyla nalezena, vratime NULL
		    return NULL;
		}
		else
		{// pokud polozka byla nalezena, vratime jeji datovou cast
		   return &tmp->data;
		}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
    if(ptrht == NULL)
    {// pokud je tabulka prazdna, nic se nedeje
		    return;
		}
		// nalezeni indexu do tabulky pomoci mapovaci funkce
		int index = hashCode(key);
		// ukazatel na predchozi polozku v linearnim seznamu
		tHTItem *prev;
    tHTItem *item = (*ptrht)[index];
		
    if(item == NULL)
    {// osetreni, zda seznam synonym neni prazdny
        return;
    }
    if(item->key == key)
    {// pokud se klice rovnaji, aktualizujeme linearni seznam a odstranime polozku
         (*ptrht)[index] = item->ptrnext;
         free(item);
         return;
    }
    
		while(item != NULL && item->key != key)
		{// prochazime linearni seznam na danem indexu, dokud neni nalezen klic nebo 
		 // dokud nejsme na konci linearniho seznamu
		    // ulozeni predchoziho prvku a posun na dalsi
		    prev = item;
		    item = item->ptrnext;
		}
		if(item != NULL)
		{// pokud je polozka s klice nalezena, premostime ruseny prvek a zrusime ji
		    prev->ptrnext = item->ptrnext;
		    free(item);
		}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
   // ukazatel na predchozi polozku v linearnim seznamu
	 tHTItem *prev;
   
	 if(ptrht == NULL)
    {// pokud je tabulka prazdna, nic se nedeje
		    return;
		}
   for(int i = 0; i < HTSIZE; i++)
   {// prochazime jednotlive indexy tabulky
	     while((*ptrht)[i] != NULL)
	     {// na kazdem indexu prochazime linearni seznam a rusime jeho polozky
	      // (zleva doprava)
			     prev = (*ptrht)[i];
					 (*ptrht)[i] = (*ptrht)[i]->ptrnext;
					 free(prev); 
			 }
	 }
}
