#include <Poco/RegularExpression.h>
#include <Poco/StringTokenizer.h>

#include <cmath>
#include <iomanip>
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
 * Returns latitude expressed in Decimal Degrees (DD).
 * @return    Latitude in DD.
 */
double Coordinates::DDLatitude() const
{
	return m_latitude;
}

/**
 * Returns longitude expressed in DD.
 * @return    Longitude in DD.
 */
double Coordinates::DDLongitude() const
{
	return m_longitude;
}

/**
 * Sets latitude expressed in Degrees Minutes Seconds (DMS).
 * @param latitude    Latitude in DMS.
 */
void Coordinates::setDMSLatitude(double latitude)
{
	m_latitude = latitude;
}

/**
 * Sets longitude expressed in DMS.
 * @param longitude    Longitude in DMS.
 */
void Coordinates::setDMSLongitude(double longitude)
{
	m_longitude = longitude;
}

/**
 * Creates link to Google Maps (position of mobile station).
 * @return    Link to Google Maps.
 */
string Coordinates::GoogleMapLink() const
{
	return "https://www.google.com/maps/place/" + convertDDToDMSLatitude() + "+" + convertDDToDMSLongitude();
}

/**
 * Converts coordinates expressed in DMS to DD.
 * @param coordinates    GPS coordinates.
 * @return    Converted GPS coordinates.
 */
Coordinates::Ptr Coordinates::DMSToDD(const string &coordinates)
{
	StringTokenizer tokens(coordinates, ",");

	if (tokens.count() != 2)
		throw CustomException("wrong number of coordinates");

	Coordinates::Ptr gps = new Coordinates;
	gps->setDMSLatitude(convertDMSToDDLatitude(tokens[0]));
	gps->setDMSLongitude(convertDMSToDDLongitude(tokens[1]));

	return gps;
}

/**
 * Converts latitude expressed in DMS to DD.
 * @param data    Latitude.
 * @return    Converted latitude.
 */
double Coordinates::convertDMSToDDLatitude(const string &data)
{
	const RegularExpression re("([0-9]+)[^0-9]*([0-9]+)[^0-9]*([0-9]+.[0-9]+)[^0-9]*([N,S])");
	RegularExpression::MatchVec matchVector;

	if (re.match(data, 0, matchVector) != 5)
		throw CustomException("wrong format of DMS latitude");

	double latitude = 0;
	latitude += stof(data.substr(matchVector[1].offset, matchVector[1].length));
	latitude += stof(data.substr(matchVector[2].offset, matchVector[2].length)) / 60;
	latitude += stof(data.substr(matchVector[3].offset, matchVector[3].length)) / 3600;

	if (data.substr(matchVector[4].offset, matchVector[4].length) == "S")
		latitude *= -1;

	return latitude;
}

/**
 * Converts longitude expressed in DMS to DD.
 * @param data    Longitude.
 * @return    Converted longitude.
 */
double Coordinates::convertDMSToDDLongitude(const string &data)
{
	const RegularExpression re("([0-9]+)[^0-9]*([0-9]+)'([0-9]+.[0-9]+)[^0-9]*([E,W])");
	RegularExpression::MatchVec matchVector;

	if (re.match(data, 0, matchVector) != 5)
		throw CustomException("wrong format of DMS longitude");

	double longitude = 0;
	longitude += stof(data.substr(matchVector[1].offset, matchVector[1].length));
	longitude += stof(data.substr(matchVector[2].offset, matchVector[2].length)) / 60;
	longitude += stof(data.substr(matchVector[3].offset, matchVector[3].length)) / 3600;

	if (data.substr(matchVector[4].offset, matchVector[4].length) == "W")
		longitude *= -1;

	return longitude;
}

/**
 * Converts latitude expressed in DD to DMS.
 * @return    Converted latitude.
 */
string Coordinates::convertDDToDMSLatitude() const
{
	if (isnan(m_latitude))
		throw CustomException("wrong DD latitude");

	double latitude = m_latitude;

	if (latitude < 0)
		latitude *= -1;

	int deg = latitude;
	int min = (latitude - deg) * 60;
	double sec = (latitude - deg - min/60.0) * 3600;
	sec = roundf(sec * 100) / 100;
	std::ostringstream pomStr;
	pomStr << sec;
	std::string secStr = pomStr.str();
	secStr.substr(0, secStr.find('.') + 2);

	return to_string(deg) + "°" + to_string(min) + "'" + secStr + "\"" + ((m_latitude < 0) ? "S" : "N");
}

/**
 * Converts longitude expressed in DD to DMS.
 * @return    Converted longitude.
 */
string Coordinates::convertDDToDMSLongitude() const
{
	if (isnan(m_longitude))
		throw CustomException("wrong DD longitude");

	double longitude = m_longitude;

	if (longitude < 0)
		longitude *= -1;

	int deg = longitude;
	int min = (longitude - deg) * 60;
	double sec = (longitude - deg - min/60.0) * 3600;
	sec = roundf(sec * 100) / 100;
	std::ostringstream pomStr;
	pomStr << sec;
	std::string secStr = pomStr.str();
	secStr.substr(0, secStr.find('.') + 2);

	return to_string(deg) + "°" + to_string(min) + "'" + secStr + "\"" + ((m_longitude < 0) ? "W" : "E");
}
