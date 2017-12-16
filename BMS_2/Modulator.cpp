#include <cmath>
#include <fstream>

#include "CustomException.h"
#include "Modulator.h"
#include "sndfile.hh"

using namespace std;

#define SAMPLE_RATE 18000
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)
#define PHASE_SHIFT_14 M_PI/4
#define PHASE_SHIFT_34 3 * M_PI/4
#define PHASE_SHIFT_54 5 * M_PI/4
#define PHASE_SHIFT_74 7 * M_PI/4
#define SAMPLES 30
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)

const string SYNC_SEQUENCE = "00110011";

Modulator::Modulator():
	m_buffer(new int[SAMPLE_RATE]),
	m_length(0)
{
}

Modulator::~Modulator()
{
	delete [] m_buffer;
}

void Modulator::modulation()
{
	ifstream file;
	list<char> inputBits;
	unsigned int length = 0;
	unsigned int i = 0;
	char c1;
	char c2;
	string outputFile;

	file.open(m_inputFile.c_str());
	if (!file.is_open())
		throw CustomException("input text file cannot be opened");

	size_t position = m_inputFile.find_last_of(".");
	if (position == string::npos)
		throw CustomException("input text file does not contain '.txt' suffix");
	if ((position != m_inputFile.size() - 4)
		|| (m_inputFile.substr(position + 1, m_inputFile.size() - 1) != "txt"))
		throw CustomException("input text file contain invalid suffix");
	outputFile = m_inputFile.substr(0, position) + ".wav";

	insertSyncSeq(inputBits);
	loadBits(file, inputBits);

	while (!inputBits.empty()) {
		if (i >= SAMPLE_RATE)
			throw CustomException("too many bits in input file");

		c1 = inputBits.front();
		inputBits.pop_front();
		c2 = inputBits.front();
		inputBits.pop_front();

		for (int j = 0; j < SAMPLES; j++) {
			if (i >= SAMPLE_RATE)
				throw CustomException("too many bits in input file");

			if (c1 == '0' && c2 == '1')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_14));

			if (c1 == '0' && c2 == '0')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_34));

			if (c1 == '1' && c2 == '0')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_54));

			if (c1 == '1' && c2 == '1')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_74));

			i++;
		}
	}
	length = i;

	SndfileHandle output;
	output = SndfileHandle(outputFile, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);
	output.write(m_buffer, length);
}

void Modulator::setInputTextFile(const string &file)
{
	m_inputFile = file;
}

string Modulator::inputTextFile() const
{
	return m_inputFile;
}

void Modulator::insertSyncSeq(list<char> &inputBits)
{
	for (char c : SYNC_SEQUENCE)
		inputBits.push_back(c);
}

void Modulator::loadBits(std::ifstream &file, list<char> &inputBits)
{
	char c;

	while(file.get(c)) {
		if (c == '0' || c == '1') {
			inputBits.push_back(c);
		}

		else if (c != '\n' && c != '\r')
			throw CustomException("invalid character in input text file");
	}

	if ((inputBits.size() % 2) != 0)
		throw CustomException("odd number of bits in input text file");

	file.close();
}

