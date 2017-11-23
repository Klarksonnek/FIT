#pragma once

#include <fstream>
#include <string>
#include <vector>

class BTSLoadData {
public:
	BTSLoadData();
	bool open(const std::string &file, char delimiter);
	void close();
	bool isEOF() const;
	std::vector<std::string> rawRow();

private:
	std::ifstream m_file;
	char m_delimiter;
	unsigned int m_columnCount;
};
