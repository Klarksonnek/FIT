#pragma once

#include <string>
#include <vector>

#include "sndfile.hh"

class Demodulator {
public:
	Demodulator();

	void demodulation();
	void setInputWavFile(const std::string &file);

private:
	std::string setOutputTxtFile();
	void setEps();
	void loadSamples(SndfileHandle &input);
	bool checkSyncSeq(unsigned int *numberOfSamples);
	unsigned int determineNumberOfSamples();
	void shiftPhase();
	std::string determinePhase(unsigned int count, unsigned int startPosition);
	std::string obtainPhase(unsigned int position);
	void demodulate(unsigned int numberOfSamples);

	std::string m_inputFile;
	int m_format;
	int m_eps;
	std::vector<int> m_samples;
	std::vector<std::string> m_bits;
};