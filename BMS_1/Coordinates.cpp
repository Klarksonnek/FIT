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

double Coordinates::DDLatitude() const
{
	return m_latitude;
}

double Coordinates::DDLongitude() const
{
	return m_longitude;
}

void Coordinates::setDMSLatitude(double latitude)
{
	m_latitude = latitude;
}

void Coordinates::setDMSLongitude(double longitude)
{
	m_longitude = longitude;
}

string Coordinates::GoogleMapLink() const
{
	return "https://www.google.com/maps/place/" + convertDDToDMSLatitude() + convertDDToDMSLongitude();
}

Coordinates::Ptr Coordinates::DMSToDD(const string &coordinates)
{
	StringTokenizer tokens(coordinates, ",");

	if (tokens.count() != 2)
		throw CustomException("nespravny pocet parametrov v suradniciach");

	Coordinates::Ptr gps = new Coordinates;
	gps->setDMSLatitude(convertDMSToDDLatitude(tokens[0]));
	gps->setDMSLongitude(convertDMSToDDLongitude(tokens[1]));

	return gps;
}

double Coordinates::convertDMSToDDLatitude(const string &data)
{
	const RegularExpression re("([0-9]+)[^0-9]*([0-9]+)[^0-9]*([0-9]+.[0-9]+)[^0-9]*([N,S])");
	RegularExpression::MatchVec matchVector;

	if (re.match(data, 0, matchVector) != 5)
		throw CustomException("nejaky text");

	double latitude = 0;
	latitude += stof(data.substr(matchVector[1].offset, matchVector[1].length));
	latitude += stof(data.substr(matchVector[2].offset, matchVector[2].length)) / 60;
	latitude += stof(data.substr(matchVector[3].offset, matchVector[3].length)) / 3600;

	if (data.substr(matchVector[4].offset, matchVector[4].length) == "S")
		latitude *= -1;

	return latitude;
}

double Coordinates::convertDMSToDDLongitude(const string &data)
{
	const RegularExpression re("([0-9]+)[^0-9]*([0-9]+)'([0-9]+.[0-9]+)[^0-9]*([E,W])");
	RegularExpression::MatchVec matchVector;

	if (re.match(data, 0, matchVector) != 5)
		throw CustomException("Nepalatne suradnice");

	double longitude = 0;
	longitude += stof(data.substr(matchVector[1].offset, matchVector[1].length));
	longitude += stof(data.substr(matchVector[2].offset, matchVector[2].length)) / 60;
	longitude += stof(data.substr(matchVector[3].offset, matchVector[3].length)) / 3600;

	if (data.substr(matchVector[4].offset, matchVector[4].length) == "W")
		longitude *= -1;

	return longitude;
}

string Coordinates::convertDDToDMSLatitude() const
{
	if (isnan(m_latitude))
		throw CustomException("Neplatna suradnica");

	double latitude = m_latitude;

	if (latitude < 0)
		latitude *= -1;

	int deg = latitude;
	int min = (latitude - deg) * 60;
	double sec = (latitude - deg - min/60.0) * 3600;

	return to_string(deg) + "°" + to_string(min) + "'" + to_string(sec) + "\"" + ((m_latitude < 0) ? "S" : "N");
}

string Coordinates::convertDDToDMSLongitude() const
{
	if (isnan(m_longitude))
		throw CustomException("neplatna suradnica ");

	double longitude = m_longitude;

	if (longitude < 0)
		longitude *= -1;

	int deg = longitude;
	int min = (longitude - deg) * 60;
	double sec = (longitude - deg - min/60.0) * 3600;

	return to_string(deg) + "°" + to_string(min) + "'" + to_string(sec) + "\"" + ((m_longitude < 0) ? "W" : "E");
}
