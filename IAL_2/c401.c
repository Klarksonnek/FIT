
/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvořil: Petr Přikryl, listopad 1994
**                                         Úpravy: Andrea Němcová, prosinec 1995
**                                                      Petr Přikryl, duben 1996
**                                                   Petr Přikryl, listopad 1997
**                                  Převod do jazyka C: Martin Tuček, říjen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                         Úpravy: Karel Masařík, říjen 2013
**                                         Úpravy: Radek Hranický, říjen 2014
**                                         Úpravy: Radek Hranický, listopad 2015
**
** Implementujte rekurzivním způsobem operace nad binárním vyhledávacím
** stromem (BVS; v angličtině BST - Binary Search Tree).
**
** Klíčem uzlu stromu je jeden znak (obecně jím může být cokoliv, podle
** čeho se vyhledává). Užitečným (vyhledávaným) obsahem je zde integer.
** Uzly s menším klíčem leží vlevo, uzly s větším klíčem leží ve stromu
** vpravo. Využijte dynamického přidělování paměti.
** Rekurzivním způsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíčem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zrušení uzlu se zadaným klíčem
**   BSTHeight .... výpočet výšky stromu
**   BSTDispose ... zrušení celého stromu
**
** ADT BVS je reprezentován kořenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíč (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). Přesnou definici typů 
** naleznete v souboru c401.h.
**
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede počáteční inicializaci stromu před jeho prvním použitím.
**
** Ověřit, zda byl již strom předaný přes RootPtr inicializován, nelze,
** protože před první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor využívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to před vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by totiž mohlo vést ke ztrátě přístupu
** k dynamicky alokované paměti (tzv. "memory leak").
**	
** Všimněte si, že se v hlavičce objevuje typ ukazatel na ukazatel.	
** Proto je třeba při přiřazení přes RootPtr použít dereferenční operátor *.
** Ten bude použit i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/
	
	*RootPtr = NULL;	
}	

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhledá uzel v BVS s klíčem K.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v proměnné Content se
** vrací obsah příslušného uzlu.´Pokud příslušný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah proměnné Content není definován (nic do ní proto
** nepřiřazujte).
**
** Při vyhledávání v binárním stromu bychom typicky použili cyklus ukončený
** testem dosažení listu nebo nalezení uzlu s klíčem K. V tomto případě ale
** problém řešte rekurzivním volání této funkce, přičemž nedeklarujte žádnou
** pomocnou funkci.
**/
							   
	if(RootPtr != NULL)
  {// overeni, zda neni strom prazdny
      if(RootPtr->Key == K)
      {// nalezen odpovidajici klic, vrati se obsah prislusneho uzlu
          *Content =  RootPtr->BSTNodeCont;
          return TRUE;
      }
      else
      {// nenalezen odpovidajici klic
          if(RootPtr->Key > K)
          {// prohledavani vlevo
              return BSTSearch(RootPtr->LPtr, K, Content);
          }
          else
          {// prohledavani vpravo
              return BSTSearch(RootPtr->RPtr, K, Content);
          }
      }
  }
  else
  {// neuspesne vyhledani
      return FALSE;
  }	
} 


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{	
/*   ---------
** Vloží do stromu RootPtr hodnotu Content s klíčem K.
**
** Pokud již uzel se zadaným klíčem ve stromu existuje, bude obsah uzlu
** s klíčem K nahrazen novou hodnotou. Pokud bude do stromu vložen nový
** uzel, bude vložen vždy jako list stromu.
**
** Funkci implementujte rekurzivně. Nedeklarujte žádnou pomocnou funkci.
**
** Rekurzivní implementace je méně efektivní, protože se při každém
** rekurzivním zanoření ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto případě byla efektivnější jak z hlediska
** rychlosti, tak z hlediska paměťových nároků. Zde jde ale o školní
** příklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/
  tBSTNodePtr newItem = *RootPtr;
	if(newItem == NULL)
  {// vytvoreni noveho uzlu - listova uroven
       // alokace pameti
       newItem = malloc(sizeof(struct tBSTNode));
       // naplneni uzlu klicem
       newItem->Key = K;
       // naplneni uzlu daty
       newItem->BSTNodeCont = Content;
       // inicializace leveho a praveho ukazatele
       newItem->LPtr = NULL;
       newItem->RPtr = NULL;
       // zapojeni uzlu do stromu
       *RootPtr = newItem;  
  }
  else
  {// hledani vhodneho mista pro nove vytvoreny uzel
      if(newItem->Key > K)
      {// prohledavani vlevo
          BSTInsert(&newItem->LPtr, K, Content);
      }
      else if(newItem->Key < K)
      {// prohledavani vpravo
          BSTInsert(&newItem->RPtr, K, Content);
      }
      else
      {// aktualizujeme obsah uzlu (uzel s danym klicem jiz existuje)
          newItem->BSTNodeCont = Content;
      }
  }	
}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude přesunuta hodnota
** nejpravějšího uzlu v podstromu, který je určen ukazatelem RootPtr.
** Předpokládá se, že hodnota ukazatele RootPtr nebude NULL (zajistěte to
** testováním před volání této funkce). Tuto funkci implementujte rekurzivně. 
**
** Tato pomocná funkce bude použita dále. Než ji začnete implementovat,
** přečtěte si komentář k funkci BSTDelete(). 
**/
  if(*RootPtr != NULL)
  {// overeni, zda neni strom prazdny
      tBSTNodePtr tmp = *RootPtr;
      if(tmp->RPtr != NULL)
      {// nalezeni nejpravejsiho uzlu
          ReplaceByRightmost(PtrReplaced, &tmp->RPtr);
      }
      else
      {// nejpravejsi uzel nalezen - presuneme klic a jeho obsah do uzlu, kam ukazuje PtrReplaced
          PtrReplaced->Key = tmp->Key;
          PtrReplaced->BSTNodeCont = tmp->BSTNodeCont;
          *RootPtr = (*RootPtr)->LPtr;
          // uvolneni uzlu
          free(tmp);                              
      }
  }	
}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zruší uzel stromu, který obsahuje klíč K.
**
** Pokud uzel se zadaným klíčem neexistuje, nedělá funkce nic. 
** Pokud má rušený uzel jen jeden podstrom, pak jej zdědí otec rušeného uzlu.
** Pokud má rušený uzel oba podstromy, pak je rušený uzel nahrazen nejpravějším
** uzlem levého podstromu. Pozor! Nejpravější uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivně s využitím dříve deklarované
** pomocné funkce ReplaceByRightmost.
**/
	if(*RootPtr != NULL)
  {// osetreni, zda neni strom prazdny
      tBSTNodePtr tmp = *RootPtr;
      if(tmp->Key > K)
      {// prohledavani vlevo
          BSTDelete(&tmp->LPtr, K);
      }
      else if(tmp->Key < K)
      {// prohledavani vpravo
          BSTDelete(&tmp->RPtr, K);
      }
      else
      {
          // odstraneni nalezeneho prvku
          if(tmp->LPtr == NULL)
          {// pravy podstrom
              *RootPtr = tmp->RPtr;
              free(tmp);
          }
          else if(tmp->RPtr == NULL)
          {// levy podstrom
              *RootPtr = tmp->LPtr;
              free(tmp);
          }
          else
          {// hledani nejpravejsiho uzlu
              ReplaceByRightmost(tmp, &tmp->LPtr);
          }   
      }    
  }	
} 

int BSTHeight (tBSTNodePtr NodePtr, bool IsRoot) {	
/*   ----------
** Vypočítá výšku BVS. Výška stromu délka (počet hran) nejdelší cesty
** od kořene k listu. Vzhledem k rekurzivní implementaci je třeba rozlišit,
** zda funkci voláme pro samotný kořen stromu nebo rekurzivně pro některý
** z ostatních uzlů.
** Výpočet výšky stromu se tedy bude provádět voláním:
**   BSTHeight(ukazatel_na_kořen, TRUE)
**
** Návratová hodnota je výška stromu. Výška prázdného stromu však není definována.
** V případě prázdného stromu bude funkce vracet hodnotu -1. 
** 
** Tuto funkci implementujte bez deklarování pomocné funkce.
**/
  // vyska prazdneho stromu neni definovana
	if(IsRoot == TRUE && NodePtr == NULL)
    return -1;
  // ukoncovaci podminka rekuze
  if(NodePtr == NULL)
    return 0;
     
  // rekurzivni zjisteni vysky leveho a praveho podstromu
  int heightLeft = BSTHeight(NodePtr->LPtr, FALSE) + 1;
  int heightRight = BSTHeight(NodePtr->RPtr, FALSE) + 1;

  if(IsRoot)
  {// korekce vysky pro korenovy uzel stromu
      heightLeft--;
      heightRight--;
  } 
  
  // vratime vetsi z nalezenych vysek
  if(heightLeft < heightRight)
  {
      return heightRight;
  }
  else
  {
      return heightLeft;
  }
}

void BSTDispose (tBSTNodePtr *RootPtr) {	
/*   ----------
** Zruší celý binární vyhledávací strom a korektně uvolní paměť.
**
** Po zrušení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivně bez deklarování pomocné
** funkce.
**/
  if(*RootPtr != NULL)
  {// overeni, zda neni strom prazdny
      tBSTNodePtr tmp = *RootPtr;
      // levy pruchod stromem
      BSTDispose(&tmp->LPtr);
      // pravy pruchod stromem
      BSTDispose(&tmp->RPtr);
      // uvolneni uzlu
      free(tmp);
      // nastaveni stromu do stavu po inicializaci
      *RootPtr = NULL;
  }  
}

/* konec c401.c */

