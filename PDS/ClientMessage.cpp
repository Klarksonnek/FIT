#include "ClientMessage.h"

using namespace std;


DHCPInfo::DHCPInfo() :
	dhcpMessageType(0x00),
	dhcpServerIdentifier{0x00}
{
}

string DHCPInfo::toString(
	const string &separator) const
{
	string repr;

	repr += "messageType: ";
	repr += Util::intToHex(dhcpMessageType, "0x");
	repr += separator;

	repr += "identifier: ";
	for (auto i : dhcpServerIdentifier)
		repr += std::to_string(i) + ".";
	repr.pop_back();
	repr += separator;

	return repr;
}

DHCPDiscovery::DHCPDiscovery() :
	op1Type(0x35), // DHCP Message type
	op1Length(0x01),
	op1payload{0x01}, // Discover
	op4Type(0x37), // Parameter Request List
	op4Length(0x04),
	op4payload{0x01, 0x03, 0x06, 0x2a}, //subnet mask, router, dns, ntp server
	end(0xff)
{
	recalculateHeadersSize();
	ipHeader.headerChecksum = 0xd839;
}

void DHCPDiscovery::recalculateHeadersSize()
{
	ipHeader.totalLength[0] = uint8_t(l3Size() >> 8);
	ipHeader.totalLength[1] = uint8_t(l3Size() & 0xff);

	udpHeader.udpLength[0] = uint8_t(l4Size() >> 8);
	udpHeader.udpLength[1] = uint8_t(l4Size() & 0xff);
}

string DHCPDiscovery::toString(
	const string &separator) const
{
	string repr;

	repr += ethHeader.toString(separator);
	repr += ipHeader.toString(separator);
	repr += udpHeader.toString(separator);
	repr += dhcpHeader.toString(separator);
	repr += dhcpData.toString(separator);

	return repr;
}

void DHCPDiscovery::setEthMAC(
	const vector<uint8_t> &mac)
{
	for (size_t i = 0; i < 6; i++)
		ethHeader.srcMACAddr[i] = mac[i];
}

void DHCPDiscovery::setFakeClientMAC(
	const vector<uint8_t> &mac)
{
	for (size_t i = 0; i < 6; i++)
		dhcpData.clientHardwareAddress[i] = mac[i];
}

void DHCPDiscovery::setTransactionID(uint32_t id)
{
	dhcpHeader.transactionID = id;
}

uint16_t DHCPDiscovery::l3Size() const
{
	return sizeof(DHCPDiscovery)
	       - sizeof(TEthHeader);
}

uint16_t DHCPDiscovery::l4Size() const
{
	return sizeof(DHCPDiscovery)
	       - sizeof(TEthHeader)
	       - sizeof(TIP4Header);
}

DHCPRequest::DHCPRequest() :
	opType(0x35), // DHCP Message type
	opLength(0x01),
	opPayload{0x03}, // Request
	op1Type(0x32),
	op1Length(4),
	op1payload{0x00},
	op2Type(0x36),
	op2Length(4),
	op2payload{0x00},
	op3Type(0x3d),
	op3Length(7),
	op3payload{0x00},
	op3EthType(0x01),
	op4Type(0x37), // Parameter Request List
	op4Length(0x04),
	op4payload{0x01, 0x03, 0x06, 0x2a}, //subnet mask, router, dns, ntp server
	end(0xff)
{
	recalculateHeadersSize();
	ipHeader.headerChecksum = 0xc339;
}

void DHCPRequest::recalculateHeadersSize()
{
	ipHeader.totalLength[0] = uint8_t(l3Size() >> 8);
	ipHeader.totalLength[1] = uint8_t(l3Size() & 0xff);

	udpHeader.udpLength[0] = uint8_t(l4Size() >> 8);
	udpHeader.udpLength[1] = uint8_t(l4Size() & 0xff);
}

string DHCPRequest::toString(
	const string &separator) const
{
	string repr;

	repr += ethHeader.toString(separator);
	repr += ipHeader.toString(separator);
	repr += udpHeader.toString(separator);
	repr += dhcpHeader.toString(separator);
	repr += dhcpData.toString(separator);

	return repr;
}

void DHCPRequest::setEthMAC(
	const vector<uint8_t> &mac)
{
	for (size_t i = 0; i < 6; i++)
		ethHeader.srcMACAddr[i] = mac[i];
}

void DHCPRequest::setFakeClientMAC(
	const vector<uint8_t> &mac)
{
	for (size_t i = 0; i < 6; i++) {
		dhcpData.clientHardwareAddress[i] = mac[i];
		op3payload[i] = mac[i];
	}
}

void DHCPRequest::setTransactionID(uint32_t id)
{
	dhcpHeader.transactionID = id;
}

uint16_t DHCPRequest::l3Size() const
{
	return sizeof(DHCPRequest)
	       - sizeof(TEthHeader);
}

uint16_t DHCPRequest::l4Size() const
{
	return sizeof(DHCPRequest)
	       - sizeof(TEthHeader)
	       - sizeof(TIP4Header);
}
