/*
 * LocalizationSim.cpp
 *
 *  Created on: Dec 13, 2012
 *      Author: robocup
 */

#include "LocalizationSim.h"

LocalizationSim::LocalizationSim() {
	// TODO Auto-generated constructor stub

}

LocalizationSim::~LocalizationSim() {
	// TODO Auto-generated destructor stub
}

bool LocalizationSim::TestBasicFuncSinglePoi() {
	m_loc = Localization::GetInstance();
	list<PoiData> pois;
	const int NUMBER_OF_PICS = 300;

	PoiData temp;

	for (int i = 0 ; i < NUMBER_OF_PICS ; i++)
	{
		pois.clear();

		temp.id = RIVAL_CENTER;
		temp.distFromMe = 120; //cm
		temp.angleFromMe = 0;
		temp.time.update();

		pois.push_back(temp);

		m_loc->SetPois(pois);

		m_loc->process();

		cout << "X: " << m_loc->GetMyLocation().x << " Y: " << m_loc->GetMyLocation().y << " Angle: " << m_loc->GetMyLocation().angle << endl;

#ifdef LOCALIZATION_DEBUG
		m_loc->setPoiMoved();
#endif
	}

	cout << "Expected final location: [0, 300, 0]" << endl;

	return true;
}

bool LocalizationSim::TestBasicFuncMultiplePoi() {
	m_loc = Localization::GetInstance();
	list<PoiData> pois;
	const int NUMBER_OF_PICS = 300;

	PoiData temp1, temp2;

	for (int i = 0 ; i < NUMBER_OF_PICS ; i++)
	{
		pois.clear();

		temp1.id = RIVAL_RIGHT_POLE;
		temp1.distFromMe = 197;
		temp1.angleFromMe = 24;
		temp1.time.update();

		pois.push_back(temp1);

		temp2.id = RIVAL_LEFT_POLE;
		temp2.distFromMe = 197;
		temp2.angleFromMe = -24;
		temp2.time.update();

		pois.push_back(temp2);

		m_loc->SetPois(pois);

		m_loc->process();

		cout << "X: " << m_loc->GetMyLocation().x << " Y: " << m_loc->GetMyLocation().y << " Angle: " << m_loc->GetMyLocation().angle << endl;
#ifdef LOCALIZATION_DEBUG
		m_loc->setPoiMoved();
#endif
	}

	cout << "Expected final location: [0, 420, 0]" << endl;

	return true;
}
