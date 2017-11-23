#include "BTSLoadData.h"
#include "CustomException.h"

using namespace std;

BTSLoadData::BTSLoadData()
{
}

bool BTSLoadData::open(const string &file, char delimiter)
{
	m_file.open(file.c_str());
	m_delimiter = delimiter;

	return m_file.is_open();
}

void BTSLoadData::close()
{
	m_file.close();
}

bool BTSLoadData::isEOF() const
{
	return m_file.eof();
}

vector<string> BTSLoadData::rawRow()
{
	vector<string> row;
	string cell;
	bool quote = false;
	char c;
	m_columnCount = 0;

	while (m_file.get(c)) {
		if (c == '\r') {
			continue;
		}

		if ('\n' == c) {
			if (quote) {
				cell += c;
			}
			else {
				row.push_back(cell);
				if (m_columnCount == 0)
					m_columnCount = row.size();

				if (m_columnCount != row.size()) {
					throw CustomException("wrong number of columns in CSV");
				}

				return row;
			}
		}
		else if (c == m_delimiter) {
			if (quote) {
				cell += c;
			}
			else {
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
