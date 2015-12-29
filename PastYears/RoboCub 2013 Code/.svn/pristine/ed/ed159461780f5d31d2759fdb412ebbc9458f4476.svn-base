/*
 * PoleCandidatesList.h
 *
 *  Created on: Jan 19, 2012
 *      Author: guy
 */

#ifndef POLECANDIDATESLIST_H_
#define POLECANDIDATESLIST_H_




#include "Pole.h"
#include "IncludesFile.h"

#include "opencv2/opencv.hpp"


class PoleCandidatesList {

private:

	friend class ObjectDetector;
	int		m_numberOfPoleCandidates;

	Pole* 	m_head;					// head of the Ball's list
	Pole* 	m_tail;					// tail of the Ball's list

public:

	PoleCandidatesList();
	~PoleCandidatesList();


	void AddPoleCandidate(Pole* newPoleCandidate);
	void RemovePoleCandidate(Pole* poleCandidateToRemove);
	void RemoveAllPoleCandidates();

	int   GetNumberOfPoleCandidates();


};

#endif /* POLECANDIDATESLIST_H_ */
