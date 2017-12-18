#include <cmath>
#include <fstream>

#include "CustomException.h"
#include "Modulator.h"
#include "sndfile.hh"

#define SAMPLE_RATE 18000
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

#define SAMPLES 30
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)

#define PHASE_SHIFT_14 M_PI/4
#define PHASE_SHIFT_34 3 * M_PI/4
#define PHASE_SHIFT_54 5 * M_PI/4
#define PHASE_SHIFT_74 7 * M_PI/4

using namespace std;

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

/**
 * Modulates input txt file.
 */
void Modulator::modulation()
{
	ifstream file;
	list<char> inputBits;
	unsigned int length = 0;
	string outputFile;

	// open the input file
	file.open(m_inputFile.c_str());
	if (!file.is_open())
		throw CustomException("input text file cannot be opened");

	// set the output wav file
	setOutputWavFile();

	// insert synchronization sequence at the beginning of list of bits
	insertSyncSeq(inputBits);

	// load all bits from the input file
	loadBits(file, inputBits);

	// module the input and store its length
	length = modulate(inputBits);

	// open the output file and write output to it
	SndfileHandle output;
	output = SndfileHandle(outputFile, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);
	output.write(m_buffer, length);
}

/**
 * Sets the input file (txt).
 * @param  file     Input filename.
 */
void Modulator::setInputTextFile(const string &file)
{
	m_inputFile = file;
}

/**
 * Creates the output filename (wav).
 * @return  Output filename.
 */
string Modulator::setOutputWavFile()
{
	string outputFile;

	// replace .txt suffix with .wav suffix
	// last position where '.' is present
	size_t position = m_inputFile.find_last_of(".");
	if (position == string::npos)
		throw CustomException("input text file does not contain '.txt' suffix");
	if ((position != m_inputFile.size() - 4)
		|| (m_inputFile.substr(position + 1, m_inputFile.size() - 1) != "txt"))
		throw CustomException("input text file contains invalid suffix");

	outputFile = m_inputFile.substr(0, position) + ".wav";

	return outputFile;
}

/**
 * Modulates input data.
 * @param  inputBits     Input bits loaded from the input file.
 * @return  Number of bit pairs.
 */
unsigned int Modulator::modulate(list<char> &inputBits)
{
	unsigned int i = 0;
	char c1;
	char c2;

	while (!inputBits.empty()) {
		if (i >= SAMPLE_RATE)
			throw CustomException("too many bits in input file");

		// load one pair of bits
		c1 = inputBits.front();
		inputBits.pop_front();
		c2 = inputBits.front();
		inputBits.pop_front();

		// modulate
		for (unsigned int j = 0; j < SAMPLES; j++) {
			if (i >= SAMPLE_RATE)
				throw CustomException("too many bits in input file");

			// 01 = phase shift M_PI/4
			if (c1 == '0' && c2 == '1')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_14));
			// 00 = phase shift 3 * M_PI/4
			if (c1 == '0' && c2 == '0')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_34));
			// 10 = phase shift 5 * M_PI/4
			if (c1 == '1' && c2 == '0')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_54));
			// 11 = phase shift 7 * M_PI/4
			if (c1 == '1' && c2 == '1')
				m_buffer[i] = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + PHASE_SHIFT_74));

			i++;
		}
	}
	return i;
}

/**
 * Inserts synchronization sequence to the list of bits.
 * @param  inputBits     List of bits.
 */
void Modulator::insertSyncSeq(list<char> &inputBits)
{
	for (char c : SYNC_SEQUENCE)
		inputBits.push_back(c);
}

/**
 * Loads bits from the input file and stores them into the list.
 * @param  file       Input file.
 * @param  inputBits  List of bits.
 */
void Modulator::loadBits(std::ifstream &file, list<char> &inputBits)
{
	char c;

	while(file.get(c)) {
		// store 0 or 1, ignore \n or \r
		if (c == '0' || c == '1')
			inputBits.push_back(c);

		else if (c != '\n' && c != '\r')
			throw CustomException("invalid character in input text file");
	}

	if ((inputBits.size() % 2) != 0)
		throw CustomException("odd number of bits in input text file");

	file.close();
}

