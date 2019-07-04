#include "Common.h"
#include <netinet/in.h>
#include "Util.h"

using namespace std;

string TEthHeader::toString(const string &separator) const
{
	string repr;

	repr += "destination MAC address: ";
	for (uint8_t i : dstMACAddr)
		repr += Util::intToHex(i) + ":";
	repr.pop_back();
	repr += separator;

	repr += "source MAC address: ";
	for (uint8_t i : srcMACAddr)
		repr += Util::intToHex(i) + ":";
	repr.pop_back();
	repr += separator;

	repr += "ethernet type: ";
	repr += Util::intToHex(ethType, "0x");
	repr += separator;

	return repr;
}

string TIP4Header::toString(const string &separator) const
{
	string repr;

	repr += "IP (4b) version and IHL (4b): ";
	repr += Util::intToHex(versionAndIHL, "0x");
	repr += separator;

	repr += "DSCP (6b) and ECN (2b): ";
	repr += Util::intToHex(DSCPAndECN, "0x");
	repr += separator;

	repr += "Total length: ";
	repr += to_string((totalLength[0] << 8) | totalLength[1]);
	repr += separator;

	repr += "Identification: ";
	repr += Util::intToHex(identification, "0x");
	repr += separator;

	repr += "Flags and fragmentation: ";
	repr += Util::intToHex(flagsAndFragmentOffset, "0x");
	repr += separator;

	repr += "TTL: ";
	repr += to_string(ttl);
	repr += separator;

	repr += "Protocol: ";
	repr += Util::intToHex(protocol, "0x");
	repr += separator;

	repr += "Header check sum: ";
	repr += Util::intToHex(headerChecksum, "0x");
	repr += separator;

	repr += "Dst IP addr: ";
	for (uint8_t i : dstIPAddr)
		repr += to_string(i) + ".";
	repr.pop_back();
	repr += separator;

	repr += "Src IP addr: ";
	for (uint8_t i : srcIPAddr)
		repr += to_string(i) + ".";
	repr.pop_back();
	repr += separator;

	return repr;
}

string TUDPHeader::toString(const string &separator) const
{
	string repr;

	repr += "Source port: ";
	repr += to_string(htons(udpSourcePort));
	repr += separator;

	repr += "Destination port: ";
	repr += to_string(htons(udpDestinationPort));
	repr += separator;

	repr += "Length: ";
	repr += to_string((udpLength[0] << 8) | udpLength[1]);
	repr += separator;

	repr += "Checksum port: ";
	repr += Util::intToHex(udpChecksum, "0x");
	repr += separator;

	return repr;
}

string TDHCPHeader::toString(const string &separator) const
{
	string repr;

	repr += "OpCode port: ";
	if (opCode == 1)
		repr += "request";
	else if (opCode == 2)
		repr += "reply";
	else
		repr += "unknown";
	repr += separator;

	repr += "Hardware type: ";
	if (hardwareType == 1)
		repr += "ethernet";
	else
		repr += "unknown";
	repr += separator;

	repr += "Hardware address length: ";
	repr += to_string(hardwareAddrLength);
	repr += separator;

	repr += "Hop count: ";
	repr += to_string(hopCount);
	repr += separator;

	repr += "Transaction ID: ";
	repr += Util::intToHex(htonl(transactionID), "0x");
	repr += separator;

	repr += "Number of seconds: ";
	repr += to_string(numberOfSeconds);
	repr += separator;

	repr += "Flags: ";
	repr += Util::intToHex(flags, "0x");
	repr += separator;

	repr += "Client IP addr: ";
	for (uint8_t i : clientIPAddr)
		repr += to_string(i) + ".";
	repr.pop_back();
	repr += separator;

	repr += "Your IP addr: ";
	for (uint8_t i : yourIPAddr)
		repr += to_string(i) + ".";
	repr.pop_back();
	repr += separator;

	repr += "Server IP addr: ";
	for (uint8_t i : serverIPAddr)
		repr += to_string(i) + ".";
	repr.pop_back();
	repr += separator;

	repr += "Gateway IP addr: ";
	for (uint8_t i : gatewayIPAddr)
		repr += to_string(i) + ".";
	repr.pop_back();
	repr += separator;

	return repr;
}

string TDHCPData::toString(const string &separator) const
{
	string repr;

	repr += "Client hardware address: ";
	for (uint8_t i : clientHardwareAddress)
		repr += Util::intToHex(i) + " ";
	repr += separator;

	repr += "Server host name: ";
	for (uint8_t i : serverHostName)
		repr += i;
	repr += separator;

	repr += "Boot filename: ";
	for (uint8_t i : bootFilename)
		repr += i;
	repr += separator;

	return repr;
}

