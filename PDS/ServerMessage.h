#pragma once

#include "Common.h"

/**
 * IP adresa.
 */
struct TIPv4 {
	int raw[4];

	static TIPv4 fromString(const std::string &ip, char separator = '.')
	{
		TIPv4 ipv4 = {0};
		size_t separatorCount = 0;
		std::string value;
		size_t index = 0;

		for (size_t i = 0; i < ip.size(); i++) {
			if (ip.at(i) != separator) {
				value.push_back(ip.at(i));

				if (i != ip.size()-1)
					continue;
			}

			separatorCount++;
			ipv4.raw[index++] = stoi(value, nullptr, 10);
			value.erase();
		}

		return ipv4;
	}

	std::string toString(const std::string &separator = "\n") const
	{
		std::string repr;

		for (auto it : raw)
			repr += std::to_string(it) + ".";
		repr.pop_back();
		repr += separator;

		return repr;
	}

	bool operator ==(const TIPv4 &ip) const
	{
		for (size_t i = 0; i < 4; i++) {
			if (raw[i] != ip.raw[i])
				return false;
		}

		return true;
	}

	bool operator !=(const TIPv4 &ip) const
	{
		for (size_t i = 0; i < 4; i++) {
			if (raw[i] != ip.raw[i])
				return true;
		}

		return false;
	}

	bool operator <(const TIPv4 &ip) const
	{
		for (size_t i = 0; i < 4; i++) {
			if (raw[i] < ip.raw[i])
				return true;
		}

		return false;
	}

	bool operator >(const TIPv4 &ip) const
	{
		for (size_t i = 0; i < 4; i++) {
			if (raw[i] > ip.raw[i])
				return true;
		}

		return false;
	}

	TIPv4& operator++ (int)
	{
		bool move = false;
		for (int i = 3; i >= 0; i--) {
			if (move) {
				move = false;

				if (raw[i] == 0xff) {
					raw[i] = 0;
					move = true;
					continue;
				}

				raw[i]++;
				continue;
			}

			if (raw[i] == 0xff) {
				raw[i] = 0;
				move = true;
				continue;
			}

			raw[i]++;
			break;
		}

		return *this;
	}
};

struct DHCPPayload {
	TIPv4 yourIPAddr;
	TIPv4 serverIPAddr;
	TIPv4 op36payload;
	TIPv4 op01payload;
	TIPv4 op1cpayload;
	TIPv4 op03payload;
	TIPv4 op06payload1;
	TIPv4 op06payload2;
};


/**
 * Informace o typu zpravy a dhcp serveru.
 */
struct DHCPServerInfo {
	uint8_t messageType;
	uint8_t clientHWAddress[16];
	uint8_t dstMACAddress[6];
	uint32_t transactionID;

	DHCPServerInfo():
		messageType(0x00),
		clientHWAddress{0x00},
		dstMACAddress{0x00},
		transactionID(0x00)
	{
	}

	std::string toString(const std::string &separator) const
	{
		std::string repr;

		repr += "messageType: ";
		repr += Util::intToHex(messageType, "0x");
		repr += separator;

		return repr;
	}
};


struct __attribute__((__packed__)) DHCPServerOffer {
	TEthHeader ethHeader;
	TIP4Header ipHeader;
	TUDPHeader udpHeader;
	TDHCPHeader dhcpHeader;
	TDHCPData dhcpData;

	// DHCP message type
	uint8_t op35Type;
	uint8_t op35Length;
	uint8_t op35payload;

	// DHCP server identifier
	uint8_t op36Type;
	uint8_t op36Length;
	uint8_t op36payload[4];

	// IP Address Lease time
	uint8_t op33Type;
	uint8_t op33Length;
	uint32_t op33payload;

	// subnet mask
	uint8_t op01Type;
	uint8_t op01Length;
	uint8_t op01payload[4];

	// broadcast address
	uint8_t op1cType;
	uint8_t op1cLength;
	uint8_t op1cpayload[4];

	// router
	uint8_t op03Type;
	uint8_t op03Length;
	uint8_t op03payload[4];

	// dns
	uint8_t op06Type;
	uint8_t op06Length;
	uint8_t op06payload[8];

	uint8_t end;

	DHCPServerOffer():
		op35Type(0x35),
		op35Length(0x01),
		op35payload(0x02),

		op36Type(0x36),
		op36Length(0x04),
		op36payload{0x00},

		op33Type(0x33),
		op33Length(0x04),
		op33payload(0x1234),

		op01Type(0x01),
		op01Length(0x04),
		op01payload{0x00},

		op1cType(0x1c),
		op1cLength(0x04),
		op1cpayload{0x00},

		op03Type(0x03),
		op03Length(0x04),
		op03payload{0x00},

		op06Type(0x06),
		op06Length(0x08),
		op06payload{0x00},

		end(0xff)
	{
		recalculareHeadersSize();
	}

	void recalculareHeadersSize()
	{
		ipHeader.totalLength[0] = uint8_t(l3Size() >> 8);
		ipHeader.totalLength[1] = uint8_t(l3Size() & 0xff);

		udpHeader.udpLength[0] = uint8_t(l4Size() >> 8);
		udpHeader.udpLength[1] = uint8_t(l4Size() & 0xff);
	}

	std::string toString(const std::string &separator) const
	{
		std::string repr;

		repr += ethHeader.toString(separator);
		repr += ipHeader.toString(separator);
		repr += udpHeader.toString(separator);
		repr += dhcpHeader.toString(separator);
		repr += dhcpData.toString(separator);

		return repr;
	}

	void setEthMAC(const std::vector<uint8_t> &mac)
	{
		for (size_t i = 0; i < 6; i++)
			ethHeader.srcMACAddr[i] = mac[i];
	}

	void setFakeClientMAC(const std::vector<uint8_t> &mac)
	{
		for (size_t i = 0; i < 6; i++)
			dhcpData.clientHardwareAddress[i] = mac[i];
	}

	void setTransactionID(uint32_t id)
	{
		dhcpHeader.transactionID = id;
	}

	uint16_t l3Size() const
	{
		return sizeof(DHCPServerOffer)
			   - sizeof(TEthHeader);
	}

	uint16_t l4Size() const
	{
		return sizeof(DHCPServerOffer)
			   - sizeof(TEthHeader)
			   - sizeof(TIP4Header);
	}

	static DHCPMessage* fromRaw(uint8_t *buffer, size_t size)
	{
		if (size < sizeof(DHCPMessage))
			throw std::invalid_argument("invalid size of structure");

		return (DHCPMessage *) buffer;
	}
};
