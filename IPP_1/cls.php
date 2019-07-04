<?php

#CLS:xnecas24
/*
 * Projekt: IPP - projekt, 1.cast - CLS: C++ Classes
 * Autor: Klara Necasova
 * Email: xnecas24@stud.fit.vutbr.cz
 * Datum: Brezen 2016
 */

/* Chybove navratove hodnoty */
// Vse v poradku
const EOK = 0;
// Spatny format parametru nebo zakazana kombinace parametru
const EPARAMS = 1;
// Neexistujici zadany soubor nebo chyba otevreni souboru pro cteni
const EINPUT_FILE = 2;
// Chyba pri pokusu o otevreni noveho nebo existujiciho souboru pro zapis
const EOUTPUT_FILE = 3;
// Chybny format vstupniho souboru
const EINPUT_FORMAT_FILE = 4;
// Konflikt mezi cleny z nekolika dedenych trid
const ECONFLICT = 21;

/* Stavy KA */
const S_CLASS = 0;
const S_CLASS_NAME_USING = 1;
const S_CLASS_NAME = 2;
const S_CLASS_NAME_INHERIT = 3;
const S_INHERIT = 4;
const S_USING = 5;
const S_METHOD_VAR_NAME_USING = 6;
const S_METHOD_VAR_SPEC_TYPE = 7;
const S_METHOD_VAR_TYPE = 8;
const S_METHOD_ARG_TYPE = 9;
const S_METHOD_ARG_NAME = 10;
const S_METHOD_ARG_END = 11;
const S_BODY = 12;
const S_CLASS_END = 13;

/* Trida reprezentujici parametry prikazoveho radku */
class Params
{
	public $inputFile = "";
	public $outputFile = "";
	public $pretty = -1;
	public $detailsClass = "";
	public $detailsDetected = 0;
	public $search = "";
};

/* Trida reprezentujici cely soubor */
class TFile
{
	public $classes = array();

	/**
	 * Prida tridu daneho jmena.
	 * @param $name Jmeno tridy.
	 */
	function addClass($name)
	{
		$this->classes[$name] = new TClass($name);
	}

}

/* Trida reprezentujici tridu v C++ */
class TClass
{
	// jmeno tridy
	public $name;
	// pole atributu
	public $attributes = array();
	// pole metod
	public $methods = array();
	// pole jmen trid, od kterych dana trida dedi, a jejich modifikatory pristupu
	public $parentsClasses = array();
	// pole jmen trid, od kterych dana trida dedi
	//public $inheritance = array();
	// pole jmen trid, ktere dedi od dane tridy
	public $childrens = array();
	// abstract (trida je abstraktni) x concrete (trida neni abstraktni)
	public $kind = "concrete";

	/**
	 * Konstruktor.
	 * @param $name Jmeno tridy.
	 */
	function __construct($name)
	{
		$this->name = $name;
	}

	/**
	 * Prida atribut se zjistenymi udaji do pole atributu.
	 * @param $modifier Modifikator.
	 * @param $dataType Datovy typ.
	 * @param $attrName Jmeno atributu.
	 * @param $specType Modifikator static.
	 * @param $className Jmeno tridy.
	 */
	function addAttr($modifier, $dataType, $attrName, $specType, $className)
	{
		$this->attributes[] = new TAttribute($modifier, $dataType, $attrName, $specType, $className);
	}

	/**
	 * Prida metodu se zjistenymi udaji do pole metod.
	 * @param $modifier Modifikator.
	 * @param $dataType Datovy typ.
	 * @param $attrName Jmeno metody.
	 * @param $specType Modifikator static.
	 * @param $className Jmeno tridy.
	 */
	function addMethod($modifier, $dataType, $methodName, $specType, $className)
	{
		$this->methods[] = new TMethod($modifier, $dataType, $methodName, $specType, $className);
	}

	/**
	 * Zjisti index atributu.
	 */
	function attrIndex()
	{
		end($this->attributes);
		return key($this->attributes);
	}

	/**
	 * Zjisti index metody.
	 */
	function methodIndex()
	{
		end($this->methods);
		return key($this->methods);
	}

	/**
	 * Ulozi udaje o tride, ze ktere dana trida dedi.
	 * @param $parent Jmeno tridy.
	 * @param $modifier Modifikator.
	 */
	private function setParentClass($parent, $modifier)
	{
		$this->parentsClasses[] = array("name" => $parent, "modifier" => $modifier);
	}

	/**
	 * Ulozi jmeno tridy, ktera od dane tridy dedi.
	 * @param $name Jmeno tridy. 
	 */
	function setChildren($name)
	{
		$this->childrens[] = $name;
	}

	/**
	 * Kopiruje atributy a metody do dane tridy.
	 * @param $source Trida.
	 * @param $modifier Modifikator.
	 */
	function copyClass($source, $modifier)
	{
		// ulozeni udaju o tride, ze ktere dana trida dedi
		$this->setParentClass($source->name, $modifier);
		//$this->inheritance[] = $source->name;

		// kopirovani atributu
		if(count($source->attributes))
		{
			foreach($source->attributes as $value)
			{// pruchod vsemi atributy
				array_push($this->attributes, clone $value);
				$index = $this->attrIndex();
				if($value->modifier == "private")
				{// private atributy nebudou vypsany
					$this->attributes[$index]->toPrint = 0;
				}
				// poznaceni, ze se jedna o zdedeny atribut
				$this->attributes[$index]->inherit = 1;
				// uprava modifikatoru pristupu dedeneho atributu
				if($modifier == "private")
				{
					$this->attributes[$index]->modifier = $modifier;
				}
				if($modifier == "protected")
				{
					if($this->attributes[$index]->modifier == "public")
					{
						$this->attributes[$index]->modifier = $modifier;
					}
				}
			}
		}

		// kopirovani metod
		if(count($source->methods))
		{
			foreach($source->methods as $value)
			{// pruchod vsemi metodami
				if($value->dataType == $value->className)
				{// konstruktor se nededi
					continue;
				}
				if(strpos($value->methodName, "~") !==  false)
				{// destruktor se nededi
					continue;
				}
				array_push($this->methods, clone $value);
				$index = $this->methodIndex();
				if($value->modifier == "private" && $value->isPure != 1)
				{// private metody, ktere nejsou ciste virtualni, se nevypisuji
					$this->methods[$index]->toPrint = 0;
				}
				// poznaceni, ze se jedna o zdedenou metodu
				$this->methods[$index]->inherit = 1;

				// uprava modifikatoru pristupu dedene metody
				if($modifier == "private")
				{
					$this->methods[$index]->modifier = $modifier;
				}
				if($modifier == "protected")
				{
					if($this->methods[$index]->modifier == "public")
					{
						$this->methods[$index]->modifier = $modifier;
					}
				}
			}
		}
	}

	/**
	 * Kontroluje, zda nedoslo k redefinici atributu.
	 * @param $attrName Jmeno atributu.
	 */
	function attrRedefinition($attrName)
	{
		foreach($this->attributes as $key => $value)
		{// pruchod vsemi atributy
			// kontrola nazvu a dedicnosti
			if($value->attrName == $attrName && $value->inherit)
			{// odstraneni zdedeneho atributu
				unset($this->attributes[$key]);
			}
		}
	}

	/**
	 * Kontroluje, zda nedoslo k redefinici metody.
	 * @param $methodName Jmeno metody.
	 */
	function methodRedefinition($methodName)
	{
		foreach($this->methods as $key => $value)
		{//pruchod vsemi metodami
			// kontrola nazvu a dedicnosti
			if($value->methodName == $methodName && $value->inherit)
			{// odstraneni zdedene metody
				unset($this->methods[$key]);
			}
		}
	}

	/**
	 * Kontroluje, zda nedoslo ke konfliktu.
	 */
	function checkConflict()
	{
		// kontrola konfliktu atributu
		foreach($this->attributes as $key => $value)
		{// pruchod vsemi atributy
			foreach($this->attributes as $key2 => $value2)
			{// kontrola jmen atributu
				if( $value->attrName == $value2->attrName && $key != $key2 )
				{
					return 1;
				}
			}
		}

		// kontrola konfliktu metod
		foreach($this->methods as $key => $value)
		{// pruchod vsemi metodami
			foreach($this->methods as $key2 => $value2)
			{// kontrola jmen metod
				if($value->methodName == $value2->methodName && $key != $key2) 
				{
					return 1;
				}
			}
		}

		// kontrola konfliktu atributu a metod
		foreach($this->attributes as $key => $value)
		{// pruchod vsemi atributy
			foreach($this->methods as $key2 => $value2)
			{// pruchod vsemi metodami
				// kontrola jmen atributu a metod
				if($value->attrName == $value2->methodName)
				{
					return 1;
				}
			}
		}
	}

	/**
	 * Prida atribut nebo metodu - using.
	 * @param $sourceClass Jmeno tridy.
	 * @param $name Jmeno atributu/metody.
	 * @param $modifier Modifikator. 
	 */
	function addUsing($sourceClass, $name, $modifier)
	{
		// pridani atributu
		foreach($this->attributes as $key => $value)
		{// pruchod vsemi atributy
			$value->modifier = $modifier;
			// kontrola jmen atributu a ruznosti trid
			if($value->attrName == $name && $value->className != $sourceClass)
			{// odstraneni shodneho atributu
				unset($this->attributes[$key]);
			}
		}

		// pridani metody
		foreach($this->methods as $key => $value)
		{// pruchod vsemi metodami
			$value->modifier = $modifier;
			//kontrola jmen metod a ruznosti trid
			if($value->methodName == $name && $value->className != $sourceClass)
			{// odstraneni shodne metody
				unset($this->methods[$key]);
			}
		}
	}

	/**
	 * Nastavi abstract.
	 */
	function checkAbstract()
	{
		foreach($this->methods as $key => $value)
		{// pruchod vsemi metodami
			if($value->isPure)
			{
				$this->kind = "abstract";
			}
		}
	}

	/**
	 * Spocita atributy s danym modifikatorem.
	 * @param $modifier Modifikator.
	 */
	function attrCount($modifier)
	{
		$count = 0;
		foreach($this->attributes as $attr)
		{// pruchod vsemi atributy
			if($attr->modifier == $modifier && $attr->toPrint)
			{
				$count++;
			}
		}
		return $count++;
	}

	/**
	 * Spocita metody s danym modifikatorem.
	 * @param $modifier Modifikator.
	 */
	function methodCount($modifier)
	{
		$count = 0;
		foreach($this->methods as $method)
		{// pruchod vsemi metodami
			if($method->modifier == $modifier && $method->toPrint)
			{
				$count++;
			}
		}
		return $count++;
	}
}


/* Trida reprezentujici atributy */
class TAttribute
{
	// modifikator pristupu (private/protected/public)
	public $modifier;
	// datovy typ
	public $dataType;
	// jmeno atributu
	public $attrName;
	// modifikator static
	public $staticModifier;
	// priznak dedicnosti (1/0)
	public $inherit;
	// jmeno tridy
	public $className;
	// priznak pro vypis
	public $toPrint;

	/**
	 * Konstruktor.
	 */
	function __construct($modifier, $dataType, $attrName, $specType, $className)
	{
		$this->modifier = $modifier;
		$this->dataType = $dataType;
		$this->attrName = $attrName;
		$this->staticModifier = $specType;
		$this->className = $className;
		$this->inherit = 0;
		$this->toPrint = 1;
	}

	/**
	 * Nastavi static x instance.´
	 */
	function getScope()
	{
		if(is_null($this->staticModifier) || empty($this->staticModifier))
		{
			return "instance";
		}
		return "static";
	}
}

/* Trida reprezentujici metody */
class TMethod
{
	// modifikator virtual
	public $virtualModifier;
	// ciste virtualni metoda (pure)
	public $isPure = 0;
	// modifikator pristupu (private/protected/public)
	public $modifier;
	// datovy typ
	public $dataType;
	// jmeno metody
	public $methodName;
	// modifikator static
	public $staticModifier;
	// pole argumentu
	public $args = array();
	// priznak dedicnosti (1/0)
	public $inherit;
	// jmeno tridy
	public $className;
	// priznak pro vypis
	public $toPrint;

	/**
	 * Konstruktor.
	 */
	function __construct($modifier, $dataType, $methodName, $specType, $className)
	{
		$this->modifier = $modifier;
		$this->dataType = $dataType;
		$this->methodName = $methodName;
		$this->className = $className;
		$this->inherit = 0;
		$this->toPrint = 1;

		if($specType == "static")
		{
			$this->staticModifier = $specType;
		}
		else
		{
			$this->virtualModifier = $specType;
		}
	}

	/**
	 * Nastavi priznak ciste virtualni metody.´
	 */
	function setPure()
	{
		$this->isPure = 1;
	}

	/**
	 * Prida argument se zjistenymi udaji do pole argumentu.
	 * @param $dataType Datovy typ.
	 * @param $argName Jmeno argumentu.
	 */
	function addArg($dataType, $argName)
	{
		if($dataType == "")
		{
			return;
		}
		$this->args[] = new TArgs($dataType, $argName);
	}

	/**
	 * Nastavi static x instance.´
	 */
	function getScope()
	{
		if(is_null($this->staticModifier) || empty($this->staticModifier))
		{
			return "instance";
		}
		return "static";
	}

	/**
	 * Nastavi pure.´
	 */
	function getVirtualPure()
	{
		if($this->isPure)
		{
			return "yes";
		}
		return "no";
	}
}

/* Trida reprezentujici argumenty */
class TArgs
{
	// nazev argumentu
	public $argName;
	// datovy typ
	public $dataType;

	/**
	 * Konstruktor.
	 */
	function __construct($dataType, $argName)
	{
		$this->dataType = $dataType;
		$this->argName = $argName;
	}
}

/*
 * Vypise chybovou hlasku a ukonci skript s danou navratovou hodnotou.
 */
function printECode($ERR)
{
	$errmsg = "";
	switch ($ERR)
	{
		case EOK:
			break;
		case EPARAMS:
			$errmsg = "Spatny format parametru nebo zakazana kombinace parametru.\n";
			break;
		case EINPUT_FILE:
			$errmsg = "Neexistujici zadany soubor nebo chyba otevreni souboru pro cteni.\n";
			break;
		case EOUTPUT_FILE:
			$errmsg = "Chyba pri pokusu o otevreni noveho nebo existujiciho souboru pro zapis.\n";
			break;
		case EINPUT_FORMAT_FILE:
			$errmsg = "Chybny format vstupniho souboru.\n";
			break;
		case ECONFLICT:
			$errmsg = "Konflikt mezi cleny z nekolika dedenych trid.\n";
			break;
		default:
			$errmsg = "Nepredvidana chyba.\n";
			break;
	}

	fprintf(STDERR, "$errmsg");
	exit($ERR);
}

/*
 * Vypise text napovedy.
 */
function printHelpMsg()
{
	echo "CLS: C++ Classes".
		"Autor: Klara Necasova (c) 2016\n".
		"Skript pro analyzu dedicnosti mezi tridami popsanymi zjednodusenou syntaxi pro validni hlavickove soubory programovaciho jazyka C++11. Skript vytvori strom dedicnosti mezi zadanymi tridami, pripadne vypise detaily o vsech clenech dane tridy.\n".
		"--help               Vypise na standardni vystup napovedu skriptu a skonci.\n".
		"--input=file         Vstupni textovy soubor file lze zadat relativni nebo absolutni cestou. Pokud nazev nebo cesta obsahuje mezeru, musi byt cesta se jmenem souboru v uvozovkach. Obsahuje popis trid jazyka C++. Chybi-li tento parametr, je uvazovan standardni vstup\n.".
		"--output=file        Vystupni soubor file ve formatu XML v kodovani UTF-8 lze zadat relativni nebo absolutni cestou. Pokud nazev nebo cesta obsahuje mezeru, musi byt cesta se jmenem souboru v uvozovkach. Chybi-li tento parametr, je vystup vypsan na standardni vystup. Existuje-li jiz vystupni soubor, bude bez varovani prepsan, jinak bude vytvoren novy soubor.\n".
		"--pretty-xml=k       Vystupni XML bude formatovano tak, ze kazde nove zanoreni bude odsazeno o k mezer oproti predchozimu. Neni-li k zadano, uvazuje se k = 4. Pokud tento parametr neni zadan, je formatovani vystupniho XML volne.\n".
		"--details=class      Na vystup se vypisuji udaje o clenech tridy jmenem class. Není-li argument class zadan, vypisuji se detaily o vsech tridach v danem souboru, kde korenem XML souboru je model. Pokud class neexistuje, bude na vystup vypsana pouze XML hlavicka.\n".
		"--search=XPATH       Ve vystupu skriptu se provede vyhledani urcitych elementu pomoci XPATH, coz je vyraz sepsany XPath syntaxi. Na vystup se vypise az vysledek tohoto vyhledavani (vcetne XML hlavicky).\n";
	exit(0);
}

/* Zpracovani parametru prikazove radky */
// urceni, zda byl zadan definovany prepinac
$argDetected = 0;

// objekt tridy Params pro ulozeni hodnot parametru
$params = new Params();
// pruchod parametry
for($i = 1; $i < $argc; $i++)
{
	if($argc == 1)
	{// neni zadan zadny parametr
		$params->inputFile = "php://stdin";
		$params->outputFile = "php://stdout";
	}

	if($argc == 2 && (($argv[$i] == "--help") || ($argv[$i] == "-h")))
	{// --help || -h
		printHelpMsg();
	}

	// vyhledame znak =
	$pos = strpos($argv[$i], "=");
	if($pos == 0)
	{// znak = se nenasel
		if($argv[$i] != "--pretty-xml" && $argv[$i] != "-p" && $argv[$i] != "--details" && $argv[$i] != "-d")
		{
			printECode(EPARAMS);
		}
		// zadan prepinac bez specifikace hodnoty, ulozeni nazvu prepinace
		$option = $argv[$i];
		$value = "";
	}
	else
	{   // ulozeni nazvu prepinace
		$option = substr($argv[$i], 0, $pos);
		// ulozeni hodnoty prepinace
		$value = substr($argv[$i], $pos + 1, strlen($argv[$i]));
		if($value == "")
		{
			printECode(EPARAMS);
		}
	}

	if($argc > 6)
	{// zadano prilis mnoho parametru
		printECode(EPARAMS);
	}

	// samotne zpracovani argumentu a ulozeni potrebnych hodnot
	if((($option == "--input") || ($option == "-i")) && ($params->inputFile == ""))
	{// --input || -i
		if(!is_readable($value))
		{// osetreni, zda soubor existuje a je citelny
			printECode(EINPUT_FILE);
		}
		$params->inputFile = $value;
		$argDetected = 1;
	}

	if((($option == "--output") || ($option == "-o")) && ($params->outputFile == ""))
	{// --output || -o
		if(file_exists($value))
		{// osetreni, zda soubor existuje
			if(is_writable($value))
			{// osetreni, zda je soubor zapisovatelny
				if(!($fw = fopen($value, "w")))
				{// osetreni uspesneho otevreni souboru pro zapis
					printECode(EOUTPUT_FILE);
				}
			}
			else
			{// soubor neni zapisovatelny
				printECode(EOUTPUT_FILE);
			}
		}
		else
		{// soubor neexistuje
			if(!($fw = fopen($value, "w")))
			{// osetreni uspesneho otevreni souboru pro zapis
				printECode(EOUTPUT_FILE);
			}
		}
		$params->outputFile = $value;
		$argDetected = 1;
	}

	if((($option == "--pretty-xml") || ($option == "-p")) && ($params->pretty == -1))
	{// --pretty-xml || -p
		if($value == "")
		{// pokud nebyl zadan pocet mezer, zvolen pocet 4
			$params->pretty = 4;
		}
		elseif(!ctype_digit($value))
		{// zda retezec obsahuje pouze cislice
			printECode(EPARAMS);
		}
		elseif($value >= 0)
		{
			$params->pretty = intval($value);
		}
		$argDetected = 1;
	}

	if((($option == "--details") || ($option == "-d")) && ($params->detailsClass == ""))
	{// --details || -d
		$params->detailsDetected = 1;
		$params->detailsClass = $value;
		$argDetected = 1;
	}

	if((($option == "--search") || ($option == "-s")) && ($params->search == ""))
	{// --search || -s
		$params->search = $value;
		$argDetected = 1;
	}

	if($argDetected === 1)
	{// v dane iteraci byl detekovan argument - ok
		$argDetected = 0;
	}
	else
	{
		printECode(EPARAMS);
	}
}

if($params->inputFile == "")
{// nastaveni vstupniho souboru - zadan aspon jeden parametr krome jmena souboru
	$params->inputFile = "php://stdin";
	$params->inputFile = "php://stdin";
}
if($params->outputFile == "")
{// nastaveni vystupniho souboru - zadan aspon jeden parametr krome jmena souboru
	$params->outputFile = "php://stdout";
}
if($params->pretty == -1)
{// nastaveni odsazeni - zadan aspon jeden parametr krome jmena souboru
	$params->pretty = 4;
}

/**
 * Zpracuje vstupni soubor a naplni dane struktury.
 * @param $input Obsah vstupniho souboru.  
*/
function parseInputFile($input)
{
	// nastaveni pocatecniho stavu
	$presentState = S_CLASS;
	// pro ulozeni udaju ze vstupniho souboru
	$inputContent = new TFile();
	$actClass = NULL;
	$argType = "";
	// implicitni hodnota modifikatoru pristupu u tridy, ze ktere dana trida dedi, je private
	$inheritModifier = "private";
	$value = "";
	$dataTypes = array("bool", "char", "int", "float", "double", "wchar_t", "unsigned char", "signed char", "unsigned int",
	"signed int", "short int", "unsigned short int", "signed short int", "long int", "signed long int", "unsigned long int", "long double");

	// konecny automat
	for($i = 0; $i < strlen($input); $i++)
	{
		if($presentState == S_CLASS)
		{// pocatecni stav
			if($input[$i] == " ")
			{// ignorovani mezer
				$presentState = S_CLASS;
			}
			else
			{
				// ignorovani klicoveho slova class (posun o 5 mist - jeste probehne jedna inkrementace)
				$i = $i + 5;
				$checkParent = 1;
				$modifTmp = "";
				$presentState = S_CLASS_NAME;
				$firstChar = 1;
				$constructor = 0;
				$destructor = 0; 
			}
		}
		elseif($presentState == S_CLASS_NAME)
		{// zpracovani jmena tridy
			if($input[$i] == "{")
			{
				$presentState = S_BODY;
				$className = $value;
				$inputContent->addClass($value);
				$actClass = $value;
				$value = "";
				$firstChar = 1;
			}
			elseif($input[$i] == ":")
			{
				$presentState = S_INHERIT;
				$className = $value;
				$inputContent->addClass($value);
				$actClass = $value;
				$value = "";
				$firstChar = 1;
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_") && $firstChar)
			{
				$firstChar = 0;
				$value .= $input[$i];
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || ctype_digit($input[$i])) && !$firstChar)
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_INHERIT)
		{// ulozeni modifikatoru pristupu tridy, ze ktere dana trida dedi
			if($input[$i] == " " && $value != "")
			{
				if($value == "private" || $value == "public" || $value == "protected")
				{
					$inheritModifier = $value;
					$value = "";
				}
				$presentState = S_CLASS_NAME_INHERIT;
				$firstChar = 1;
			}
			elseif($input[$i] == "," || $input[$i] == "{")
			{
				$presentState = S_CLASS_NAME_INHERIT;
				$i--;
				$firstChar = 1;
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_") && $firstChar)
			{
				$firstChar = 0;
				$value .= $input[$i];
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || ctype_digit($input[$i])) && !$firstChar)
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_CLASS_NAME_INHERIT)
		{
			if($input[$i] == ",")
			{
				if(!array_key_exists($value, $inputContent->classes))
				{
					printECode(EINPUT_FORMAT_FILE);
				}
				$presentState = S_INHERIT;
				// zkopirovani vsech hodnot ze tridy, od ktere dedime
				$inputContent->classes[$actClass]->copyClass($inputContent->classes[$value],$inheritModifier);
				// dana trida je potomkem tridy, od ktere dedi
				$inputContent->classes[$value]->setChildren($actClass);
				$inheritModifier = "private";
				$value = "";
			}
			elseif($input[$i] == "{")
			{
				if(!array_key_exists($value, $inputContent->classes))
				{
					printECode(EINPUT_FORMAT_FILE);
				}
				$presentState = S_BODY;
				// zkopirovani vsech hodnot ze tridy, od ktere dedime
				$inputContent->classes[$actClass]->copyClass($inputContent->classes[$value],$inheritModifier);
				// dana trida je potomkem tridy, od ktere dedi
				$inputContent->classes[$value]->setChildren($actClass);
				$inheritModifier = "private";
				$value = "";
				$firstChar = 1;
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_") && $firstChar)
			{
				$firstChar = 0;
				$value .= $input[$i];
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || ctype_digit($input[$i])) && !$firstChar)
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_BODY)
		{// zpracovani tela tridy
			if($checkParent)
			{
				$checkParent = 0;
			}
			if($input[$i] == "}")
			{// konec tridy
				$presentState = S_CLASS_END;
			}
			elseif((($input[$i] == " " || $input[$i] == ":") && ($value != "") && ($input[$i + 1] != "*") 
			&& ($input[$i + 1] != "&")) || ($input[$i - 1] == "*") || ($input[$i - 1] == "&"))
			{
				$specTypeTmp = "";
				if($value == "private" || $value == "public" || $value == "protected")
				{// ulozeni modifikatoru pristupu polozky tridy
					$modifTmp = $value;
					$presentState = S_BODY;
					$value = "";
				}
				elseif($value == "static" || $value == "virtual")
				{// ulozeni modifikatoru static nebo virtual
					$specTypeTmp = $value;
					if($modifTmp == "")
					{
						$modifTmp = "private";
					}
					$presentState = S_METHOD_VAR_SPEC_TYPE;
					$value = "";
				}
				elseif($value == "using")
				{// detekce using
					if($modifTmp == "")
					{
						$modifTmp = "private";
					}
					$presentState = S_CLASS_NAME_USING;
					$value = "";
					$firstChar = 1;
				}
				else
				{// ulozeni datoveho typu
					if($modifTmp == "")
					{
						$modifTmp = "private";
					}
					if($value == "unsigned" || $value == "signed" || $value == "short" || $value == "long" || $value ==  "unsigned short" || $value == "signed short" || $value == "signed long" || $value == "unsigned long")
					{
						$value .= " ";
						continue;
					}
					if($value == $className)
					{
						// detekce konstruktoru
						$constructor = 1;
					}
					$typeTmp = $value;
					if($input[$i - 1] == "&" || $input[$i - 1] == "*")
					{
						$i--;
					}
					$typeVerif = str_replace("*", "", $typeTmp);
					$typeVerif = str_replace("&", "", $typeVerif);
					$typeVerif = trim($typeVerif ,"\t\n\r\0\x0B");
					if(!array_key_exists($typeVerif, $inputContent->classes) && !in_array($typeVerif, $dataTypes))
					{// overeni, zda existuje dany datovy typ
						printECode(EINPUT_FORMAT_FILE);
					}
					$presentState = S_METHOD_VAR_TYPE;
					$value = "";
				}
			}
			elseif($input[$i] == "(" && !$destructor)
			{// detekce metody
				if($modifTmp == "")
				{
					$modifTmp = "private";
				}
				$specTypeTmp = "";
				$constructor = 1;
				$i--;
				$typeTmp = $value;
				$presentState = S_METHOD_VAR_TYPE;
			}
			elseif($input[$i] == "(")
			{// detekce metody
				if($modifTmp == "")
				{
					$modifTmp = "private";
				}
				$specTypeTmp = "";
				$i--;
				$typeTmp = $value;
				$presentState = S_METHOD_VAR_TYPE;
			}
			elseif(ctype_alpha($input[$i]) || $input[$i] == "_" || $input[$i] == "~" || $input[$i] == "*" || $input[$i] == "&")
			{
				if($input[$i] == "~")
				{// detekce destruktoru
					$destructor = 1;
				}
				$value .= $input[$i];
			}	
		}
		elseif($presentState == S_CLASS_NAME_USING)
		{// ulozeni nazvu tridy, ze ktere dana trida dedi - using
			if($input[$i] == ":")
			{
				$classUsingTmp = $value;
				$presentState = S_USING;
				$value = "";
				$firstChar = 1;
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_") && $firstChar)
			{
				$firstChar = 0;
				$value .= $input[$i];
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || ctype_digit($input[$i])) && !$firstChar)
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_USING)
		{// detekce : u using
			if($input[$i] == ":")
			{
				$presentState = S_METHOD_VAR_NAME_USING;
			}
		}
		elseif($presentState == S_METHOD_VAR_NAME_USING)
		{// zpracovani using - nazvu tridy, nazvu polozky
			if($input[$i] == ";")
			{
				$inputContent->classes[$actClass]->addUsing($classUsingTmp, $value, $modifTmp);
				$value = "";
				$presentState = S_BODY;
				$firstChar = 1;
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_") && $firstChar)
			{
				$firstChar = 0;
				$value .= $input[$i];
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || ctype_digit($input[$i])) && !$firstChar)
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_METHOD_VAR_SPEC_TYPE)
		{// ulozeni datoveho typu polozky
			if($input[$i] == " ")
			{
				$typeTmp = $value;
				$presentState = S_METHOD_VAR_TYPE;
				$value = "";
			}
			elseif(ctype_alpha($input[$i]))
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_METHOD_VAR_TYPE)
		{
			if($input[$i] == ";")
			{// zpracovani atributu a kontrola redefinice
				$inputContent->classes[$actClass]->addAttr($modifTmp, $typeTmp, $value, $specTypeTmp, $actClass);
				$inputContent->classes[$actClass]->attrRedefinition($value);
				$inputContent->classes[$actClass]->methodRedefinition($value);
				$presentState = S_BODY;
				$value = "";
				$firstChar = 1;
			}
			elseif($input[$i] == "(")
			{// zpracovani metody
				if($constructor)
				{
					$value = $typeTmp;
					$typeTmp = "";
					$constructor = 0;
				}
				if($destructor)
				{
					$value = $typeTmp;
					$typeTmp = "";
					$destructor = 0;
				}
				$inputContent->classes[$actClass]->addMethod($modifTmp, $typeTmp, $value, $specTypeTmp, $actClass);
				// ulozeni jmena promenne - vyuziti u kontroly redefinice
				$methodName = $value;
				$presentState = S_METHOD_ARG_TYPE;
				$value = "";
				$firstChar = 1;
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || $input[$i] == "~") && $firstChar)
			{
				$firstChar = 0;
				$value .= $input[$i];
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || ctype_digit($input[$i])) && !$firstChar)
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_METHOD_ARG_TYPE)
		{// ulozeni datoveho typu argumentu
			$index = $inputContent->classes[$actClass]->methodIndex();
			if(($input[$i] == " " && $value != "" && $input[$i + 1] != "*" && $input[$i + 1] != "&") || ($input[$i - 1] == "&") || ($input[$i - 1] == "*"))
			{
				$presentState = S_METHOD_ARG_NAME;
				$argType = $value;
				$value = "";
				$i--;
			}
			elseif($input[$i] == ")")
			{
				$presentState = S_METHOD_ARG_END;
				$argType = $value;
				$inputContent->classes[$actClass]->methods[$index]->addArg($value, NULL);
				$value = "";
			}
			elseif($input[$i] == ",")
			{
				$presentState = S_METHOD_ARG_TYPE;
				$argType = $value;
				$inputContent->classes[$actClass]->methods[$index]->addArg($value, NULL);
				$value = "";
			}
			elseif(ctype_alpha($input[$i]) || $input[$i] == "*" || $input[$i] == "&")
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_METHOD_ARG_NAME)
		{// zpracovani argumentu
			if($input[$i] == ")")
			{
				$presentState = S_METHOD_ARG_END;
				$inputContent->classes[$actClass]->methods[$index]->addArg($argType, $value);
				$value = "";
				$firstChar = 1;
			}
			elseif($input[$i] == ",")
			{
				$presentState = S_METHOD_ARG_TYPE;
				$inputContent->classes[$actClass]->methods[$index]->addArg($argType, $value);
				$value = "";
				$firstChar = 1;
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_") && $firstChar)
			{
				$firstChar = 0;
				$value .= $input[$i];
			}
			elseif((ctype_alpha($input[$i]) || $input[$i] == "_" || ctype_digit($input[$i])) && !$firstChar)
			{
				$value .= $input[$i];
			}
		}
		elseif($presentState == S_METHOD_ARG_END)
		{
			// kontrola redefinice metod
			$inputContent->classes[$actClass]->methodRedefinition($methodName);
			$inputContent->classes[$actClass]->attrRedefinition($methodName);
			if($input[$i] == ";" || $input[$i] == "}")
			{// navrat do zpracovani tela tridy
				$presentState = S_BODY;;
			}
			elseif($input[$i] == "{" || $input[$i] == "=")
			{
				$presentState = S_METHOD_ARG_END;
			}
			elseif($input[$i] == "0")
			{// nastaveni ciste virtualni tridy - pure
				$presentState = S_METHOD_ARG_END;
				$index =  $inputContent->classes[$actClass]->methodIndex();
				$inputContent->classes[$actClass]->methods[$index]->setPure();
			}
		}
		elseif($presentState == S_CLASS_END)
		{// konec tridy
			if($input[$i] == ";" || $input[$i] == "c" || $input[$i+1] == "c")
			{// kontrola, zda se nejedna o abstraktni tridu a kontrola konfliktu
				$presentState = S_CLASS;
				$inputContent->classes[$actClass]->checkAbstract();
				$result = $inputContent->classes[$actClass]->checkConflict();
				if($result == 1)
				{
					printECode(ECONFLICT);
				}
			}
		}
	}
	return $inputContent;
}

/**
 * Vygeneruje strom dedicnosti mezi tridami definovanymi na vstupu.
 * @param $inputContent Vstupni data.
 * @param $indent Odsazeni. 
 */
function generateInheritanceTreeXML($inputContent, $indent)
{
	$writer = new xmlWriter();
	$writer->openMemory();
	$writer->startDocument( "1.0", "UTF-8" );
	$writer->setIndent(1);
	// nastaveni odsazeni
	$writer->setIndentString(str_repeat(" ", $indent));
	$writer->startElement("model");

	foreach($inputContent->classes as $value)
	{// pruchod vsemi tridami
		if(!count($value->parentsClasses))
		{
			$writer->startElement("class");
			$writer->writeAttribute("name", $value->name);
			$writer->writeAttribute("kind", $value->kind);
			generateInheritanceTree($value->name, $inputContent, $writer);
			$writer->endElement();
		}
	}
	$writer->endDocument();
	$writer = $writer->flush();
	return $writer;
}

/**
 *  Vygeneruje popis vsech clenu zadane tridy.
 * @param $inputContent Vstupni data.
 * @param $className Jmeno tridy.
 * @param $indent Odsazeni.
 */
function generateDetailsXML($inputContent, $className, $indent)
{
	// overeni, zda existuje aspon jedna trida
	if(!count($inputContent->classes))
	{
		return;
	}

	$writer = new xmlWriter();
	$writer->openMemory();
	$writer->startDocument( "1.0", "UTF-8" );
	$writer->setIndent(1);
	// nastaveni odsazeni
	$writer->setIndentString(str_repeat(" ", $indent));

	if($className == "" && count($inputContent->classes) > 0)
	{
		$writer->startElement("model");
	}

	foreach($inputContent->classes as $name => $content)
	{// pruchod vsemi tridami
		// osetreni vygenerovani popisu jen zadane tridy
		if($className != "" && $className != $name)
		{
			continue;
		}

		// udaje o tride
		$writer->startElement("class");
		$writer->writeAttribute("name", $name);
		$writer->writeAttribute("kind", $content->kind);

		// udaje o dedicnosti
		if(count($content->parentsClasses))
		{
			$writer->startElement("inheritance");

			foreach($content->parentsClasses as $key => $value)
			{// pruchod vsemi tridami, od kterych dana trida dedi
				$writer->startElement("from");
				$writer->writeAttribute("name", $value["name"]);
				$writer->writeAttribute("privacy", $value["modifier"]);
				$writer->endElement();
			}
			$writer->endElement();
		}

		$attrDetected = 0;

		foreach(array("private", "protected", "public") as $modifier)
		{// pruchod vsemi modifikatory pristupu
			// osetreni, zda existuje nejaky atribut s danym modifikatorem pristupu
			if(!$content->methodCount($modifier) && !$content->attrCount($modifier))
			{
				continue;
			}
			
			$OnlyPrivate = 1;
			if($modifier == "private" && $content->attrCount($modifier))
			{
				foreach($content->attributes as $attr)
				{
					if(($attr->modifier != $modifier && !$attr->toPrint) || $attr->toPrint) 
					{// osetreni, aby se vypsaly atributy s chtenym modifikatorem pristupu a ne private atributy
						$OnlyPrivate = 0;
						break;
					}
				}
				if($OnlyPrivate)
				{
					continue;
				}
			}

			$attrDetected = 1;
			$writer->startElement($modifier);
			if($content->attrCount($modifier))
			{// vypis atributu
				$writer->startElement("attributes");
				foreach($content->attributes as $attr)
				{// pruchod vsemi atributy
					if($attr->modifier != $modifier || !$attr->toPrint) 
					{// osetreni, aby se vypsaly atributy s chtenym modifikatorem pristupu
						continue;
					}

					$writer->startElement("attribute");
					$writer->writeAttribute("name", $attr->attrName);
					$writer->writeAttribute("type", $attr->dataType);
					$writer->writeAttribute("scope", $attr->getScope());
					if($attr->className != $name)
					{
						$writer->startElement("from");
						$writer->writeAttribute("name", $attr->className);
						$writer->endElement();
					}
					$writer->endElement();
				}
				$writer->endElement();
			}

			// pruchod vsemi modifikatory pristupu
			// osetreni, zda existuje nejaka metoda s danym modifikatorem pristup
			if($content->methodCount($modifier))
			{// vypis metod
				$writer->startElement("methods");
				foreach($content->methods as $method)
				{// pruchod vsemi metodami
					if($method->modifier != $modifier || !$method->toPrint)
					{// osetreni, aby se vypsaly metody s chtenym modifikatorem pristupu a ne private metody
						continue;
					}
					$writer->startElement("method");
					$writer->writeAttribute("name", $method->methodName);
					$writer->writeAttribute("type", $method->dataType);
					$writer->writeAttribute("scope", $method->getScope());
					if($method->className != $name)
					{
						$writer->startElement("from");
						$writer->writeAttribute("name", $method->className);
						$writer->endElement();
					}
					// (ciste) virtualni metoda
					if($method->virtualModifier == "virtual")
					{
						$writer->startElement("virtual");
						$writer->writeAttribute("pure", $method->getVirtualPure());
						$writer->endElement();
					}

					$writer->startElement("arguments");
					foreach($method->args as $arg)
					{//pruchod vsemi argumenty
						// pokud je funkce void, tak nema zadne parametry
						if($arg->dataType == "void")
						{
							continue;
						}
						$writer->startElement("argument");
						$writer->writeAttribute("name", $arg->argName);
						$writer->writeAttribute("type", $arg->dataType);
						$writer->endElement();
					}
					$writer->endElement(); 
					$writer->endElement(); 
				}
				$writer->endElement(); 
			}
			$writer->endElement(); 
		}
		$writer->endElement(); 
	}
	$writer->endDocument();
	$writer = $writer->flush();

	return $writer;
}

/**
 *  Vygeneruje strom dedicnosti mezi tridami definovanymi na vstupu - pomocna funkce.
 * @param $name Jmeno tridy. 
 * @param $inputContent Vstupni data.
 * @param $writer Objekt trdy XMLWriter.
 */
function generateInheritanceTree($name, $inputContent, $writer)
{
	foreach($inputContent->classes[$name]->childrens as $value)
	{// pruchod vsemi tridami, ktere od dane tridy dedi
		$writer->startElement("class");
		$writer->writeAttribute("name", $value);
		$writer->writeAttribute("kind", $inputContent->classes[$value]->kind);
		// rekurzivni volani funkce
		generateInheritanceTree($value, $inputContent, $writer);
		$writer->endElement();
	}
}

/**
 * Vyhodnoceni XPath vyrazu.
 * @param $writer Objekt trdy XMLWriter.
 * @param $path Cesta. 
 */
function executeXPathExpression($writer, $path)
{
	// Prevod XML retezce na simpleXMLElement objekt 
	$tmp = simplexml_load_string($writer);
	// Prevod simpleXMLElement objektu na DOOMElement objekt
	$domTmp = dom_import_simplexml($tmp);

	// Ulozeni
	$dom = new DOMDocument('1.0', 'utf-8');
	$domTmp = $dom->importNode($domTmp, true);
	$domTmp = $dom->appendChild($domTmp);

	// vytvoreni noveho DOMXPath objektu 
	$xpath = new DOMXpath($dom);
	// vyhodnoceni XPath
	$elements = $xpath->query($path);
	
	return $elements;
}

// ulozeni obsahu vstupniho souboru do retezce
$input = file_get_contents($params->inputFile);
// odstraneni bilych znaku ze zacatku a z konce retezce
$input = trim($input, " \t\n\r\0\x0B");
// odstraneni bilych znaku uvnitr retezce
$input = preg_replace("/\s+/"," ", $input);

// zpracovani vstupniho souboru (retezce)
$inputContent = parseInputFile($input);

// generovani stromu dedicnosti mezi tridami
if($params->detailsClass == "" && !$params->detailsDetected)
{
	$writer = generateInheritanceTreeXML($inputContent, $params->pretty);
}
// generovani popisu clenu tridy
else
{
	$writer = generateDetailsXML($inputContent, $params->detailsClass, $params->pretty);
}

// generovani vystupu pro XPATH
if($params->search != "") 
{
	$attr = false;
	$elem = false;
	
	// zpracovani XPath vyrazu
	$elements = executeXPathExpression($writer, $params->search); 

	// vystupni dokument
	$domOutput = new DOMDocument('1.0', 'utf-8');
	$domOutput->formatOutput = true;
	$text = $domOutput;

	// vytvoreni tagu result
	if(($elements->length > 1) || ($elements->length > 0 && $elements->item(0) instanceof DOMAttr)) 
	{
		$root = $domOutput->createElement('result');
		$text = $domOutput->appendChild($root);

		$text->nodeValue = PHP_EOL;
	}

	// prevod vyhodnoceneho XPath vyrazu do xml
	foreach ($elements as $element) 
	{
		if ($element instanceof DOMAttr) 
		{
			$text->nodeValue = $text->textContent.str_repeat(" ", $params->pretty).$element->value.PHP_EOL;
			$attr = true;
		} 
		else if ($element instanceof DOMElement) 
		{
			$text->appendChild($domOutput->importNode($element, true));
			$elem = true;
		}
	}

	$document = new DOMDocument();
	$document->formatOutput = true;
	
	if(substr_count($domOutput->saveXML(), "\n") <= 1)
	{// pokud vyraz XPath neni nalezen
		file_put_contents($params->output_file, $domOutput->saveXML());
		return;
	}

	
	$writer = preg_replace("/(.?>)\s*/", "$1", $domOutput->saveXML());
	$document->loadXML($writer);

	// rozdeleni po radcich
	$lines = preg_split("/[\r\n]/", $document->saveXML());

	// generovani mezer (parametr pretty)
	$output = "";
	foreach($lines as $line) 
	{
		$spaces = strpos($line, "<");
		preg_replace("/\s+/", '', $line);        
		$output = $output.str_repeat(" ",($spaces/2 * $params->pretty));
		$output = $output.preg_replace("/\s*(.?<)/", "$1", $line).PHP_EOL;
		$spaces = 0;
	}

	// vypis xml
	if ($attr && $elem)
		$writer = $dom->saveXML();
	else
		$writer = $output;
}

// zapis do souboru
file_put_contents($params->outputFile, $writer);
printECode(EOK);
?>
