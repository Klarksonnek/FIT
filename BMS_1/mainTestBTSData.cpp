#include <iostream>

#include "BTSLoadData.h"

using namespace std;

static const string csvPath = "./csv/";

int main()
{
	// TEST 1
	cout << "T1" << endl;
	BTSLoadData parser;
	if (!parser.open(csvPath + "00-crlf.csv", ','))
		cerr << "file cannot be opened" << endl;
	if (parser.isEOF())
		cerr << "file is empty" << endl;

	unsigned int i;
	auto row1 = parser.rawRow();
	for (i = 0; i < row1.size(); i++)
		cerr << row1.at(i) << endl;
	if(3 != row1.size() ||
	"one" != row1.at(0) ||
	"two" != row1.at(1) ||
	"three" != row1.at(2))
		cerr << "wrong 1. row" << endl;

	auto row2 = parser.rawRow();
	for (i = 0; i < row2.size(); i++)
		cerr << row2.at(i) << endl;
	if(3 != row2.size() ||
	"four" != row2.at(0) ||
	"five" != row2.at(1) ||
	"six" != row2.at(2))
		cerr << "wrong 2. row" << endl;

	// TODO: nutno doplnit
	parser.rawRow();

	if(!parser.isEOF())
		cerr << "parser did not finish, but all is processed" << endl;


	// TEST 2
	cout << "T2" << endl;
	if (!parser.open(csvPath + "00-lf.csv", ','))
		cerr << "file cannot be opened" << endl;
	if (parser.isEOF())
		cerr << "file is empty" << endl;

	row1 = parser.rawRow();
	for (i = 0; i < row1.size(); i++)
		cerr << row1.at(i) << endl;
	if(3 != row1.size() ||
	"one" != row1.at(0) ||
	"two" != row1.at(1) ||
	"three" != row1.at(2))
		cerr << "wrong 1. row" << endl;

	row2 = parser.rawRow();
	for (i = 0; i < row2.size(); i++)
		cerr << row2.at(i) << endl;
	if(3 != row2.size() ||
	"four" != row2.at(0) ||
	"five" != row2.at(1) ||
	"six" != row2.at(2))
		cerr << "wrong 2. row" << endl;

	// TODO: nutno doplnit
	parser.rawRow();

	if(!parser.isEOF())
		cerr << "parser did not finish, but all is processed" << endl;


	// TEST 3
	cout << "T3" << endl;
	if (!parser.open(csvPath + "00-crlf.csv", ','))
		cerr << "file cannot be opened" << endl;
	if (parser.isEOF())
		cerr << "file is empty" << endl;

	row1 = parser.rawRow();
	for (i = 0; i < row1.size(); i++)
		cerr << row1.at(i) << endl;
	if(3 != row1.size() ||
	"one" != row1.at(0) ||
	"two" != row1.at(1) ||
	"three" != row1.at(2))
		cerr << "wrong 1. row" << endl;

	row2 = parser.rawRow();
	for (i = 0; i < row2.size(); i++)
		cerr << row2.at(i) << endl;
	if(3 != row2.size() ||
	"four" != row2.at(0) ||
	"five" != row2.at(1) ||
	"six" != row2.at(2))
		cerr << "wrong 2. row" << endl;

	auto row3 = parser.rawRow();
	if (0 != row3.size())
		cerr << "wrong 3. row" << endl;

	if(!parser.isEOF())
		cerr << "parser did not finish, but all is processed" << endl;


	// TEST 4
	cout << "T4" << endl;
	if (!parser.open(csvPath + "00-lf.csv", ','))
		cerr << "file cannot be opened" << endl;
	if (parser.isEOF())
		cerr << "file is empty" << endl;

	row1 = parser.rawRow();
	for (i = 0; i < row1.size(); i++)
		cerr << row1.at(i) << endl;
	if(3 != row1.size() ||
	"one" != row1.at(0) ||
	"two" != row1.at(1) ||
	"three" != row1.at(2))
		cerr << "wrong 1. row" << endl;

	row2 = parser.rawRow();
	for (i = 0; i < row2.size(); i++)
		cerr << row2.at(i) << endl;
	if(3 != row2.size() ||
	"four" != row2.at(0) ||
	"five" != row2.at(1) ||
	"six" != row2.at(2))
		cerr << "wrong 2. row" << endl;

	row3 = parser.rawRow();
	if (0 != row3.size())
		cerr << "wrong number of rows" << endl;

	if(!parser.isEOF())
		cerr << "parser did not finish, but all is processed" << endl;


	// TEST 5
	cout << "T5" << endl;
	if (!parser.open(csvPath + "02.csv", ';'))
		cerr << "file cannot be opened" << endl;

	auto firstLine = parser.rawRow();
	if (6 != firstLine.size() ||
	("LAC" != firstLine.at(0)) ||
	("CID" != firstLine.at(1)) ||
	("RSSI" != firstLine.at(2)) ||
	("Signal" != firstLine.at(3)) ||
	("ant H" != firstLine.at(4)) ||
	("power" != firstLine.at(5)))
		cout << "wrong number of rows or wrong header" << endl;

	// skip to last line
	for (size_t i = 1; i <= 6; i++)
		parser.rawRow();

	auto const lastLine = parser.rawRow();
	if (6 != lastLine.size() ||
	("8030" != lastLine.at(0)) ||
	("37276" != lastLine.at(1)) ||
	("-1" != lastLine.at(2)) ||
	("-91" != lastLine.at(3)) ||
	("20" != lastLine.at(4)) ||
	("10" != lastLine.at(5)))
		cout << "wrong last row" << endl;


	// TEST 6
	cout << "T6" << endl;
	if (!parser.open(csvPath + "03-bts.csv", ';'))
		cerr << "file cannot be opened" << endl;

	firstLine = parser.rawRow();
	if (5 != firstLine.size() ||
	("CID" != firstLine.at(0)) ||
	("LAC" != firstLine.at(1)) ||
	("BCH" != firstLine.at(2)) ||
	("Localization" != firstLine.at(3)) ||
	("GPS" != firstLine.at(4)))
		cout << "wrong number of rows or wrong header" << endl;

	auto const firstDataLine = parser.rawRow();
	if (5 != firstDataLine.size() ||
	("10127" != firstDataLine.at(0)) ||
	("8050" != firstDataLine.at(1)) ||
	("777" != firstDataLine.at(2)))
		cout << "wrong last row" << endl;
	//CPPUNIT_ASSERT("Brno - Kohoutovice, Axmanova 531/13, panel�k, ex 1020x (+GSM)" == firstDataLine.at(3));
	//CPPUNIT_ASSERT("49�11'23.10N,16�32'13.38E" == firstDataLine.at(4));


	// TEST 7
	cout << "T7" << endl;
	if (!parser.open(csvPath + "04.csv", ','))
		cerr << "file cannot be opened" << endl;

	row1 = parser.rawRow();
	if (3 == row1.size())
		cout << "wrong number of rows" << endl;

	try {
		row1 = parser.rawRow();
	}
	catch(const std::exception& e) {
		std::cout << e.what();
	}

	return 0;
}
