#include <iostream>
#include <map>

#include "BTSData.h"
#include "BTSLoadData.h"
#include "CustomException.h"
#include "Distance.h"

const int FREQUENCY = 900;
const double HEIGHT = 1.2;

using namespace std;

/**
 * Initializes list of BTS.
 * @param   BTSFile     File name containing list of BTS.
 * @param   BTSInfo     Information about BTS.
 * @return  True if initialization is successful, false otherwise.
 */
bool init(const string &BTSFile, map<BTSData::BTSKey, BTSData::Ptr> &BTSInfo)
{
	BTSLoadData data;

	if (!data.open(BTSFile, ';')) {
		cerr << "file " + BTSFile + " with list of BTS cannot be loaded" << endl;
		return false;
	}

    if (BTSFile.empty()) {
        cerr << "no list of BTS in file" << endl;
        return false;
    }

	data.rawRow();

	// process list of BTS and store CID and GPS coordinates
	while (!data.isEOF()) {
		vector<string> row = data.rawRow();

		if (row.empty())
			continue;

		int CID = stoi(row[0], nullptr, 10);

		// save GPS coordinates of BTS
		BTSData::Ptr BTS = new BTSData(CID);
		BTS->setGPSCoordinate(Coordinates::DegMinSecToDecDeg(row[4]));

		BTSData::BTSKey btsKey;
		btsKey = CID;
		BTSInfo.emplace(btsKey, BTS);
	}

	return true;
}

/**
 * Initializes BTS from input file.
 * @param   nearBTSFile     File name containing list of near BTS.
 * @param   BTSInfo         Information about BTS.
 * @param   nearBTSInfo     Information about near BTS.
 * @return  True if initialization is successful, false otherwise.
 */
bool initNearBTS(const string &nearBTSFile, map<BTSData::BTSKey, BTSData::Ptr> &BTSInfo, vector<BTSData::Ptr> &nearBTSInfo)
{
	BTSLoadData data;

	if (!data.open(nearBTSFile, ';')) {
		cerr << "file " + nearBTSFile + " with list of BTS cannot be loaded" << endl;
		return false;
	}

    if (data.isEOF()) {
        cerr << "no information about near BTS in input file" << endl;
        return false;
    }

	data.rawRow();

	while (!data.isEOF()) {
		vector<string> row = data.rawRow();

		if (row.size() == 0)
			continue;

		int CID = stoi(row[1], nullptr, 10);

		bool found = false;
		// store more detailed information about near BTS
		for (auto &it : BTSInfo) {
			if (it.first == CID) {
				it.second->setSignal(stoi(row[3], nullptr, 10));
				it.second->setHeight(stof(row[4], nullptr));
				it.second->setPower(stoi(row[5], nullptr, 10));

				nearBTSInfo.push_back(it.second);
				found = true;

				break;
			}
		}

		if (!found) {
			cerr << "no BTS was found" << endl;
			continue;
		}
	}

	return true;
}

/**
 * Computes distance between base and mobile stations using Hata model.
 * @param nearBTSInfo    Information about near BTS.
 * @param distance       Distance between base and mobile stations.
 */
void computeDistance(vector<BTSData::Ptr> &nearBTSInfo, vector<Distance::BTSDistance> &distance)
{
	for (auto BTS : nearBTSInfo) {
		double Lu = Distance::computePathLoss(BTS->power(), BTS->signal());
		double dist = Distance::computeDistance(
                Lu, FREQUENCY, BTS->height(),
                Distance::smallOrMediumSizedCity(FREQUENCY, HEIGHT));
		distance.push_back({BTS, dist});
	}
}

/**
 * Creates and saves file containing link to Google Maps with reference to the calculated coordinates.
 * @param GPS      GPS coordinates.
 */
void save(Coordinates::Ptr GPS)
{
	string outputFile = "out.txt";
	ofstream output(outputFile);

	if(output.is_open()) {
		output << GPS->GoogleMapLink() << endl;
		output.close();
	}
	else {
		cerr << "GPS coordinates of mobile station were not saved" << endl;
	}
}

/**
 * Main program.
 */
int main(int argc, char* argv[])
{
	if (argc != 2) {
		cerr <<  "file containing information about BTS was not entered" << endl;
	}

	map<BTSData::BTSKey, BTSData::Ptr> BTSInfo;
	vector<BTSData::Ptr> nearBTSInfo;
	vector<Distance::BTSDistance> distance;

	try {
		// load input file and file containing BTS
		if (!init("bts.csv", BTSInfo) || !initNearBTS(argv[1], BTSInfo, nearBTSInfo))
			return EXIT_FAILURE;

		// compute distances between mobile and base stations
		computeDistance(nearBTSInfo, distance);

		// find GPS coordinates of mobile station
		Distance location;
		Coordinates::Ptr GPS = location.findMS(distance);

		// save link to Google Maps
		save(GPS);
	}
	catch (const CustomException &ex) {
		cerr << ex.message() << endl;
		return EXIT_FAILURE;
	}
	catch (...) {
		cerr << "unknown error" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
