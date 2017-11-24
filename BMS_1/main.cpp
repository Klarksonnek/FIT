#include <iostream>
#include <map>

#include "BTSLoadData.h"
#include "CustomException.h"
#include "BTSData.h"
#include "Distance.h"

const int FREQUENCY = 900;
const double HEIGHT = 1.2;

using namespace std;

/**
 * Initializes list of BTS stations.
 * @param m_BTSFile    File name containing list of BTS.
 * @param m_BTSData    Information about BTS.
 * @return    True if initialization is successful, false otherwise.
 */
bool init(const string &m_BTSFile, map<BTSData::BTSKey, BTSData::Ptr> &m_BTSData)
{
	BTSLoadData data;

	if (!data.open(m_BTSFile, ';')) {
		cerr << "file " + m_BTSFile + " with list of BTS cannot be loaded" << endl;
		return false;
	}

	data.rawRow();

	while (!data.isEOF()) {
		vector<string> row = data.rawRow();

		if (row.empty())
			continue;

		int CID = stoi(row[0], nullptr, 10);

		BTSData::Ptr bts = new BTSData(CID);
		bts->setGPSCoordinate(Coordinates::DMSToDD(row[4]));

		BTSData::BTSKey btsKey;
		btsKey = CID;
		m_BTSData.emplace(btsKey, bts);
	}

	return true;
}

/**
 * Initializes BTS from input file.
 * @param m_nearBTSFIle    File name containing list of near BTS.
 * @param m_BTSData    Information about BTS.
 * @param m_nearBTS    Information about near BTS.
 * @return   True if initialization is successful, false otherwise.
 */
bool initNearBTS(const string &m_nearBTSFIle, map<BTSData::BTSKey, BTSData::Ptr> &m_BTSData, vector<BTSData::Ptr> &m_nearBTS)
{
	BTSLoadData data;

	if (!data.open(m_nearBTSFIle, ';')) {
		cerr << "file " + m_nearBTSFIle + " with list of BTS cannot be loaded" << endl;
		return false;
	}

	data.rawRow();

	while (!data.isEOF()) {
		vector<string> row = data.rawRow();

		if (row.size() == 0)
			continue;

		int CID = stoi(row[1], nullptr, 10);

		bool found = false;
		for (auto &it : m_BTSData) {
			if (it.first == CID) {
				it.second->setSignal(stoi(row[3], nullptr, 10));
				it.second->setHeight(stof(row[4], nullptr));
				it.second->setPower(stoi(row[5], nullptr, 10));

				m_nearBTS.push_back(it.second);
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
 * Counts distance between base and mobile stations.
 * @param m_nearBTS    Information about near BTS.
 * @param m_distance    Distance between base and mobile stations.
 */
void countDistance(vector<BTSData::Ptr> &m_nearBTS, vector<Distance::BTSDistance> &m_distance)
{
	for (auto BTS : m_nearBTS) {
		double Lu = Distance::countPathLoss(BTS->power(), BTS->signal());
		double distance = Distance::countDistance(
			Lu, FREQUENCY, BTS->height(),
			Distance::smallOrMediumSizedCity(FREQUENCY, HEIGHT));
		m_distance.push_back({BTS, distance});
	}
}

/**
 * Creates and saves file containing link to Google Maps with reference * to the calculated coordinates.
 * @param GPS
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
		cerr << "GPS coordinates of mobile station was not save" << endl;
	}
}

// TODO - otestovat ci vsetko funguje ako ma
// TODO - premenovat niektore premenne a niektore premenne v niektorych triedach
// TODO - upravit Makefile pre testy

/**
 * Main program.
 */
int main(int argc, char* argv[])
{
	if (argc != 2) {
		cerr <<  "file containing information about BTS was not entered" << endl;
	}

	map<BTSData::BTSKey, BTSData::Ptr> m_BTSData;
	vector<BTSData::Ptr> m_nearBTS;
	vector<Distance::BTSDistance> m_distance;

	try {
		if (!init("bts.csv", m_BTSData) || !initNearBTS(argv[1], m_BTSData, m_nearBTS))
			return EXIT_FAILURE;

		//TODO: check if input file is not empty
		countDistance(m_nearBTS, m_distance);

		Distance location;

		Coordinates::Ptr GPS = location.findMS(m_distance);

		// TODO: check if GPS coordinates are valid
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
