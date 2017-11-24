#pragma once

#include <Poco/SharedPtr.h>

#include <string>

/**
 *  Class for converting GPS coordinates expressed in decimal degrees format
 *  to degrees minutes seconds format and vice versa. It also creates
 *  link to Google Maps.
 */
class Coordinates {
public:
	typedef Poco::SharedPtr<Coordinates> Ptr;

	Coordinates();

	void setDegMinSecLatitude(double latitude);
	void setDegMinSecLongitude(double longitude);

	double DecDegLatitude() const;
	double DecDegLongitude() const;

	std::string GoogleMapLink() const;

	static Coordinates::Ptr DegMinSecToDecDeg(const std::string &GPS);

	std::string convertDecDegToDegMinSecLatitude() const;
	std::string convertDecDegToDegMinSecLongitude() const;

private:
	double m_latitude;
	double m_longitude;

	static double convertDegMinSecToDecDegLatitude(const std::string &data);
	static double convertDegMinSecToDecDegLongitude(const std::string &data);
};
