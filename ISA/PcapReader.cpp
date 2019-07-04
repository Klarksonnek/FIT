/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor: 	PcapReader.cpp 
 * Autor: 	Klara Necasova
 * Email: 	xnecas24@stud.fit.vutbr.cz
 * Datum: 	listopad 2016
 */

#include "PcapReader.h"

/**
 * Ziska statistiky o poctu prenesenych bajtu.
 * @param value_1 Hodnota vyjadrujici soucet bajtu od vrstvy L2.
 * @param value_2 Hodnota vyjadrujici soucet bajtu od konce hlavicky dane vrstvy, hodnota zavisi na zvolenem filtru.
 * @param addr Hodnota filtru (adresa).
 * @param param Parametry prikazove radky.
 */
void PcapParser::editStatisticsValue(int value_1, int value_2, string addr, ProcessParams *param)
{
	if(param->statistics.empty())
	{// prazdne statistiky 
		// nova statistika 
		statisticsValues statisticValue;
	    statisticValue.value_1 = 0;
	    statisticValue.value_2 = 0;
	    statisticValue.addr = addr;
		
		// ulozeni nove statistiky 
	    param->statistics.push_back(statisticValue);
	}
	if(strcmp(addr.c_str(), param->statistics[0].addr.c_str()) == 0)
	{// pokud dana adresa odpovida adrese, kterou zadal uzivatel - inkrementujeme value_1 a value_2
		param->statistics[0].value_1 += value_1;
		param->statistics[0].value_2 += value_2;
		//cout << "addr " << param->statistics[0].addr.c_str() << " value1 " << param->statistics[0].value_1 << " value2 " << param->statistics[0].value_2 << endl;
	}
}

/**
 * Ziska statistiky o poctu prenesenych bajtu ve tvaru  adresa_hodnota1_hodnota2 (_ je znak mezery).
 * @param value_1 Hodnota vyjadrujici soucet bajtu od vrstvy L2.
 * @param value_2 Hodnota vyjadrujici soucet bajtu od konce hlavicky dane vrstvy, hodnota zavisi na zvolenem filtru.
 * @param addr Hodnota filtru (adresa).
 * @param param Parametry prikazove radky.
 */
void PcapParser::editStatisticsTop10(int value_1, int value_2, string addr, ProcessParams *param)
{
	vector<statisticsValues>::iterator it = param->statistics.begin();
	for(; it != param->statistics.end(); ++it)
	{
		if(strcmp(addr.c_str(), (*it).addr.c_str()) == 0)
		{// pokud dana adresa odpovida adrese, kterou zadal uzivatel - inkrementujeme value_1 a value_2
			(*it).value_1 += value_1;
			(*it).value_2 += value_2;
			//cout << (*it).addr.c_str() << " " << (*it).value_1 << " " << (*it).value_2 << endl;
			return;
		} 
	}
	// vytvoreni noveho zaznamu o statistice
	statisticsValues statisticValue;
	statisticValue.value_1 = value_1;
	statisticValue.value_2 = value_2;
	statisticValue.addr = addr;
	// ulozeni nove statistiky 
	param->statistics.push_back(statisticValue);
}

/**
 * Ziska potrebne statistiky na zaklade toho, zda se filtruji zdrojove/cilove adresy,
 * take se kontroluje, zda je/neni zadan filtr top10. 
 * @param value_1 Hodnota vyjadrujici soucet bajtu od vrstvy L2.
 * @param value_2 Hodnota vyjadrujici soucet bajtu od konce hlavicky dane vrstvy, hodnota zavisi na zvolenem filtru.
 * @param srcAddr Zdrojova adresa.
 * @param dstAddr Cilova adresa.
 * @param param Parametry prikazove radky.
 */
void PcapParser::editStatistics(int value_1, int value_2, string srcAddr, string dstAddr, ProcessParams *param)
{
	if(param->cSrc)
	{// filtr se aplikuje na zdrojove adresy 
	    if(strcmp(param->cFilterValue.c_str(), "top10") != 0)
	    {// nebyl zadan filtr top10
			if(strcmp(param->cFilterValue.c_str(), srcAddr.c_str()) == 0)
			{
				editStatisticsValue(value_1, value_2, srcAddr, param);	
			}	
	    }
	    else
	    {// byl zadan filtr top10
			editStatisticsTop10(value_1, value_2, srcAddr, param);
	    }
	}
	if(param->cDst)
	{// filtr se aplikuje na cilove adresy 
	    if(strcmp(param->cFilterValue.c_str(), "top10") != 0)
	    {// nebyl zadan filtr top10
			if(strcmp(param->cFilterValue.c_str(), dstAddr.c_str()) == 0)
			{
				if(param->cSrc && strcmp(dstAddr.c_str(), srcAddr.c_str()) == 0)
				{// cilova a zdrojova adresa se shoduje - pocet bajtu zapocitat 1x
					return;
				}
				editStatisticsValue(value_1, value_2, dstAddr, param);
			}
	    }
	    else
	    {// byl zadan filtr top10
			if(param->cSrc && strcmp(dstAddr.c_str(), srcAddr.c_str()) == 0)
			{// cilova a zdrojova adresa se shoduje - pocet bajtu zapocitat 1x
				return;
			}
			editStatisticsTop10(value_1, value_2, dstAddr, param);	
	    }
	}
}

/**
 * Porovna hodnoty dvou zaznamu o statistikach. 
 * @param recordA Prvni zaznam.
 * @param recordB Druhy zaznam.
 * @return Vraci true, pokud prvni zaznam obsahuje vetsi hodnotu (vice bajtu) nez druhy. 
 */
bool compareRecords(const statisticsValues &recordA, const statisticsValues &recordB)
{
	return (recordA.value_1 > recordB.value_1);
}

/**
 * Vytiskne statistiky. 
 * @param param Parametry prikazove radky.
 */
void PcapParser::printStatistics(ProcessParams *param)
{
	if(param->statistics.empty())
	{// pokud jsou statistiky prazdne - konec 
		if(strcmp(param->cFilterValue.c_str(), "top10") != 0)
		{
			cout << "0" << " " << "0" << endl;
		}
		return;
	}

	if(strcmp(param->cFilterValue.c_str(), "top10") != 0)
	{// vypis statistik
		cout << param->statistics[0].value_1 << " " << param->statistics[0].value_2 << endl;
	}
	else
	{// vypis statistik pro filtr top10
		// serazeni statistik 
		std::sort(param->statistics.begin(), param->statistics.end(), compareRecords);
		vector<statisticsValues>::iterator it = param->statistics.begin();
		int i = 0;
		for(; it != param->statistics.end(); ++it)
		{// vypis deseti statistik 
			if(i == TOP_10_VALUE)
			{
				break;
			}
			cout << param->statistics[i].addr << " " << param->statistics[i].value_1 << " " << param->statistics[i].value_2 << endl;
			i++;
		}
	}	
}

/**
 * Nacte a zpracuje soubor ve formatu pcap.
 * @param param Parametry prikazove radky.
 * @param file Nazev souboru.
 * @return Vraci EOK, pokud se soubor podarilo spravne nacist a zpracovat.
 */
int PcapParser::readPcapFile(ProcessParams *param, FILE *file)
{	
	int result = 0;
	int pos;
	int value_1 = 0;
	int value_2 = 0;
	int L4Length = 0;
	this->ethLength;
	int padding;
	
	// zjisteni velikosti souboru
	fseek(file, 0L, SEEK_END);
	long int size = ftell(file);

	fseek(file, 0, SEEK_SET);

	// preskoceni globalni hlavicky 
	pos = PCAP_GLOBAL_HEADER;
	fseek(file, pos, SEEK_CUR);
	long int curPos = ftell(file);

	while(1)
	{
		padding = 0;
		this->ethLength = ETH_HEADER_LENGTH;

		fseek(file,  curPos + value_1, SEEK_SET);
		if(curPos + value_1 >= size)
		{// osetreni posunu za konec souboru
			break;
		}
		
		// preskoceni casovych razitek, delky ramce		
		pos = PCAP_TIMESTAMP_SEC + PCAP_TIMESTAMP_MICROSEC + PCAP_FRAME_LENGTH;
		fseek(file, pos, SEEK_CUR);

		// nacteni skutecne delky paketu
		if(fread(&value_1, sizeof(uint32_t), 1, file) != 1)
		{
		    return EREAD;
		}
		//cout << value_1 << endl;	
		curPos = ftell(file);
		// -------------------------------------------------
		// zpracovani linkove vrstvy 
		result = processLinkLayer(file);
		if(result == IPv4)
		{// IPv4
			if(strcmp(param->cFilterType.c_str(), "ipv6") == 0)
		    {// ignorovani paketu - jiny protokol nez potrebujeme
		        pos = value_1 - this->ethLength;
		        fseek(file, pos, SEEK_CUR);
		        continue;
		    }
		    // zpracovani sitove vrstvy
		    result = processNetworkLayer(file);

		    if(this->length + this->ethLength < value_1) 
		    {// zjisteni hodnoty padding
				padding = value_1 - this->length - this->ethLength;
		    }
		    if(strcmp(param->cFilterType.c_str(), "ipv4") == 0)
		    {// filtr IPv4 
				// vypocet hodnoty value_2
				value_2 = value_1 - this->ethLength - this->IHL - padding;
				// uprava statistik 
				editStatistics(value_1, value_2, this->IPSrc.c_str(), this->IPDst.c_str(), param);
				pos = value_1 - this->ethLength - IPV4_HEADER_LENGTH;
				fseek(file, pos, SEEK_CUR);
				continue;
		    }
		    L4Length = value_1 - this->ethLength - this->IHL - padding;
		}
		// -------------------------------------------------
		if(result == IPv6)
		{// IPv6 
			// zpracovani sitove vrstvy
		    result = processNetworkLayer(file);
		    if((strcmp(param->cFilterType.c_str(), "ipv6") == 0 && result != TCP && result != UDP) || strcmp(param->cFilterType.c_str(), "ipv4") == 0)
		    {// ignorovani paketu - nepodporovany protokol na vyssi vrstve, nebo jiny protokol nez potrebujeme
		        pos = value_1 - this->ethLength - IPV6_HEADER_LENGTH;
		        fseek(file, pos, SEEK_CUR);
		        continue;
		    } 

		    if(strcmp(param->cFilterType.c_str(), "ipv6") == 0)
		    {// filtr IPv6
				// vypocet hodnoty value_2
				value_2 = this->length;
				// uprava statistik
				editStatistics(value_1, value_2, this->IPSrc.c_str(), this->IPDst.c_str(), param);
				pos = value_1 - this->ethLength - IPV6_HEADER_LENGTH;
				fseek(file, pos, SEEK_CUR);
				continue;
		    }		    
		    L4Length = this->length;
		}
		// -------------------------------------------------	 
		if(strcmp(param->cFilterType.c_str(), "mac") == 0)
	    {// MAC
			if(result == ARP || result == RARP)
			{// zpracovani ARP, RARP packetu
				// vypocet hodnoty value_2
				value_2 = ARP_HEADER_LENGTH;
				if(value_2 + this->ethLength < value_1) 
				{// vypocet hodnoty padding
					padding = value_1 - value_2 - this->ethLength;
					//cout << "padding:" << padding << endl;
				}
			}
			else
			{// na L3 neni ARP, RARP paket
				// vypocet hodnoty value_2
				value_2 = value_1 - this->ethLength - padding;
			}
			// uprava statistik 
			editStatistics(value_1, value_2, this->MACSrc.c_str(), this->MACDst.c_str(), param);
			if(result == IPv4)
			{
				pos = value_1 - this->ethLength - IPV4_HEADER_LENGTH;
			}
			else if(result == IPv6)
			{
				pos = value_1 - this->ethLength - IPV6_HEADER_LENGTH;
			}
			else if(result == ARP)
			{	
				pos = value_1 - this->ethLength;
			}
			else
			{// nepodporovany protokol na vyssi vrstve
				pos = value_1 - this->ethLength;
			}
			fseek(file, pos, SEEK_CUR);
			continue;
	    }
		// -------------------------------------------------
		if(result == UDP)
		{// UDP
			// zpracovani transportni vrstvy
		    processTransportLayer(file);
		    if(strcmp(param->cFilterType.c_str(), "udp") == 0)
		    {
				// vypocet hodnoty value_2
				value_2 = this->length - UDP_HEADER_LENGTH;
				// uprava statistik 
				editStatistics(value_1, value_2, this->srcPort, this->dstPort, param);	
		    }
		    pos = this->length - UDP_HEADER_LENGTH + padding;
		    fseek(file, pos, SEEK_CUR);
		    continue;
		}
		// -------------------------------------------------
		if(result == TCP)
		{// TCP
			// zpracovani transportni vrstvy
		    processTransportLayer(file);
		    if(strcmp(param->cFilterType.c_str(), "tcp") == 0)
		    {
				// vypocet hodnoty value_2
				value_2 = L4Length - this->dataOffset;
				// uprava statistik 
				editStatistics(value_1, value_2, this->srcPort, this->dstPort, param);
		    }
		    pos = L4Length - this->dataOffset + padding;
		    fseek(file, pos, SEEK_CUR);
		}	
	}
	return EOK;
}

/**
 * Zpracuje data na linkove vrstve.
 * @param file Nazev souboru.
 * @return Vraci typ protokolu na dalsi vrstve (L3). 
 */
int PcapParser::processLinkLayer(FILE *file)
{	
	unsigned char MACDst[7] = {0};
	unsigned char MACSrc[7] = {0};
	uint16_t payloadType;
	int pos;
	ProcessParams params;
	Converter converter;
	
	// nacteni cilove adresy MAC
	if(fread(&MACDst, 6*sizeof(unsigned char), 1, file) != 1)
	{
	    return EREAD;
	}
	MACDst[6] = '\0';
	// prevod retezce do hexadecimalni podoby
	this->MACDst = converter.strToHexStr(MACDst, 6);
	this->MACDst = params.normalizeMAC(this->MACDst);	
	
	// nacteni zdrojove adresy MAC
	if(fread(&MACSrc, 6*sizeof(unsigned char), 1, file) != 1)
	{
	    return EREAD;
	}
	MACSrc[6] = '\0';
	// prevod retezce do hexadecimalni podoby
	this->MACSrc = converter.strToHexStr(MACSrc, 6);
	this->MACSrc = params.normalizeMAC(this->MACSrc);

	// nacteni typu protokolu na vyssi vrstve (L3)
	if(fread(&payloadType, sizeof(uint16_t), 1, file) != 1)
	{
	    return EREAD;
	}
	this->payloadType = htons(payloadType);

	while(this->payloadType == ETH_802_1Q || this->payloadType == ETH_802_1Q_DOUBLE)
	{// vnorene VLAN hlavicky a Q-in-Q
		if(this->payloadType == ETH_802_1Q)
		{// VLAN hlavicka
			do{
				pos = ETH_VLAN_SEEK;
				// uprava delky hlavicky 
				this->ethLength += ETH_VLAN_TYPE;
				fseek(file, pos, SEEK_CUR);
				// nacteni typu protokolu na vyssi vrstve (L3)
				if(fread(&payloadType, sizeof(uint16_t), 1, file) != 1)
				{
				        return EREAD;
				}
				this->payloadType = htons(payloadType);
			}while(this->payloadType == ETH_802_1Q);
		}

		if(this->payloadType == ETH_802_1Q_DOUBLE)
		{// Q-in-Q
			do{
				pos = ETH_VLAN_DOUBLE_SEEK;
				// uprava delky hlavicky
				this->ethLength =+ ETH_VLAN_DOUBLE_TYPE;
				fseek(file, pos, SEEK_CUR);
				// nacteni typu protokolu na vyssi vrstve (L3)
				if(fread(&payloadType, sizeof(uint16_t), 1, file) != 1)
				{
				        return EREAD;
				}
				this->payloadType = htons(payloadType);
			}while(this->payloadType == ETH_802_1Q_DOUBLE);
		}
	}
	return this->payloadType;
}

/**
 * Zpracuje data na sitove vrstve.
 * @param file Nazev souboru.
 * @return Vraci typ protokolu na dalsi vrstve (L4). 
 */
int PcapParser::processNetworkLayer(FILE *file)
{	
	uint8_t IHL_version;
	uint32_t IPv4Src;
	uint32_t IPv4Dst;
	int pos;
	uint32_t version_TC_flowID;
	uint8_t payloadType;
	uint16_t length;
	ProcessParams params;
	Converter converter;
	
	if(this->payloadType == IPv4)
	{// IPv4
	    if(fread(&IHL_version, sizeof(uint8_t), 1, file) != 1)
	    {
			return EREAD;
	    }
		// verze protokolu 
	    this->version = (IHL_version & 0xF0) >> 4;
		
		// delka hlavicky - pro zisk skutecne delky je nutne ziskanou hodnotu vynasobit 4 
	    this->IHL = (IHL_version & 0x0F) * 4;
	    
	    pos = IPv4_DSCP_ECN;
	    fseek(file, pos, SEEK_CUR);

		// celkova delka datagramu v bytech
	    if(fread(&length, sizeof(uint16_t), 1, file) != 1)
	    {
			return EREAD;
	    }
	    this->length = htons(length);
	    pos = IPv4_IDENTIFICATION + IPv4_FLAGS_FRAGMENT_OFFSET + IPv4_TTL;
	    fseek(file, pos, SEEK_CUR);

		// nacteni typu protokolu na vyssi vrstve (L4)
	    if(fread(&payloadType, sizeof(uint8_t), 1, file) != 1)
	    {
			return EREAD;
	    }
	    this->payloadType = payloadType;

	    pos = IPv4_HEADER_CHECKSUM;
	    fseek(file, pos, SEEK_CUR);

		// nacteni zdrojove IPv4 adresy
	    if(fread(&IPv4Src, sizeof(uint32_t), 1, file) != 1)
	    {
			return EREAD;
	    }
	    this->IPSrc = converter.ipv4ToString(IPv4Src);
		this->IPSrc = params.normalizeIPv4(this->IPSrc);
	    
		// nacteni cilove IPv6 adresy
	    if(fread(&IPv4Dst, sizeof(uint32_t), 1, file) != 1)
	    {
			return EREAD;
	    }
	    this->IPDst = converter.ipv4ToString(IPv4Dst);
		this->IPDst = params.normalizeIPv4(this->IPDst);
	}
	// -------------------------------------------------
	if(this->payloadType == IPv6)
	{// IPv6
		// verze protokolu
	    if(fread(&version_TC_flowID, sizeof(uint32_t), 1, file) != 1)
	    {
			return EREAD;
	    }
	    this->version = (version_TC_flowID & 0xF0000000) >> 28;

	    // celkova delka datagramu v bytech 
	    if(fread(&(this->length), sizeof(uint16_t), 1, file) != 1)
	    {
			return EREAD;
	    }
	    this->length = htons(this->length);

		// nacteni typu protokolu na vyssi vrstve (L7)
	    if(fread(&payloadType, sizeof(uint8_t), 1, file) != 1)
	    {
			return EREAD;
	    }
		this->payloadType = payloadType;
	    fseek(file, IPv6_HOP_LIMIT, SEEK_CUR);

		// prevod zdrojove IPv6 adresy na retezec
	    this->IPSrc = converter.ipv6ToString(file);
		if(strcmp(this->IPSrc.c_str(), "") == 0)
		{
			return EREAD;
		}
		this->IPSrc = params.normalizeIPv6(this->IPSrc);

		// prevod cilove IPv6 adresy na retezec
	    this->IPDst = converter.ipv6ToString(file);
		this->IPDst = params.normalizeIPv6(this->IPDst);
	}
	return this->payloadType;	
}

/**
 * Zpracuje data na transportni vrstve.
 * @param file Nazev souboru.
 * @return Vraci EOK nebo prislusny chybovy kod. 
 */
int PcapParser::processTransportLayer(FILE *file)
{	
	uint16_t srcPortPom;
	uint16_t dstPortPom;
	uint8_t dataOffset;
	int pos;
	long curPos;
	Converter converter;
	
	if(this->payloadType == UDP)
	{// UDP
		// nacteni zdrojoveho portu
		if(fread(&srcPortPom, sizeof(uint16_t), 1, file) != 1)
		{
		    return EREAD;
		}
		srcPortPom = htons(srcPortPom);
		// prevod zdrojoveho portu na retezec
		this->srcPort = converter.uint16ToString(srcPortPom);
	
		// nacteni ciloveho portu
		if(fread(&dstPortPom, sizeof(uint16_t), 1, file) != 1)
		{
			return EREAD;
		}
		dstPortPom = htons(dstPortPom);
		// prevod ciloveho portu na retezec
		this->dstPort = converter.uint16ToString(dstPortPom);

		// nacteni delky segmentu
		if(fread(&(this->length), sizeof(uint16_t), 1, file) != 1)
		{
			return EREAD;
		}
		this->length = htons(this->length);
							
		fseek(file, UDP_CHECKSUM, SEEK_CUR);
	}
	// -------------------------------------------------
	if(this->payloadType == TCP)
	{// TCP
		// nacteni zdrojoveho portu
		if(fread(&srcPortPom, sizeof(uint16_t), 1, file) != 1)
		{
			return EREAD;
		}
		srcPortPom = htons(srcPortPom);
		// prevod zdrojoveho portu na retezec
		this->srcPort = converter.uint16ToString(srcPortPom);
	
		// nacteni ciloveho portu
		if(fread(&dstPortPom, sizeof(uint16_t), 1, file) != 1)
		{
			return EREAD;
		}
		dstPortPom = htons(dstPortPom);
		// prevod ciloveho portu na retezec
		this->dstPort = converter.uint16ToString(dstPortPom);
		
		pos = TCP_SEQ_NUM + TCP_ACK_NUM;
		fseek(file, pos, SEEK_CUR);
		// nacteni data offsetu
		if(fread(&(this->dataOffset), sizeof(uint8_t), 1, file) != 1)
		{
			return EREAD;
		}
		this->dataOffset = ((this->dataOffset & 0xF0) >> 4) * 4;
		pos = TCP_FLAGS + TCP_WINDOWS_SIZE + TCP_CHECKSUM;
		fseek(file, pos, SEEK_CUR);
	}
	return EOK;
}
