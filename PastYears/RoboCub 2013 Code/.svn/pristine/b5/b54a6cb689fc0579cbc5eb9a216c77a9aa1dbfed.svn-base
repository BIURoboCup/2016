/*
 * ListCombinedData.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: guy
 */

#include "ListCombinedData.h"

ListCombinedData::ListCombinedData() {

	m_head = NULL;
	m_tail = NULL;
	m_numberOfCombinedDatas = 0;

}

ListCombinedData::~ListCombinedData() {
	RemoveAllCombinedDatas();
}

void ListCombinedData::AddCombinedData(BallLocation newBallLocation,
		Location myLocation) {
	CombinedData_t* newCombinedData = new CombinedData_t();
	newCombinedData->ballLocation = newBallLocation;
	newCombinedData->myLocation = myLocation;

	//put the new data first in list

	if (m_head == NULL) {	//if there is no data before
		newCombinedData->m_next = NULL;
		m_tail = newCombinedData;
	} else {
		m_head->m_prev = newCombinedData;
		newCombinedData->m_next = m_head;
	}

	m_head = newCombinedData;
	newCombinedData->m_prev = NULL;

	m_numberOfCombinedDatas++;
}

void ListCombinedData::RemoveCombinedData(
		CombinedData_t* CombinedDataToRemove) {
	CombinedData_t* nextPoleCandidate = CombinedDataToRemove->m_next;
	CombinedData_t* prevPoleCandidate = CombinedDataToRemove->m_prev;

	if (prevPoleCandidate == NULL) // pole candidate is head
			{
		m_head = nextPoleCandidate;
	} else {
		prevPoleCandidate->m_next = nextPoleCandidate;
	}
	if (nextPoleCandidate == NULL) // pole candidate is tail
			{
		m_tail = prevPoleCandidate;
	} else {
		nextPoleCandidate->m_prev = prevPoleCandidate;
	}

	CombinedDataToRemove->m_next = NULL;
	CombinedDataToRemove->m_prev = NULL;
	delete CombinedDataToRemove;
	m_numberOfCombinedDatas--;

}
void ListCombinedData::RemoveAllCombinedDatas() {
	CombinedData_t* ballToRemove;
	CombinedData_t* iterator = m_head;
	int numberOfPoleCandidatesToRemove = m_numberOfCombinedDatas;

	for (int i = 0; i < numberOfPoleCandidatesToRemove; i++) {
		ballToRemove = iterator;
		iterator = iterator->m_next;
		RemoveCombinedData(ballToRemove);
	}

}

int ListCombinedData::GetNumberOfCombinedDatas() {
	return m_numberOfCombinedDatas;
}
