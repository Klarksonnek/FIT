#pragma once

#include <Poco/SharedPtr.h>

#include <string>
#include <vector>

#include "Coordinates.h"

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
	Coordinates::Ptr m_GPSCoordinate;
	double m_signal;
	double m_BTSHeight;
	double m_power;
};
