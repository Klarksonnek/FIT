/**
 * File: main.c
 * Author: Bc. Klara Necasova (xnecas24)
 * Login: xnecas24
 * Date: 16. 2. 2018
 * Description: Parameter parsing and GIF to BMP conversion using gif2bmp() method
 */

#include <iostream>
#include <unistd.h>

#include "gif2bmp.h"

using namespace std;

struct Params {
	string inputFile;
	string outputFile;
	string logFile;
	bool help = false;
};

void printHelp()
{
	const string helpMsg =
		"Conversion of GIF image to BMP image\n"
		"Parameters:\n"
		"-h  prints help message\n"
		"-i  <file> input file name\n"
 		"-o  <file> output file name\n"
		"-l  <file> log file name with input GIF size and output BMP size";

	cout << helpMsg << endl;
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
				params->help = true;
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
	Params params;

	int result = getParams(argc, argv, &params);
	if (params.help) {
		printHelp();
		return EXIT_SUCCESS;
	}

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

	tGIF2BMP gifData = {0, 0};
	int conversion = gif2bmp(&gifData, inputFile, outputFile);

	if (!params.logFile.empty() && conversion == 0) {
		string log;
		log += "login = xnecas24\n";
		log += "uncodedSize = " + to_string(gifData.bmpSize) + "\n";
		log += "codedSize = " + to_string(gifData.gifSize) + "\n";

		ofstream out(params.logFile);
		if (out.is_open()) {
			out << log << endl;
			out.close();
		}
		else {
			cerr << "log file error" << endl;
		}
	}

	return EXIT_SUCCESS;
}
