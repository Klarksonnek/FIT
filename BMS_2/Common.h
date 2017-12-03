#pragma once

#include <string>

class Common
{
public:
	void modulation(const std::string &inputFile, const std::string &outputFile);
	void demodulation(const std::string &inputFile, const std::string &outputFile);
};
