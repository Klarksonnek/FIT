/**
 * Author: Bc. Klara Necasova
 * Login: xnecas24
 * Date: 16. 2. 2018
 * Description: Library for GIF to BMP conversion
 */

#include <iostream>
#include <unistd.h>

#include "gif2bmp.h"

using namespace std;

struct Params {
	string inputFile;
	string outputFile;
	string logFile;
}params;

void printHelp()
{
	cout << "..." << endl;
}

int getParams(int argc, char *argv[], Params *params)
{
	int c;

	while ((c = getopt(argc, argv, "i:o:l:h")) != -1) {
		switch(c) {
			case 'i':
				params->inputFile = optarg;
				break;
			case 'o':
				params->outputFile = optarg;
				break;
			case 'l':
				params->logFile = optarg;
				break;
			case 'h':
				printHelp();
				break;
			case ':':
				return -1;
			default:
				return -2;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int result = getParams(argc, argv, &params);
	switch (result) {
		case -1:
			cerr << "missing argument" << endl;
			return -1;
		case -2:
			cerr << "invalid parameter" << endl;
			return -2;
		default:;
	}

	FILE *inputFile;
	FILE *outputFile;

	if (params.inputFile.empty())
		inputFile = stdin;
	else
		inputFile = fopen(params.inputFile.c_str(), "rb");

	if (inputFile == nullptr) {
		cerr << "input file cannot be opened" << endl;
		return -3;
	}

	if (params.outputFile.empty())
		outputFile = stdout;
	else
		outputFile = fopen(params.outputFile.c_str(), "wb");

	if (outputFile == nullptr) {
		cerr << "output file cannot be opened" << endl;
		return -4;
	}

	tGIF2BMP gifData = {0,0};
	int conversion = gif2bmp(&gifData, inputFile, outputFile);
	LOG cout << "Conversion result: " << conversion << endl;

	return 0;
}
