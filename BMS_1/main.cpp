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
		cerr << "File " + m_BTSFile + " with list of BTS cannot be loaded." << endl;
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
		cerr << "File " + m_nearBTSFIle + " with list of BTS cannot be loaded." << endl;
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

// TODO - opravit texty vo vynimkach
// TODO - doplnit este nacitanie argumentov programu
// TODO - zamysliet sa ako osetrit neplatny vstup v stoi a podobne pri parsovani vstupneho suboru
// TODO - otestovat ci vsetko funguje ako ma
// TODO - pridat generovanie vystupneho suboru, kde bude odkaz
// TODO - premenovat niektore premenne a niektore premenne v niektorych triedach
// TODO - upravit Makefile pre testy

int main()
{
	map<BTSData::BTSKey, BTSData::Ptr> m_BTSData;
	vector<BTSData::Ptr> m_nearBTS;
	vector<Distance::BTSDistance> m_distance;

	try {
		if (!init("./csv/03-bts.csv", m_BTSData) || !initNearBTS("./csv/in1.csv", m_BTSData, m_nearBTS))
			return EXIT_FAILURE;

		countDistance(m_nearBTS, m_distance);

		Distance location;

		Coordinates::Ptr GPS = location.findMS(m_distance);
		cout << GPS->GoogleMapLink() << endl;

	}
	catch (const CustomException &ex) {
		cerr << ex.message() << endl;
		return EXIT_FAILURE;
	}
	catch (const exception &ex) {
		cerr << "unknown error" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
