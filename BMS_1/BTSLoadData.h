#pragma once

#include <fstream>
#include <string>
#include <vector>

/**
 * Class for file processing.
 */
class BTSLoadData {
public:
	BTSLoadData();

	bool open(const std::string &file, char delimiter);
	void close();
	bool isEOF() const;
	bool isEmpty();

	std::vector<std::string> rawRow();

private:
	std::ifstream m_file;
	char m_delimiter;
	unsigned int m_columnCount;
};
