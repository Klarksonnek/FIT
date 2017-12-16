#include <iostream>

#include "CustomException.h"
#include "Demodulator.h"

using namespace std;

/**
 * Main program.
 */
int main(int argc, char** argv)
{
	if (argc != 2) {
		cerr <<  "input wav file was not entered" << endl;
		return EXIT_FAILURE;
	}

	try {
		Demodulator demodulator;
		demodulator.setInputWavFile(argv[1]);
		demodulator.demodulation();
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

