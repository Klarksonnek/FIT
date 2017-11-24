#pragma once

#include <Poco/SharedPtr.h>

#include "Coordinates.h"

/**
 * Class containing information about near BTS that are
 * stored in input file.
 */
class BTSData {
public:
	typedef int BTSKey;
	typedef Poco::SharedPtr<BTSData> Ptr;

	BTSData(int CID);

	void setGPSCoordinate(Coordinates::Ptr GPS);
	Coordinates::Ptr GPSCoordinate() const;

	void setSignal(double signal);
	double signal() const;

	void setHeight(double height);
	double height() const;

	void setPower(double power);
	double power() const;

private:
	int m_CID;
	Coordinates::Ptr m_GPS;
	double m_signal;
	double m_height;
	double m_power;
};
