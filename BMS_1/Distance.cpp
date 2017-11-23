#include <cmath>
#include <iostream>

#include "CustomException.h"
#include "Distance.h"
#include "UTM.h"

using namespace std;
using namespace Poco;

double Distance::countLu(double f, double hB, double CH, double d)
{
	d /= 1000;
	return 69.55 + 26.16 * log10(f) - 13.82 * log10(hB) - CH
		 + (44.9 - 6.55 * log10(hB)) * log10(d);
}

double Distance::smallOrMediumSizedCity(double f, double hB)
{
	return 0.8 + (1.1 * log10(f) - 0.7) * hB - 1.56 * log10(f);
}

double Distance::largeCities(double f, double hB)
{
	if (f >= 150 && f <= 200)
		return 8.29 * pow((log10(1.54 * hB)), 2) - 1.1;
	else if (f > 200 && f <= 1500)
		return 3.2 * pow((log10(11.75 * hB)), 2) - 4.97;

	throw CustomException("wrong frequency");
}

double Distance::countDistance(double Lu, double f, double hB, double CH)
{
	double numerator = Lu - 26.16 * log10(f) + 13.82 * log10(hB) + CH - 69.55;
	double denominator = 44.9 - 6.55 * log10(hB);

	return pow(10, numerator / denominator) * 1000;
}

double Distance::countPathLoss(double power, double signal)
{
	return 10 * log10(1000 * power) - signal;
}

Coordinates::Ptr Distance::findMS(
	const vector<Distance::BTSDistance> &nearBTS)
{
	vector<PointAndDistance> points;

	for (size_t i = 1; i <= nearBTS.size(); i++) {
		Coordinates::Ptr gps1 = nearBTS.at(i - 1).first->GPSCoordinate();

	double UTMNorthing;
	double UTMEasting;

	char UTMZone = UTM::UTMLetterDesignator(gps1->DDLatitude());

	UTM::LLtoUTM(gps1->DDLatitude(), gps1->DDLongitude(), UTMNorthing, UTMEasting, &UTMZone);

	const PointAndDistance p1 = {
		UTMEasting,
		UTMNorthing,
		nearBTS.at(i - 1).second
	};

	for (size_t j = 1; j < nearBTS.size(); j++) {
		const Coordinates::Ptr gps2 = nearBTS.at(j).first->GPSCoordinate();

		double UTMNorthing2;
		double UTMEasting2;

		char UTMZone2 = UTM::UTMLetterDesignator(gps2->DDLatitude());

		UTM::LLtoUTM(gps2->DDLatitude(), gps2->DDLongitude(), UTMNorthing2, UTMEasting2, &UTMZone2);

		const PointAndDistance p2 = {
				UTMEasting2,
				UTMNorthing2,
				nearBTS.at(j).second
			};

			for (const auto &it : countMS(p1, p2)) {
				if (std::isnan(it.x) || std::isnan(it.y))
					continue;

				points.push_back(it);
			}
		}
	}

	double x = 0;
	double y = 0;

	for (auto &it : points) {
		x += it.x;
		y += it.y;
	}

	x = x / points.size();
	y = y / points.size();

	Coordinates::Ptr coordinates = new Coordinates;

	double latitude;
	double longitude;

	UTM::UTMtoLL(y, x, "33U", latitude, longitude);

	coordinates->setDMSLatitude(latitude);
	coordinates->setDMSLongitude(longitude);

	return coordinates;
}

vector<PointAndDistance> Distance::countMS(
		PointAndDistance p1, PointAndDistance p2)
{
	vector<PointAndDistance> points;

	double ax = p1.x;
	double ay = p1.y;
	double ar = p1.distance;

	double bx = p2.x;
	double by = p2.y;
	double br = p2.distance;

	double d = sqrt(pow(ax - bx, 2) + pow(ay - by, 2));
	double m = (pow(ar, 2) - pow(br, 2)) / (2 * d) + (d / 2);
	double v = sqrt(abs(pow(ar, 2) - pow(m, 2)));

	double sx = ax + (m / d) * (bx - ax);
	double sy = ay + (m / d) * (by - ay);

	double cx = (v / d) * (ay - by);
	double cy = (v / d) * (ax - bx);

	points.push_back({sx + cx, sy - cy, 0});
	points.push_back({sx - cx, sy + cy, 0});

	return points;
}
