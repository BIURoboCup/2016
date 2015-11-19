/*
 * PoiHandler.h
 *
 *  Created on: Nov 14, 2012
 *      Author: Elad
 */

#ifndef POIHANDLER_H_
#define POIHANDLER_H_

#include "LocalizationTypedefs.h"
#include <list>

class Localization;
class Algorithm;

class PoiHandler {
public:
	PoiHandler(Localization* loc);
	~PoiHandler() {};

	//Called by vision
	//Strip down all irrelevant information from Poi object for the relevant struct
	void SetPois(list<PoiData>& poiList);

	//Called by Localization
	//Sends m_poiDataList to the Algorithm
	void SendDataToAlgo();

	// Returns true if there is new data in m_poiDataList
	bool IsNewData() { return m_poiDataList.size() > 0; };

	// See notes for m_hasMoved
	void SetHasMoved(bool hasMoved)	{ m_hasMoved = hasMoved; };

private:

	list<PoiData> m_poiDataList;

	//These are used for calibration purposes. Will need to be refined for Pois instead of Poles
	//double m_goleHeight2dist;
	//double m_goleWidth2dist;
	//double m_sidePoleHeight2dist;
	//double m_sidePoleWidth2dist;

	//True if the robot moved since we last checked, false otherwise
	bool m_hasMoved;

	//Needed for when re-sampling the same pole over and over again, may cause to much noise!
	unsigned m_samePoiCounter[NUMBER_OF_POIS];
	//PoiId m_lastPoiId;
	Localization* m_loc;

	bool IsInList(PoiId id);
};

#endif /* POIHANDLER_H_ */



