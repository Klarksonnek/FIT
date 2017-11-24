#include <string>

#include "BTSData.h"

using namespace std;

BTSData::BTSData(int CID):
	m_CID(CID)
{
}

/**
 * Sets GPS coordinates.
 * @param GPS    GPS coordinates.
 */
void BTSData::setGPSCoordinate(Coordinates::Ptr GPS)
{
	m_GPSCoordinate = GPS;
}

/**
 * Returns GPS coordinates.
 * @return    GPS coordinates.
 */
Coordinates::Ptr BTSData::GPSCoordinate() const
{
	return m_GPSCoordinate;
}

/**
 * Sets signal measured on mobile station.
 * @param signal    Signal measured on mobile station.
 */
void BTSData::setSignal(double signal)
{
	m_signal = signal;
}

/**
 * Returns signal measured on mobile station.
 * @return    Signal measured on mobile station.
 */
double BTSData::signal() const
{
	return m_signal;
}

/**
 * Sets height of BTS antenna.
 * @param height    Heigth of BTS antenna.
 */
void BTSData::setHeight(double height)
{
	m_BTSHeight = height;
}

/**
 * Returns heigth of BTS antenna.
 * @return    Heigth of BTS antenna.
 */
double BTSData::height() const
{
	return m_BTSHeight;
}

/**
 * Sets BTS power.
 * @param power    Power.
 */
void BTSData::setPower(double power)
{
	m_power = power;
}

/**
 * Return BTS power.
 * @return    Power.
 */
double BTSData::power() const
{
	return m_power;
}
