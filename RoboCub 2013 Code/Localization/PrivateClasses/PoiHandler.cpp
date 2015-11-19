/*
 * PoiHandler.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: Elad
 */

#include "PoiHandler.h"
#include "Algorithm.h"
#include "Localization.h"

PoiHandler::PoiHandler(Localization* loc): m_hasMoved(false), m_loc(loc)
{
	m_poiDataList.clear();
	for(int i=0;i<NUMBER_OF_POIS;i++)
		m_samePoiCounter[i] = 0;
}

//Translates a received list of poi's from vision into locali's private member list
void PoiHandler::SetPois(list<PoiData>& newPoiList)
{
	//Our list of poi's is empty, simply fill it up
	if(m_poiDataList.empty()) {
		m_poiDataList = newPoiList;
	}
	//List is not empty and we've received new information without moving
	else if(!m_hasMoved)
	{
		int runs = newPoiList.size();
		for(int i=0;i<runs;i++)
		{
			PoiData temp = newPoiList.front();
			newPoiList.pop_front();
			//Compare current data with all data in list. If a match is found..
			if(IsInList(temp.id))
				//Check to see how many times we've received the same data id without moving
				// if we've received it too many times, discard the new data
				if(++m_samePoiCounter[temp.id]>=4)
					continue;
			//If a match is not found, add the new data to the back of the list
			m_poiDataList.push_back(temp);
		}
	}
	//List is not empty and we've received new information after motion
	else
	{
		//
		int runs = newPoiList.size();
		for(int i=0; i<runs; i++)
		{
			m_poiDataList.push_back(newPoiList.front());
			newPoiList.pop_front();
		}
		//Reset all counters to 0
		for(int i=0;i<NUMBER_OF_POIS;i++)
			m_samePoiCounter[i] = 0;
	}
#ifdef LOCALIZATION_DEBUG
	cout << "[PoiHandler::SetPois] number of pois set : " << m_poiDataList.size() << endl;
#endif

}

bool PoiHandler::IsInList(PoiId id)
{
	list<PoiData>::const_iterator iterator;
	for (iterator = m_poiDataList.begin(); iterator != m_poiDataList.end(); ++iterator) {
	    if(iterator->id == id)
	    	return true;
	}

	return false;
}

void PoiHandler::SendDataToAlgo()
{
	m_loc->GetAlgo()->ParticleFilter(m_poiDataList);
}
