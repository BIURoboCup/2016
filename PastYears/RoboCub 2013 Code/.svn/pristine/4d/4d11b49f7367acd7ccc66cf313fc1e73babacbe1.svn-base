/*
 * ListCombinedData.h
 *
 */

#ifndef LIST_COMBINED_DATA_H_
#define LIST_COMBINED_DATA_H_

#include "IncludesFile.h"
#include "Structs/CombinedData.h"

class ListCombinedData {

private:
	friend class Goalkeeper;
	int m_numberOfCombinedDatas;

	CombinedData_t* m_head; // head of the Ball's list
	CombinedData_t* m_tail; // tail of the Ball's list

public:

	ListCombinedData();
	~ListCombinedData();

	void AddCombinedData(BallLocation newCombinedData, Location myLocation);
	void RemoveCombinedData(CombinedData_t* CombinedDataToRemove);
	void RemoveAllCombinedDatas();

	int GetNumberOfCombinedDatas();

};

#endif /* LIST_BALL_LOCATION_H_ */
