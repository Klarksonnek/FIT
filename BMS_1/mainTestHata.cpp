#include <cmath>
#include <iostream>
#include "Distance.h"

using namespace std;

static const double f = 900;
static const double hB = 1.5;

int main()
{
	// TEST 1
	double Lu = Distance::countLu(
		f,
		25,
		Distance::smallOrMediumSizedCity(f, hB),
		10000
	);
	if (fabs(163.2410642444338 - Lu) > pow(10, -12))
		cout << "OK Lu small or medium city" << endl;


	// TEST 2
	Lu = Distance::countLu(
		f,
		25,
		Distance::largeCities(f, hB),
		10000
	);

	if(fabs(163.25786511723783 - Lu) > pow(10, -12))
		cout << "OK Lu large cities" << endl;


	// TEST 3
	Lu = Distance::countPathLoss(10, -60);

	const double solvedDistance = Distance::countDistance(
		Lu,
		f,
		25,
		Distance::smallOrMediumSizedCity(f, hB)
	);

	if(fabs(170.097190844304 - solvedDistance) > pow(10, -12))
		cout << "OK path loss small or medium city" << endl;


	// TEST 4
	const double Lu2 = Distance::countPathLoss(10, -60);
	const double Lu1 = Distance::countLu(
		f,
		25,
		Distance::smallOrMediumSizedCity(f, hB),
		170
	);

	if(fabs(Lu1 - Lu2) > pow(10, -2))
		cout << "wrong Lu = path loss small or medium city" << endl;

	// TEST 5
	Distance loc;

	PointAndDistance p1 = {1, 1, 2};
	PointAndDistance p2 = {1, 3, 2};

	const auto p3 = loc.countMS(p1, p2);
	if(2 != (int) p3.size())
		cout << "wrong position of MS" << endl;

	if(fabs(-0.732051 - p3.at(0).x) > pow(10, 5))
		cout << "wrong x in p3[0]" << endl;
	if(fabs(2 - p3.at(0).y) > pow(10, 5))
		cout << "wrong y in p3[0]" << endl;

	if(fabs(2.73205 - p3.at(1).x) > pow(10, 5))
		cout << "wrong x in p3[1]" << endl;
	if(fabs(2 - p3.at(1).y) > pow(10, 5))
		cout << "wrong x in p3[1]" << endl;
}
