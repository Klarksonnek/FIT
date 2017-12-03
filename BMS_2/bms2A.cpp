/* 
 * File:   bms1A.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>

#include "Common.h"
#include "CustomException.h"

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2) {
		cerr <<  "file containing bit sequence of modulator was not entered" << endl;
		return EXIT_FAILURE;
	}

	try {
		Common modulatorAndDemodulator;
		modulatorAndDemodulator.modulation(argv[1], string(argv[1]) + ".waw");
	}
	catch (const CustomException &ex) {
		cerr << ex.message() << endl;
		return EXIT_FAILURE;
	}
	catch (...) {
		cerr << "unknown error" << endl;
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}

