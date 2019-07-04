#include "CustomException.h"

using namespace std;

CustomException::CustomException(const string &text):
	m_message(text)
{
}

/**
 * Returns exception message.
 * @return	Exception message.
 */
std::string CustomException::message() const
{
	return m_message;
}
