#include "BTSLoadData.h"
#include "CustomException.h"

using namespace std;

BTSLoadData::BTSLoadData()
{
}

/**
 * Opens input file and file containing list of BTS.
 * @param 	file    	File name.
 * @param 	delimiter	Delimiter.
 * @return 	True if file is open, false otherwise.
 */
bool BTSLoadData::open(const string &file, char delimiter)
{
	m_file.open(file.c_str());
	m_delimiter = delimiter;

	return m_file.is_open();
}

/**
 * Closes file.
 */
void BTSLoadData::close()
{
	m_file.close();
}

/**
 * Tests end of file.
 * @return	True if it is EOF, false otherwise.
 */
bool BTSLoadData::isEOF() const
{
	return m_file.eof();
}

bool BTSLoadData::isEmpty()
{
	return m_file.peek() == ifstream::traits_type::eof();
}

/**
 * Parses one line of input file.
 * @return	Parsed line.
 */
vector<string> BTSLoadData::rawRow()
{
	vector<string> row;
	string cell;
	bool quote = false;
	char c;
	m_columnCount = 0;

	while (m_file.get(c)) {
		if (c == '\r') {
			// carriage return
			continue;
		}

		if (c == '\n') {
			// line feed
			if (quote) {
				cell += c;
			}
			else {
				// store cell into vector
				row.push_back(cell);
				if (m_columnCount == 0)
					m_columnCount = row.size();

				if (m_columnCount != row.size())
					throw CustomException("wrong number of columns in CSV");

				return row;
			}
		}
		else if (c == m_delimiter) {
			if (quote) {
				cell += c;
			}
			else {
				// store cell into vector
				row.push_back(cell);
				cell.clear();
			}
		}
		else if (c == '"') {
			quote = !quote;
		}
		else {
			cell += c;
		}
	}

	if (!cell.empty())
		row.push_back(cell);

	close();
	return row;
}
