#include "CustomException.h"
#include "Distance.h"
#include "UTM.h"

using namespace std;
using namespace Poco;

/**
 * Computes path loss in urban areas.
 * @param 	f		Frequency of transmission.
 * @param 	hB		Height of base station antenna.
 * @param 	CH		Antenna height correction factor.
 * @param 	d 		Distance between the base and mobile stations.
 * @return 	Path loss in urban areas.
 */
double Distance::computeLu(double f, double hB, double CH, double d)
{
	// convert meters to kilometers
	d /= 1000;
	return 69.55 + 26.16 * log10(f) - 13.82 * log10(hB) - CH
		 + (44.9 - 6.55 * log10(hB)) * log10(d);
}

/**
 * Computes antenna height correction factor for small or medium-sized city.
 * @param 	f		Frequency of transmission.
 * @param 	hB		Height of base station antenna.
 * @return	Antenna height correction factor (small or medium-sized city).
 */
double Distance::smallOrMediumSizedCity(double f, double hB)
{
	return 0.8 + (1.1 * log10(f) - 0.7) * hB - 1.56 * log10(f);
}

/**
 * Computes antenna height correction factor for large city.
 * @param 	f		Frequency of transmission.
 * @param 	hB		Height of base station antenna.
 * @return	Antenna height correction factor (large city).
 */
double Distance::largeCity(double f, double hB)
{
	if (f >= 150 && f <= 200)
		return 8.29 * pow((log10(1.54 * hB)), 2) - 1.1;
	else if (f > 200 && f <= 1500)
		return 3.2 * pow((log10(11.75 * hB)), 2) - 4.97;

	throw CustomException("wrong frequency");
}

/**
 * Computes distance between base and mobile stations.
 * @param 	Lu		Path loss in urban areas.
 * @param 	f		Frequency of transmission.
 * @param 	hB 		Height of base station antenna.
 * @param 	CH		Antenna height correction factor.
 * @return 	Distance between base and mobile stations.
 */
double Distance::computeDistance(double Lu, double f, double hB, double CH)
{
	double numerator = Lu - 26.16 * log10(f) + 13.82 * log10(hB) + CH - 69.55;
	double denominator = 44.9 - 6.55 * log10(hB);

	// convert distance in kilometers to meters
	return pow(10, numerator / denominator) * 1000;
}

/**
 * Computes path loss.
 * @param 	power		BTS power.
 * @param 	signal		Signal measured on mobile station.
 * @return 	Path loss.
 */
double Distance::computePathLoss(double power, double signal)
{
	return 10 * log10(1000 * power) - signal;
}

/**
 * Finds coordinates of mobile station.
 * @param 	nearBTSDist		Distances to near BTS.
 * @return	Coordinates of mobile station.
 */
Coordinates::Ptr Distance::findMS(
	const vector<Distance::BTSDistance> &nearBTSDist)
{
	vector<PointAndDistance> points;

	for (size_t i = 1; i <= nearBTSDist.size(); i++) {
		Coordinates::Ptr GPS1 = nearBTSDist.at(i - 1).first->GPSCoordinate();

	double UTMNorth;
	double UTMEast;

	char UTMZone = UTM::UTMLetterDesignator(GPS1->DecDegLatitude());

	// convert latitude and longitude to UTM coordinates
	UTM::LLtoUTM(GPS1->DecDegLatitude(), GPS1->DecDegLongitude(), UTMNorth, UTMEast, &UTMZone);

	const PointAndDistance p1 = {
		UTMEast,
		UTMNorth,
		nearBTSDist.at(i - 1).second
	};

	for (size_t j = 1; j < nearBTSDist.size(); j++) {
		const Coordinates::Ptr GPS2 = nearBTSDist.at(j).first->GPSCoordinate();

		double UTMNorth2;
		double UTMEast2;

		char UTMZone2 = UTM::UTMLetterDesignator(GPS2->DecDegLatitude());

		// convert latitude and longitude to UTM coordinates
		UTM::LLtoUTM(GPS2->DecDegLatitude(), GPS2->DecDegLongitude(), UTMNorth2, UTMEast2, &UTMZone2);

		const PointAndDistance p2 = {
				UTMEast2,
				UTMNorth2,
				nearBTSDist.at(j).second
			};

			for (const auto &it : computeIntersection(p1, p2)) {
				// compute intersections with other circles
				if (std::isnan(it.x) || std::isnan(it.y))
					continue;

				points.push_back(it);
			}
		}
	}

	// compute average of circle intersections
	double x = 0;
	double y = 0;

	for (auto &it : points) {
		x += it.x;
		y += it.y;
	}

	x = x / points.size();
	y = y / points.size();

	Coordinates::Ptr GPS = new Coordinates;

	double latitude;
	double longitude;

	// convert UTM coordinates to latiude and longitude
	UTM::UTMtoLL(y, x, "33U", latitude, longitude);

	if (isnan(latitude) || isnan(longitude))
		throw CustomException("GPS coordinates of mobile station are not valid");

	GPS->setDegMinSecLatitude(latitude);
	GPS->setDegMinSecLongitude(longitude);

	return GPS;
}

/**
 * Computes point of intersection of two circles.
 * @param 	p1		Coordinates of point 1 in UTM.
 * @param 	p2		Coordinates of point 2 in UTM.
 * @return	Point of intersection.
 */
vector<PointAndDistance> Distance::computeIntersection(
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
	double h = sqrt(abs(pow(ar, 2) - pow(m, 2)));

	double sx = ax + (m / d) * (bx - ax);
	double sy = ay + (m / d) * (by - ay);

	double cx = (h / d) * (ay - by);
	double cy = (h / d) * (ax - bx);

	points.push_back({sx + cx, sy - cy, 0});
	points.push_back({sx - cx, sy + cy, 0});

	return points;
}
