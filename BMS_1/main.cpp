#include <iostream>
#include <map>

#include "BTSLoadData.h"
#include "CustomException.h"
#include "BTSData.h"
#include "Distance.h"

const int FREQUENCY = 900;
const double HEIGHT = 1.2;

using namespace std;

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

		// TODO chybalo
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

		countDistance(m_nearBTS, m_distance);

		Distance location;

		Coordinates::Ptr GPS = location.findMS(m_distance);
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
