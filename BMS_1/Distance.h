#pragma once

#include <list>
#include <vector>

#include "BTSData.h"

struct PointAndDistance {
	double x;
	double y;
	double distance;
};

class Distance {
public:
	typedef std::pair<Poco::SharedPtr<BTSData>, double> BTSDistance;

	static double countLu(double f, double hB, double CH, double d);
	static double smallOrMediumSizedCity(double f, double hB);
	static double largeCities(double f, double hB);
	static double countPathLoss(double power, double signal);

	static double countDistance(double Lu, double f, double hB, double CH);

	Coordinates::Ptr findMS(const std::vector<BTSDistance> &nearBTS);

	std::vector<PointAndDistance> countMS(
	PointAndDistance point1, PointAndDistance point2);
};
