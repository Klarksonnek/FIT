#pragma once

#include <list>
#include <string>

/**
 * Class containing information and methods needed
 * for modulation.
 */
class Modulator {
public:
	Modulator();
	~Modulator();

	void modulation();
	void setInputTextFile(const std::string &file);

private:
	std::string makeWavFile();
	unsigned int modulate(std::list<char> &inputBits);
	void loadBits(std::ifstream &file, std::list<char> &inputBits);

	std::string m_inputFile;
	int* m_buffer;
};