#include <iostream>

#include "CustomException.h"
#include "Modulator.h"

using namespace std;

/**
 * Main program.
 */
int main(int argc, char** argv)
{
	if (argc != 2) {
		cerr <<  "input text file was not entered" << endl;
		return EXIT_FAILURE;
	}

	try {
		Modulator modulator;
		modulator.setInputTextFile(argv[1]);
		modulator.modulation();
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

