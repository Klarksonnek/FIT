#pragma once

#include <exception>
#include <string>

class CustomException : public std::exception {
public:
	CustomException(const std::string &text);

	std::string message() const;

private:
	std::string m_message;
};