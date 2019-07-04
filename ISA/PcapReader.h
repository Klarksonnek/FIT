/**
 * Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
 * Soubor: PcapReader.h 
 * Autor: Klara Necasova
 * Email: xnecas24@stud.fit.vutbr.cz
 * Datum: listopad 2016
 */

#ifndef PCAPREADER_H  
#define PCAPREADER_H

#include <algorithm>
#include <iomanip>
#include "global.h"
#include "ProcessParams.h"
#include "Converter.h"

#define PCAP_GLOBAL_HEADER		24
#define TOP_10_VALUE			10

#define PCAP_TIMESTAMP_SEC		4
#define PCAP_TIMESTAMP_MICROSEC	4
#define PCAP_FRAME_LENGTH		4
#define PCAP_CAPTURE_LENGTH		4

#define ETH_VLAN_TYPE			4
#define ETH_VLAN_DOUBLE_TYPE	8
#define ETH_VLAN_SEEK			2
#define ETH_VLAN_DOUBLE_SEEK	6

#define IPv4_DSCP_ECN				1
#define IPv4_IDENTIFICATION			2
#define IPv4_FLAGS_FRAGMENT_OFFSET	2
#define IPv4_TTL					1
#define IPv4_HEADER_CHECKSUM		2

#define IPv6_HOP_LIMIT				1

#define UDP_CHECKSUM			2

#define TCP_SEQ_NUM				4
#define TCP_ACK_NUM				4
#define TCP_FLAGS				1
#define TCP_WINDOWS_SIZE		2
#define TCP_CHECKSUM			2
#define TCP_REQUIRED_LENGTH		18

#define ETH_802_1Q			0x8100
#define ETH_802_1Q_DOUBLE	0x88A8
#define IPv4				0x0800
#define IPv6				0x86DD
#define ARP					0x0806
#define RARP				0x8035
#define UDP					0x11
#define TCP					0x06

#define ETH_HEADER_LENGTH		14
#define IPV4_HEADER_LENGTH		20
#define IPV6_HEADER_LENGTH		40
#define ARP_HEADER_LENGTH		28
#define UDP_HEADER_LENGTH		8

/**
 * Trida PcapParser zajistujici zpracovani pcap souboru.
 */
class PcapParser
{
    public:	
		int readPcapFile(ProcessParams *param, FILE *file);
		int processLinkLayer(FILE *file);
		int processNetworkLayer(FILE *file);
		int processTransportLayer(FILE *file);
		string ipv4ToString(uint32_t ipAddress);
		string ipv6ToString(FILE *file);
		void editStatistics(int value_1, int value_2,  string srcAddr,  string dstAddr, ProcessParams *param);
		void editStatisticsValue(int value_1, int value_2, string addr, ProcessParams *param);
		void editStatisticsTop10(int value_1, int value_2, string addr, ProcessParams *param);
		void printStatistics(ProcessParams *param);
    private:
		string MACDst;
		string MACSrc;
		int payloadType;
		string IPDst;
		string IPSrc;
		string srcPort;
		string dstPort;
		int IHL;
		int version;
		uint16_t length;
		uint8_t dataOffset;
		int ethLength;
		uint8_t hwAddrLength;
		uint8_t protocolAddrLength;
		int L4Length;
};

#endif 
