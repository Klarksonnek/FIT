#pragma once

#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <vector>

class Util {
public:
	/**
	 * Zjisteni dostupnych rozhrani na komunikaci.
	 */
	static int allDevices(std::set<std::string> &devices);

	static uint8_t randomUint8();
	static uint32_t randomUint32();

	/**
	 * Pomocna metoda pro vypis cisla v hexadecimalnim formatu.
	 */
	template<typename T>
	static std::string intToHex(
		T value, const std::string &prefix = "")
	{
		std::stringstream stream;
		stream << prefix
		       << std::setfill ('0') << std::setw(sizeof(T)*2)
		       << std::hex << unsigned(value);
		return stream.str();
	}

	/**
	 * Vraceni aktualniho timestampu v milisekundach.
	 */
	static uint64_t timestamp();

	/**
	 * Zjisteni mac adresy daneho zarizeni.
	 */
	static int MACAddress(
		const std::string &dev, std::vector<uint8_t> &mac);
};

