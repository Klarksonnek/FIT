#pragma once

#include "Common.h"

/**
 * Informace o typu zpravy a dhcp serveru.
 */
struct DHCPInfo {
	uint8_t dhcpMessageType;
	uint8_t dhcpServerIdentifier[4];

	DHCPInfo();

	std::string toString(const std::string &separator) const;
};

/**
 * DHCP Discovery zprava.
 */
struct __attribute__((__packed__)) DHCPDiscovery {
	TEthHeader ethHeader;
	TIP4Header ipHeader;
	TUDPHeader udpHeader;
	TDHCPHeader dhcpHeader;
	TDHCPData dhcpData;

	// option: DHCP Message type
	uint8_t op1Type;
	uint8_t op1Length;
	uint8_t op1payload;

	// option: Parameter Request List
	uint8_t op4Type;
	uint8_t op4Length;
	uint8_t op4payload[4];

	uint8_t end;

	DHCPDiscovery();

	void recalculateHeadersSize();
	std::string toString(const std::string &separator) const;

	void setEthMAC(const std::vector<uint8_t> &mac);
	void setFakeClientMAC(const std::vector<uint8_t> &mac);
	void setTransactionID(uint32_t id);

	uint16_t l3Size() const;
	uint16_t l4Size() const;
};

/**
 * DHCP Request zprava.
 */
struct __attribute__((__packed__)) DHCPRequest {
	TEthHeader ethHeader;
	TIP4Header ipHeader;
	TUDPHeader udpHeader;
	TDHCPHeader dhcpHeader;
	TDHCPData dhcpData;

	// option: DHCP Message type
	uint8_t opType;
	uint8_t opLength;
	uint8_t opPayload;

	// option: Requested ip address
	uint8_t op1Type;
	uint8_t op1Length;
	uint8_t op1payload[4];

	// option: DHCP server identifier
	uint8_t op2Type;
	uint8_t op2Length;
	uint8_t op2payload[4];

	// client identifier - max
	uint8_t op3Type;
	uint8_t op3Length;
	uint8_t op3EthType;
	uint8_t op3payload[6];

	// option: Parameter Request List
	uint8_t op4Type;
	uint8_t op4Length;
	uint8_t op4payload[4];

	uint8_t end;

	DHCPRequest();

	void recalculateHeadersSize();
	std::string toString(const std::string &separator) const;

	void setEthMAC(const std::vector<uint8_t> &mac);
	void setFakeClientMAC(const std::vector<uint8_t> &mac);
	void setTransactionID(uint32_t id);

	uint16_t l3Size() const;
	uint16_t l4Size() const;
};
