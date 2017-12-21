#include <cmath>
#include <fstream>

#include "CustomException.h"
#include "Demodulator.h"

#define SAMPLE_RATE 18000
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)
#define SAMPLES 15
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)

#define PHASE_SHIFT_14 M_PI/4
#define PHASE_SHIFT_34 3 * M_PI/4
#define PHASE_SHIFT_54 5 * M_PI/4
#define PHASE_SHIFT_74 7 * M_PI/4

#define SYNC_SEQUENCE_LENGTH 8
#define PHASE_SHIFT_CNT 4

using namespace std;

// phase shift: 3 * M_PI/4, M_PI/4, 5 * M_PI/4, 7 * M_PI/4
double phase[PHASE_SHIFT_CNT] = {PHASE_SHIFT_34, PHASE_SHIFT_14, PHASE_SHIFT_54, PHASE_SHIFT_74};
const string SYNC_SEQUENCE = "00110011";

Demodulator::Demodulator():
	m_format(0),
	m_eps(0)
{
}

/**
 * Demodulates input wav file.
 */
void Demodulator::demodulation()
{
	SndfileHandle input;
	unsigned int numberOfSamples = 0;
	string outputFile;

	// open the input wav file
	input = SndfileHandle(m_inputFile.c_str());

	// check format of the input file
	m_format = input.format();
	if ((m_format & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
		throw CustomException("input file is not in wav format");

	// check bit encoding and set precision value for comparison
	setEps();

	// set the output txt file
	outputFile = setOutputTxtFile();

	// load all samples from the input file
	loadSamples(input);

	// check synchronization sequence
	if (!checkSyncSeq(&numberOfSamples))
		throw CustomException("invalid sync sequence");

	// demodulate the input
	demodulate(numberOfSamples);

	// create and open the output file
	ofstream output(outputFile);
	if (!output.is_open())
		throw CustomException("output text file cannot be opened");

	// check if some data are available
	if (m_bits.empty())
		return;

	// write data to the output file
	for (unsigned int i = 0; i < m_bits.size(); i++)
		output << m_bits.at(i);
	output << "\n";

	output.close();
}

/**
 * Sets the input file (wav).
 * @param  file     Input filename.
 */
void Demodulator::setInputWavFile(const string &file)
{
	m_inputFile = file;
}

/**
 * Creates the output filename (txt).
 * @return  Output filename.
 */
string Demodulator::setOutputTxtFile()
{
	string outputFile;

	// replace .wav suffix with .txt suffix
	// last position where '.' is present
	size_t position = m_inputFile.find_last_of(".");
	if (position == string::npos)
		throw CustomException("input wav file does not contain '.wav' suffix");
	if ((position != m_inputFile.size() - 4)
		|| (m_inputFile.substr(position + 1, m_inputFile.size() - 1) != "wav"))
		throw CustomException("input wav file contains invalid suffix");

	outputFile = m_inputFile.substr(0, position) + ".txt";

	return outputFile;
}

/**
 * Sets precision for sample comparison according to the type of modulation.
 */
void Demodulator::setEps()
{
	if ((m_format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_32)
		m_eps = 0;
	else if ((m_format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_24)
		m_eps = 256;
	else if ((m_format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_16)
		m_eps = 65536;
	else
		throw CustomException("unsupported PCM format");
}

/**
 * Loads samples from the input file.
 * @param  input     Input file.
 */
void Demodulator::loadSamples(SndfileHandle &input)
{
	int sample = 0;

	// store input data to the vector
	while (input.read(&sample, 1) == 1)
		m_samples.push_back(sample);
}

/**
 * Checks the synchronization sequence.
 * @param  numberOfSamples     Number of samples.
 * @return  True if sequence is valid, false otherwise.
 */
bool Demodulator::checkSyncSeq(unsigned int *numberOfSamples)
{
	unsigned int i = 0;

	*numberOfSamples = determineNumberOfSamples();

	if ((m_samples.size() % *numberOfSamples) != 0)
		throw CustomException("different number of samples for a phase shift");

	// i goes through samples and j goes through pair of bits
	for (unsigned int j = 0; j < SYNC_SEQUENCE_LENGTH; j += 2) {
		string refPhase = SYNC_SEQUENCE.substr(j, 2);
		string phase = determinePhase(*numberOfSamples, i * *numberOfSamples);

		// if phases are not equal, synchronization sequence is invalid
		if(phase != refPhase)
			return false;

		i++;
	}

	return true;
}

/**
 * Determines phase shift and number of samples for the phase shift.
 * @return  Number of samples.
 */
unsigned int Demodulator::determineNumberOfSamples()
{
	unsigned int i = 0;
	int refSample;
	int sample;

	refSample = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + phase[0]));
	sample = m_samples.at(i);

	// if the absolute error between samples is greater than given precision,
	// shift the sin phase
	if (abs(refSample - sample) > m_eps) {
		shiftPhase();

		refSample = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + phase[0]));
		// if the absolute error between samples is greater than given precision again,
		// the sequence is invalid
		if (abs(refSample - sample) > m_eps)
			throw CustomException("invalid sync sequence");
	}

	i++;

	// determine the number of samples for phase shift
	while (i < m_samples.size()) {
		refSample = int(AMPLITUDE * sin(FREQ * 2 * M_PI * i + phase[0]));
		sample = m_samples.at(i);

		// if the absolute error between samples is greater than given precision,
		// return number of samples
		if (abs(refSample - sample) > m_eps)
			break;
		i++;
	}

	return i;
}

/**
 * Shifts the phase.
 */
void Demodulator::shiftPhase()
{
	for (unsigned int i = 0; i < PHASE_SHIFT_CNT; i++)
		phase[i] = 2 * M_PI - phase[i];
}

/**
 * Determines pair of bits corresponding with the phase shift.
 * @param  count     Count of samples for phase shift.
 * @param  startPosition     Start position in the input data.
 * @return  Pair of bits corresponding with phase shift.
 */
string Demodulator::determinePhase(unsigned int count, unsigned int startPosition)
{
	int refSample = 0;
	int sample = 0;
	unsigned int defaultPosition = startPosition;

	for (unsigned int i = 0; i < PHASE_SHIFT_CNT; i++) {
		startPosition = defaultPosition;

		for (unsigned int j = 0; j < count; j++) {
			refSample = int(AMPLITUDE * sin(FREQ * 2 * M_PI * startPosition + phase[i]));
			sample = m_samples.at(startPosition);

			// if the absolute error between samples is greater than given precision,
			// try next phase shift
			if (abs(refSample - sample) > m_eps)
				break;

			// if all samples are equal, obtain phase
			if (j == count - 1)
				return obtainPhase(i);
			// next sample
			startPosition++;
		}
	}

	throw CustomException("invalid phase");
}

/**
 * Determines pair of bits according to the phase shift.
 * @param  position     Position corresponding with the phase shift.
 * @return  Pair of bits.
 */
string Demodulator::obtainPhase(unsigned int position)
{
	if (position == 0)
		return "00";
	else if (position == 1)
		return "01";
	else if (position == 2)
		return "10";
	else
		return "11";
}

/**
 * Demodulates input data.
 * @param  numberOfSamples     Number of samples.
 */
void Demodulator::demodulate(unsigned int numberOfSamples)
{
	string phase;
	unsigned int dataLength = (unsigned)m_samples.size()/numberOfSamples;
	unsigned int syncSequenceLength = SYNC_SEQUENCE_LENGTH/2;

	// skip the synchronization sequence
	for (unsigned int i = syncSequenceLength; i < dataLength; i++) {
		// determine pair of bits corresponding with the phase shift
		phase = determinePhase(numberOfSamples, i * numberOfSamples);
		m_bits.push_back(phase);
	}
}




