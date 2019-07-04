#include <chrono>
#include <cstring>
#include <error.h>
#include <exception>
#include <net/if.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <pcap.h>
#include <iostream>

#include "Util.h"
#include <ifaddrs.h>
#include <arpa/inet.h>

using namespace std;

int Util::allDevices(set<string> &devices)
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *addr;

	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next)
		devices.insert(ifa->ifa_name);

	freeifaddrs(ifap);
	return 0;
}

uint8_t Util::randomUint8()
{
	return (uint8_t) rand();
}

uint32_t Util::randomUint32()
{
	return (uint32_t) rand();
}

uint64_t Util::timestamp()
{
	return (uint64_t) chrono::duration_cast< chrono::milliseconds >(
		chrono::system_clock::now().time_since_epoch()).count();
}

int Util::MACAddress(const string &dev, vector<uint8_t> &mac)
{
	struct ifreq s = {0};
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (fd <= 0 )
		return -1;

	strcpy(s.ifr_name, dev.c_str());
	if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
		int i;
		for (i = 0; i < 6; ++i)
			mac.push_back((uint8_t) s.ifr_addr.sa_data[i]);

		return 0;
	}

	return -2;
}
