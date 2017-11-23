#pragma once

#include <Poco/SharedPtr.h>

#include <list>
#include <string>
#include <vector>

class Coordinates {
public:
	typedef Poco::SharedPtr<Coordinates> Ptr;

	Coordinates();

	void setDMSLatitude(double latitude);
	void setDMSLongitude(double longitude);

	double DDLatitude() const;
	double DDLongitude() const;

	std::string GoogleMapLink() const;

	static Coordinates::Ptr DMSToDD(const std::string &coordinates);

	std::string convertDDToDMSLatitude() const;
	std::string convertDDToDMSLongitude() const;

private:
	double m_latitude;
	double m_longitude;

	static double convertDMSToDDLatitude(const std::string &data);
	static double convertDMSToDDLongitude(const std::string &data);
};
