#include <cmath>
#include <fstream>
#include <iostream>
#include <list>

#include "Common.h"
#include "CustomException.h"
#include "sndfile.hh"

#define SAMPLE_RATE 18000
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)
#define PHASE_SHIFT_14 M_PI/4
#define PHASE_SHIFT_34 3 * M_PI/4
#define PHASE_SHIFT_54 5 * M_PI/4
#define PHASE_SHIFT_74 7 * M_PI/4
#define SAMPLES 15
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)

using namespace std;

void Common::modulation(const string &inputFile, const string &outputFile)
{
	ifstream file;
	char c;
	list<char> inputBitSequence;
	char c1;
	char c2;
	int *buffer = new int[SAMPLE_RATE];
	unsigned int i = 0;
	const string SYNC_SEQUENCE = "00110011";

	for (char c : SYNC_SEQUENCE)
		inputBitSequence.push_back(c);

	file.open(inputFile.c_str());

	if (!file.is_open())
		throw CustomException("file with input bit sequence of modulator cannot be opened");

	while(file.get(c)) {
		if (c == '0' || c == '1') {
			inputBitSequence.push_back(c);
		}

		else if (c != '\n' && c != '\r')
			throw CustomException("invalid character in input bit sequence of modulator");
	}

	if ((inputBitSequence.size() % 2) != 0)
		throw CustomException("odd number of bits in input bit sequence of modulator");

	file.close();

	while (!inputBitSequence.empty()) {
		if (i >= SAMPLE_RATE)
			throw CustomException("too many samples on input bit sequence of modulator");

		c1 = inputBitSequence.front();
		inputBitSequence.pop_front();
		c2 = inputBitSequence.front();
		inputBitSequence.pop_front();

		for (int j = 0; j < SAMPLES; j++) {
			if (i >= SAMPLE_RATE)
				throw CustomException("too many samples on input bit sequence of modulator");

			if (c1 == '0' && c2 == '1')
				buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_14));

			if (c1 == '0' && c2 == '0')
				buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_34));

			if (c1 == '1' && c2 == '0')
				buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_54));

			if (c1 == '1' && c2 == '1')
				buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_74));

			i++;
		}
	}

	SndfileHandle output;
	output = SndfileHandle(outputFile, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);
	output.write(buffer, i);

	delete [] buffer;
}
