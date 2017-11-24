#pragma once

#include <vector>

#include "BTSData.h"

/**
 * Structure for coordinate and distance.
 */
struct PointAndDistance {
	double x;
	double y;
	double distance;
};

/**
 * Class for computing position of mobile station using Hata model.
 */
class Distance {
public:
	typedef std::pair<Poco::SharedPtr<BTSData>, double> BTSDistance;

	static double computeLu(double f, double hB, double CH, double d);
	static double smallOrMediumSizedCity(double f, double hB);
	static double largeCity(double f, double hB);
	static double computePathLoss(double power, double signal);

	static double computeDistance(double Lu, double f, double hB, double CH);

	Coordinates::Ptr findMS(const std::vector<BTSDistance> &nearBTS);

	std::vector<PointAndDistance> computeIntersection(
			PointAndDistance point1, PointAndDistance point2);
};
