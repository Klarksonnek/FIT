#include <string>
#include <getopt.h>
#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include "Util.h"
#include "ServerMessage.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <time.h>

#include <pcap.h>
#include <vector>

#include <poll.h>
#include <map>

using namespace std;

struct TMAC {
	uint8_t raw[6];

	static TMAC fromArray(uint8_t *buffer)
	{
		TMAC mac;
		for (size_t i = 0; i < 6; i++)
			mac.raw[i] = buffer[i];

		return mac;
	}

	bool operator ==(const TMAC &mac) const
	{
		for (size_t i = 0; i < 6; i++) {
			if (raw[i] != mac.raw[i])
				return false;
		}

		return true;
	}

	bool operator !=(const TMAC &mac) const
	{
		for (size_t i = 0; i < 6; i++) {
			if (raw[i] != mac.raw[i])
				return true;
		}

		return false;
	}

	bool operator <(const TMAC &ip) const
	{
		for (size_t i = 0; i < 6; i++) {
			if (raw[i] < ip.raw[i])
				return true;
		}

		return false;
	}

	bool operator >(const TMAC &mac) const
	{
		for (size_t i = 0; i < 6; i++) {
			if (raw[i] > mac.raw[i])
				return true;
		}

		return false;
	}
};

/**
 * IP rozsah adres
 */
struct TPool {
	TIPv4 start;
	TIPv4 end;

	static TPool fromString(
		const string &pool, char separator = '-')
	{
		TPool tPool = {0};
		const size_t index = pool.find_first_of('-');
		const string first = pool.substr(0, index);

		tPool.start = TIPv4::fromString(first);
		tPool.end = TIPv4::fromString(
			pool.substr(first.size() + 1, -1));

		return tPool;
	}

	TIPv4 broadcastAddress() const
	{
		const auto mask = subnetMask();
		TIPv4 ip = {0};

		for (size_t i = 0; i < 4; i++) {
			if (mask.raw[i] == 0)
				ip.raw[i] = 255;
			else
				ip.raw[i] = start.raw[i];
		}

		return ip;
	}

	TIPv4 subnetMask() const
	{
		size_t i = 0;
		for (; i < 4; i++) {
			if (end.raw[i] != start.raw[i])
				break;
		}

		switch (i) {
		case 0:
			return TIPv4::fromString("128.0.0.0");

		case 1:
			return TIPv4::fromString("255.0.0.0");

		case 2:
			return TIPv4::fromString("255.255.0.0");

		default:
			return TIPv4::fromString("255.255.255.0");
		}
	}

	/**
     * IP rozsah adres - velikost
     */
	size_t size() const
	{
		uint64_t size = 0;

		for (size_t i = 0; i < 4; i++) {
			const int range = end.raw[i] - start.raw[i];

			if (range <= 0)
				continue;

			size += range << ((3 - i) * 8);
		}

		if (size > 0)
			size++;

		return size;
	}

	string toString (
		const string &separator = "\n") const
	{
		string repr;

		repr += "pool: \n";
		repr += "\tstart: " + start.toString("") + "\n";
		repr += "\tend  : " + end.toString("") + "\n";
		repr += "\tsize : " + to_string(size()) + "\n";
		repr += "\tmask : " + subnetMask().toString();
		repr += "\tbcast: " + broadcastAddress().toString();
		repr += separator;

		return repr;
	}
};

struct TLeaseItem {
	TIPv4 ip;
	uint8_t state; //offer, request, ack
	uint64_t expiredTime;
};

struct TLeases {
	map<TMAC, TLeaseItem> leases;
	TPool pool;
	vector<TIPv4> free;
	vector<TIPv4> checked;
	uint64_t leaseTime;

	TLeases(TPool p):
		pool(p)
	{
		TIPv4 ip = pool.start;
		for (size_t i = 0; i < pool.size(); i++) {
			free.push_back(ip);
			ip++;
		}
	}

	size_t freeSize() const
	{
		return free.size();
	}

	size_t checkedSize() const
	{
		return checked.size();
	}

	int insert(TMAC mac, uint8_t type, TIPv4 &newIP)
	{
		if (free.empty())
			return -1;

		// vyber posledni IP adresy
		newIP = free.back();
		auto it = leases.emplace(mac,
			TLeaseItem{newIP, type, Util::timestamp() + leaseTime});

		// MAC adresa zde neni
		if (!it.second)
			return -2;

		free.pop_back();
		checked.push_back(newIP);

		return 0;
	}


	void release()
	{
		const uint64_t now = Util::timestamp();
		for (auto it = leases.begin(); it != leases.end(); ++it) {
			if (it->second.expiredTime >= now)
				continue;

			free.push_back(it->second.ip);
			leases.erase(it);

			size_t i = 0;
			for (;i < checked.size(); i++) {
				if (checked.at(i) == it->second.ip)
					break;
			}
			checked.erase(checked.begin() + i);
		}
	}
};

/**
 * Struktura se vstupnimi parametry, ktere
 * jsou prevedene do internetove podoby.
 */
struct TParams {
	string interface;
	TPool pool;
	TIPv4 gateway;
	TIPv4 dnsServer;
	string domain;
	uint64_t leaseTime;

	string toString(
		const string &separator = "\n") const
	{
		string repr;

		repr += "interface: ";
		repr += interface;
		repr += separator;

		repr += pool.toString();

		repr += "gateway: ";
		repr += gateway.toString();

		repr += "DNS Server: ";
		repr += dnsServer.toString();

		repr += "domain: ";
		repr += domain;
		repr += separator;

		repr += "lease time: ";
		repr += to_string(leaseTime);
		repr += separator;

		return repr;
	}
};

/**
 * Zpracovani vstupnich a overeni, 
 * ze byly zadany vsechny potrebne argumenty.
 */
 // Todo overit duplicitu argumentov a ich semantiku
int extractArguments(int argc, char *argv[], TParams *t)
{
	int c;
	while ((c = getopt(argc, argv, "i:p:g:n:d:l:")) != -1) {
		switch (c){
		case 'i':
			t->interface = optarg;
			break;

		case 'p':
			t->pool = TPool::fromString(optarg);
			break;

		case 'g':
			t->gateway = TIPv4::fromString(optarg);
			break;

		case 'n':
			t->dnsServer = TIPv4::fromString(optarg);
			break;

		case 'd':
			t->domain = optarg;
			break;

		case 'l':
			t->leaseTime = stoi(optarg, nullptr, 10);
			break;

		default:
			cerr << "error: invalid arguments" << endl;
			return -1;
		}
	}

	return 0;
}

/**
 * Overeni existence zadaneho rozhrani.
 */
int checkExistInterface(const string &interface)
{
	set<string> allDevices;
	if (Util::allDevices(allDevices) < 0)
		return -1;

	auto it = allDevices.find(interface);
	if (it == allDevices.end()) {
		string err;
		err += "unknown device name: ";
		err += interface;
		err += "\n";
		err += "all found devices: ";

		if (allDevices.size() > 1) {
			for (const auto &dev : allDevices)
				err += dev + ", ";

			// remove last two chars
			err.pop_back();
			err.pop_back();
		}
		else {
			err += "no device found";
		}

		cerr << err << endl;
		return -1;
	}

	return 0;
}

/**
 * Struktura obsahuje file descriptory na sockety, pripadne 
 * na handler pro odesilani udaju pomoci knihovny pcap.
 */
struct TCon {
	int listenSock;
	pcap_t *sendSock;
	struct sockaddr_in addrIn;

	int init(const string &interface)
	{
		int ret;

		listenSock = socket(PF_INET, SOCK_DGRAM, 0);
		if (listenSock <= 0 ) {
			cerr << "error: socket creating" << endl;
			return -1;
		}

		int option = 1;
		ret = setsockopt(
			listenSock,
			SOL_SOCKET,
			SO_REUSEADDR,
			&option,
			sizeof(option)
		);

		if (ret < 0) {
			cerr << "error: changing interface" << endl;
			close(listenSock);
			return -2;
		}

		ret = setsockopt(
			listenSock,
			SOL_SOCKET,
			SO_BINDTODEVICE,
			interface.c_str(),
			interface.length() + 1
		);

		if (ret < 0) {
			cerr << "error: problem with interface setting, are you root?" << endl;
			close(listenSock);
			return -3;
		}

		memset(&addrIn, 0, sizeof(addrIn));
		addrIn.sin_family = AF_INET;
		addrIn.sin_addr.s_addr = htonl(INADDR_ANY);
		addrIn.sin_port = htons(67);

		ret = bind(
			listenSock,
			(struct sockaddr *) &addrIn,
			sizeof(addrIn)
		);

		if (ret < 0) {
			cerr << "error: bind" << endl;
			close(listenSock);
			return -4;
		}

		pcap_t *handle;
		char errBuffer[PCAP_ERRBUF_SIZE] = {0};

		// otevreni pcap handleru
		sendSock = pcap_open_live(
			interface.c_str(),  // device
			512,                // snapLen
			1,                  // promisc
			1000,               // to_ms
			errBuffer           // error buffer
		);

		return 0;
	}
};


/**
 * Cteni ze vstupniho socketu bez cekani.
 */
int directRead(TCon *con, vector<uint8_t> &bufferVec)
{
	uint8_t buf[1024] = {0};
	int ret = 0;

	ret = (int) read(con->listenSock, buf, sizeof(buf) - 1);

	for (size_t i = 0; i < ret; i++) {
		bufferVec.push_back(buf[i]);
		//cout << hex << unsigned(buf[i]) << " ";
	}
	//cout << endl;

	return ret;
}

 /**
 * Cteni ze vstupniho socketu s timeoutem, po vyprseni casu 
 * vraci -1.
 * Pri prijmu dat se pro jejich nacteni pouzije directRead(). 
 * Doba cekani je v ms. 
 */
int poolRead(
	TCon *con, vector<uint8_t> &bufferVec, int timeout)
{
	struct pollfd pfd[1];
	pfd[0].fd = con->listenSock;
	pfd[0].events = POLLIN | POLLERR | POLLRDBAND;
	pfd[0].revents = 0;

	int ret;
	if ((ret = poll(pfd, 1, timeout)) < 0) {
		cerr << "error: pool: " << errno << endl;
		close(con->listenSock);
		return -1;
	}

	if (ret == 0) {
		//cerr << "timeout error" << endl;
		return 0;
	}

	if (pfd[0].revents & POLLERR) {
		cerr << "error: port seems to be closed" << endl;
		return -2;
	}

	if (pfd[0].revents & (POLLRDBAND | POLLIN)) {
		return directRead(con, bufferVec);
	}

	return -1;
}

/**
 * http://man7.org/linux/man-pages/man3/getifaddrs.3.html
 */
int address(TCon *con, const string &name, TIPv4 &ip) {
	int family;
	struct ifreq ifreq;
	char host[128];

	memset(&ifreq, 0, sizeof ifreq);
	strncpy(ifreq.ifr_name, name.c_str(), IFNAMSIZ);

	if (ioctl(con->listenSock, SIOCGIFADDR, &ifreq) != 0)
		return -1;

	switch(family=ifreq.ifr_addr.sa_family) {
	case AF_UNSPEC:
		return -2;
	case AF_INET:
	case AF_INET6:
		getnameinfo(
			&ifreq.ifr_addr,
			sizeof ifreq.ifr_addr,
			host,
			sizeof host,
			0,
			0,
			NI_NUMERICHOST
		);
		break;
	default:
		cerr << "error: unknown family: " << family << endl;
		return -3;
	}

	TIPv4 tmp = TIPv4::fromString(host);
	for (size_t i = 0; i < 4; i++)
		ip.raw[i] = tmp.raw[i];

	return 0;
}

bool m_stop = false;
void signalHandler(int signum)
{
	cerr << "end of application" << endl;
	m_stop = true;
}

/**
 * Zjisteni typu DHCP zpravy, identifikace serveru. 
 */
void extractOptions(
	DHCPServerInfo &info, uint8_t *options, size_t size)
{
	for (size_t m = 0; m < 30; m++) {
		switch (*options) {
		case 0x35:     // DHCP Message type
			options++; // length
			options++; // data
			info.messageType = *options;
			options++;
			break;

		default:
			options++;
			uint8_t length = *options;
			options++;
			options += length;
		}
	}
}

void extractPacketInformation(
	uint8_t *buffer,
	DHCPServerInfo &info,
	size_t size
)
{
	TDHCPHeader *header = (TDHCPHeader *) buffer;
	TDHCPData *tdhcpData = (TDHCPData *) (buffer + sizeof(TDHCPHeader));

	extractOptions(
		info,
		buffer + sizeof(TDHCPHeader) + sizeof(TDHCPData),
		size - sizeof(TDHCPHeader) - sizeof(TDHCPData)
	);

	info.transactionID = header->transactionID;

	for (size_t i = 0; i < 6; i++)
		info.dstMACAddress[i] = tdhcpData->clientHardwareAddress[i];

	for (size_t i = 0; i < 16; i++)
		info.clientHWAddress[i] = tdhcpData->clientHardwareAddress[i];
}

DHCPServerOffer buildOffer(
	DHCPServerInfo &info,
	DHCPPayload &payload,
	const vector<uint8_t> &mac
	)
{
	DHCPServerOffer offer;
	offer.setEthMAC(mac);
	offer.udpHeader.udpDestinationPort = 0x4400; // 68
	offer.udpHeader.udpSourcePort = 0x4300; // 67
	offer.setTransactionID(info.transactionID);
	offer.ipHeader.headerChecksum = 0xb639;

	for (size_t i = 0; i < 4; i++) {
		offer.dhcpHeader.yourIPAddr[i] = payload.yourIPAddr.raw[i];
		offer.dhcpHeader.serverIPAddr[i] = payload.serverIPAddr.raw[i];
		offer.op36payload[i] = payload.op36payload.raw[i];
		offer.op01payload[i] = payload.op01payload.raw[i];
		offer.op1cpayload[i] = payload.op1cpayload.raw[i];
		offer.op03payload[i] = payload.op03payload.raw[i];
		offer.op06payload[i] = payload.op06payload1.raw[i];
		offer.op06payload[4 + i] = payload.op06payload2.raw[i];
	}

	for (size_t i = 0; i < 6; i++)
		offer.ethHeader.dstMACAddr[i] = info.clientHWAddress[i];

	for (size_t i = 0; i < 16; i++)
		offer.dhcpData.clientHardwareAddress[i] = info.clientHWAddress[i];

	return offer;
}

DHCPServerOffer buildACK(
	DHCPServerInfo &info,
	DHCPPayload &payload,
	const vector<uint8_t> &mac
)
{
	DHCPServerOffer ack =
		buildOffer(info, payload, mac);
	ack.op35payload = 0x05;

	return ack;
}

void processPacket(
	const vector<uint8_t> &data,
	TCon *t,
	DHCPPayload &payload,
	const vector<uint8_t> &mac,
	TLeases &leases)
{
	uint8_t buffer[data.size()];
	for (size_t i = 0; i < data.size(); i++)
		buffer[i] = data[i];

	DHCPServerInfo info;
	extractPacketInformation(
		buffer,
		info,
		data.size()
	);

	uint8_t type = info.messageType;
	if (type == 1) {
		int ret = leases.insert(
			TMAC::fromArray((uint8_t *)&(info.clientHWAddress)),
			1,
			payload.yourIPAddr);

		DHCPServerOffer offer = buildOffer(info, payload, mac);
		offer.op33payload = htonl(leases.leaseTime);

		if (ret == -1) {
			cerr << "error: empty pool" << endl;
			return;
		}
		else if (ret == -2) {
			cerr << "error: duplicate mac" << endl;
			return;
		}

		pcap_sendpacket(t->sendSock, (const u_char *) &offer, sizeof(offer));

		//cout << "send offer" << endl;
	}
	else if (type == 3) {
		DHCPServerOffer ack = buildACK(info, payload, mac);
		auto it = leases.leases.find(
			TMAC::fromArray((uint8_t *)&(info.clientHWAddress))
			);
		if (it == leases.leases.end()) {
			cerr << "error: not registered mac addr" << endl;
			return;
		}
		for (size_t i = 0; i < 4; i++)
			ack.dhcpHeader.yourIPAddr[i] = it->second.ip.raw[i];

		ack.op33payload = htonl(leases.leaseTime);

		pcap_sendpacket(t->sendSock, (const u_char *) &ack, sizeof(ack));

		//cout << "send request" << endl;
	}
}

int main(int argc, char *argv[])
{
	signal(SIGINT, signalHandler);

	if (argc != 13) {
		cerr << "error: invalid number of arguments" << endl;
		return EXIT_FAILURE;
	}

	TParams params;
	if (extractArguments(argc, argv, &params) == -1) {
		cerr << "error: invalid arguments" << endl;
		return EXIT_FAILURE;
	}

	if (checkExistInterface(params.interface) < 0) {
		cerr << "error: wrong interface" << endl;
		return EXIT_FAILURE;
	}

	//cout << params.toString() << endl;

	TCon con = {0};

	if (con.init(params.interface) < 0)
		return EXIT_FAILURE;

	TLeases leases(params.pool);
	leases.leaseTime = params.leaseTime;
	vector<uint8_t> vec;
	const int milliseconds = 10 * 1000; // 10 s
	while (!m_stop) {
		vec.clear();
		int ret = poolRead(&con, vec, milliseconds);
		if (ret == 0)
			continue;
		else if (ret < 0)
			return EXIT_FAILURE;

		DHCPPayload pay = {0};
		TIPv4 tmpIP = {0};

		if (address(&con, params.interface, tmpIP) < 0)
			return EXIT_FAILURE;
		pay.serverIPAddr = tmpIP;
		pay.op36payload = pay.serverIPAddr;

		pay.op01payload = params.pool.subnetMask();
		pay.op1cpayload = params.pool.broadcastAddress();

		pay.op03payload = params.gateway;
		pay.op06payload1 = params.dnsServer;
		pay.op06payload2 = params.dnsServer;

		vector<uint8_t> mac;
		Util::MACAddress(params.interface, mac);

		leases.release();
		processPacket(vec, &con, pay, mac, leases);
	}
}
