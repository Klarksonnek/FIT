#include <cppunit/extensions/HelperMacros.h>

#include <cmath>

#include "Coordinates.h"

using namespace std;

static const string csvPath = "./csv/";

int main()
{
	// TEST 1
	auto gps1 = Coordinates::DMSToDD("49°11'23.10\"N,16°32'13.38\"E");

	if(fabs(gps1->DDLatitude() - 49.18975) > pow(10, -6))
		cout << "wrong DD lati" << endl;

	if(fabs(gps1->DDLongitude() - 16.53705) > pow(10, -6))
		cout << "wrong DD longi" << endl;

	cout << "DMS lati " << gps1->convertDDToDMSLatitude() << endl;
	cout << "DMS longi " << gps1->convertDDToDMSLongitude() << endl;

	if ((gps1->convertDDToDMSLatitude() != "49°11'23.10\"N") || (gps1->convertDDToDMSLongitude() != "16°32'13.38\"E"))
		cout << "wrong DMS lati or Longi" << endl;

	auto gps2 = Coordinates::DMSToDD("49°11'23.10\"S,16°32'13.38\"W");

	if(fabs(gps1->DDLatitude() - 49.18975) > pow(10, -6))
		cout << "wrong DD lati" << endl;
	if(fabs(gps1->DDLongitude() - 16.53705) > pow(10, -6))
		cout << "wrong DD longi" << endl;

	cout << "DMS lati " << gps2->convertDDToDMSLatitude() << endl;
	cout << "DMS longi " << gps2->convertDDToDMSLongitude() << endl;

	if (gps1->convertDDToDMSLatitude() != "49°11'23.10\"S" || (gps1->convertDDToDMSLongitude() != "16°32'13.38\"W"))
		cout << "wrong DMS lati or Longi" << endl;


	// TEST 2
	auto gps = Coordinates::DMSToDD("49�11'23.10N,16�32'13.38E");

	if(fabs(gps1->DDLatitude() - 49.18975) > pow(10, -6))
		cout << "wrong DD lati" << endl;
	if(fabs(gps1->DDLongitude() - 16.53705) > pow(10, -6))
		cout << "wrong DD longi" << endl;

	gps = Coordinates::DMSToDD("49�11'23.10S,16�32'13.38W");

	if(fabs(gps1->DDLatitude() - 49.18975) > pow(10, -6))
		cout << "wrong DD lati" << endl;
	if(fabs(gps1->DDLongitude() - 16.53705) > pow(10, -6))
		cout << "wrong DD longi" << endl;

	return 0;
}
