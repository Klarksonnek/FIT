#include <string>

#include "BTSData.h"

using namespace std;

BTSData::BTSData(int CID):
	m_CID(CID)
{
}

void BTSData::setGPSCoordinate(Coordinates::Ptr GPS)
{
	m_GPSCoordinate = GPS;
}

Coordinates::Ptr BTSData::GPSCoordinate() const
{
	return m_GPSCoordinate;
}

void BTSData::setSignal(double signal)
{
	m_signal = signal;
}

double BTSData::signal() const
{
	return m_signal;
}

void BTSData::setHeight(double height)
{
	m_BTSHeight = height;
}

double BTSData::height() const
{
	return m_BTSHeight;
}

void BTSData::setPower(double power)
{
	m_power = power;
}

double BTSData::power() const
{
	return m_power;
}
