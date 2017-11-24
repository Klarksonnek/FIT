#include <Poco/RegularExpression.h>
#include <Poco/StringTokenizer.h>

#include <sstream>

#include "Coordinates.h"
#include "CustomException.h"

using namespace Poco;
using namespace std;

Coordinates::Coordinates():
	m_latitude(NAN),
	m_longitude(NAN)
{
}

/**
 * Returns latitude expressed in decimal degrees.
 * @return	Latitude in decimal degrees.
 */
double Coordinates::DecDegLatitude() const
{
	return m_latitude;
}

/**
 * Returns longitude expressed in decimal degrees.
 * @return	Longitude in decimal degrees.
 */
double Coordinates::DecDegLongitude() const
{
	return m_longitude;
}

/**
 * Sets latitude expressed in degrees minutes seconds.
 * @param latitude		Latitude in degrees minutes seconds.
 */
void Coordinates::setDegMinSecLatitude(double latitude)
{
	m_latitude = latitude;
}

/**
 * Sets longitude expressed in degrees minutes seconds.
 * @param longitude		Longitude in degrees minutes seconds.
 */
void Coordinates::setDegMinSecLongitude(double longitude)
{
	m_longitude = longitude;
}

/**
 * Creates link to Google Maps (position of mobile station).
 * @return	Link to Google Maps.
 */
string Coordinates::GoogleMapLink() const
{
	return "https://www.google.com/maps/place/" + convertDecDegToDegMinSecLatitude() + "+" +
			convertDecDegToDegMinSecLongitude();
}

/**
 * Converts coordinates expressed in degrees minutes seconds to decimal degrees.
 * @param 	GPS    GPS coordinates.
 * @return	Converted GPS coordinates.
 */
Coordinates::Ptr Coordinates::DegMinSecToDecDeg(const string &GPS)
{
	StringTokenizer tokens(GPS, ",");

	if (tokens.count() != 2)
		throw CustomException("wrong number of GPS");

	Coordinates::Ptr gps = new Coordinates;
	gps->setDegMinSecLatitude(convertDegMinSecToDecDegLatitude(tokens[0]));
	gps->setDegMinSecLongitude(convertDegMinSecToDecDegLongitude(tokens[1]));

	return gps;
}

/**
 * Converts latitude expressed in degrees minutes seconds to decimal degrees.
 * @param 	lat    Latitude.
 * @return 	Converted latitude.
 */
double Coordinates::convertDegMinSecToDecDegLatitude(const string &lat)
{
	const RegularExpression re("([0-9]+)[^0-9]*([0-9]+)[^0-9]*([0-9]+.[0-9]+)[^0-9]*([N,S])");
	RegularExpression::MatchVec matchVector;

	if (re.match(lat, 0, matchVector) != 5)
		throw CustomException("wrong latitude in degrees minutes seconds");

	double latitude = 0;
	latitude += stof(lat.substr(matchVector[1].offset, matchVector[1].length));
	latitude += stof(lat.substr(matchVector[2].offset, matchVector[2].length)) / 60;
	latitude += stof(lat.substr(matchVector[3].offset, matchVector[3].length)) / 3600;

	if (lat.substr(matchVector[4].offset, matchVector[4].length) == "S")
		latitude *= -1;

	return latitude;
}

/**
 * Converts longitude expressed in degrees minutes seconds to decimal degrees.
 * @param 	longi    Longitude.
 * @return	Converted longitude.
 */
double Coordinates::convertDegMinSecToDecDegLongitude(const string &longi)
{
	const RegularExpression re("([0-9]+)[^0-9]*([0-9]+)'([0-9]+.[0-9]+)[^0-9]*([E,W])");
	RegularExpression::MatchVec matchVector;

	if (re.match(longi, 0, matchVector) != 5)
		throw CustomException("wrong longitude in degrees minutes seconds");

	double longitude = 0;
	longitude += stof(longi.substr(matchVector[1].offset, matchVector[1].length));
	longitude += stof(longi.substr(matchVector[2].offset, matchVector[2].length)) / 60;
	longitude += stof(longi.substr(matchVector[3].offset, matchVector[3].length)) / 3600;

	if (longi.substr(matchVector[4].offset, matchVector[4].length) == "W")
		longitude *= -1;

	return longitude;
}

/**
 * Converts latitude expressed in decimal degrees to degrees minutes seconds.
 * @return	Converted latitude.
 */
string Coordinates::convertDecDegToDegMinSecLatitude() const
{
	if (isnan(m_latitude))
		throw CustomException("wrong latitude in decimal degrees");

	double latitude = m_latitude;

	if (latitude < 0)
		latitude *= -1;

	int deg = latitude;
	int min = (latitude - deg) * 60;
	double sec = (latitude - deg - min/60.0) * 3600;
	// round seconds to two decimal places
	sec = roundf(sec * 100) / 100;
	std::ostringstream strTmp;
	strTmp << sec;
	std::string secStr = strTmp.str();
	secStr.substr(0, secStr.find('.') + 2);

	return to_string(deg) + "°" + to_string(min) + "'" + secStr + "\"" + ((m_latitude < 0) ? "S" : "N");
}

/**
 * Converts longitude expressed in decimal degrees to degrees minutes seconds.
 * @return	Converted longitude.
 */
string Coordinates::convertDecDegToDegMinSecLongitude() const
{
	if (isnan(m_longitude))
		throw CustomException("wrong longitude in decimal degrees");

	double longitude = m_longitude;

	if (longitude < 0)
		longitude *= -1;

	int deg = longitude;
	int min = (longitude - deg) * 60;
	double sec = (longitude - deg - min/60.0) * 3600;
	// round seconds to two decimal places
	sec = roundf(sec * 100) / 100;
	std::ostringstream strTmp;
	strTmp << sec;
	std::string secStr = strTmp.str();
	secStr.substr(0, secStr.find('.') + 2);

	return to_string(deg) + "°" + to_string(min) + "'" + secStr + "\"" + ((m_longitude < 0) ? "W" : "E");
}
