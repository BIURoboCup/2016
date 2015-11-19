/*
 * PoleCandidatesList.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: guy
 */

#include "PoleCandidatesList.h"
#include "Vision/header/ColorsValues.h"

#define ABS(x)	( (x) > 0 ? (x) : -(x) )

PoleCandidatesList::PoleCandidatesList()
{

	m_head = NULL;
	m_tail = NULL;
	m_numberOfPoleCandidates = 0;

}

PoleCandidatesList::~PoleCandidatesList()
{
	RemoveAllPoleCandidates();
}

void PoleCandidatesList::AddPoleCandidate(Pole* newPoleCandidate)
{
	if (m_head == NULL)
	{
		m_head = newPoleCandidate;
		newPoleCandidate->m_prev = NULL;
	}
	else
	{
		m_tail->m_next = newPoleCandidate;
		newPoleCandidate->m_prev = m_tail;
	}

	m_tail = newPoleCandidate;
	newPoleCandidate->m_next = NULL;

	m_numberOfPoleCandidates++;

}
void PoleCandidatesList::RemovePoleCandidate(Pole* poleCandidateToRemove)
{
	Pole* nextPoleCandidate = poleCandidateToRemove->m_next;
	Pole* prevPoleCandidate = poleCandidateToRemove->m_prev;

	if (prevPoleCandidate == NULL)	// pole candidate is head
	{
		m_head = nextPoleCandidate;
	}
	else
	{
		prevPoleCandidate->m_next = nextPoleCandidate;
	}
	if (nextPoleCandidate == NULL)	// pole candidate is tail
	{
		m_tail = prevPoleCandidate;
	}
	else
	{
		nextPoleCandidate->m_prev = prevPoleCandidate;
	}


	poleCandidateToRemove->m_next = NULL;
	poleCandidateToRemove->m_prev = NULL;
	delete poleCandidateToRemove;
	m_numberOfPoleCandidates--;

}
void PoleCandidatesList::RemoveAllPoleCandidates()
{
	Pole* poleToRemove;
	Pole* iterator = m_head;
	int numberOfPoleCandidatesToRemove = m_numberOfPoleCandidates;



	for (int i = 0; i < numberOfPoleCandidatesToRemove; i++)
	{
		poleToRemove = iterator;
		iterator = iterator->m_next;
		RemovePoleCandidate(poleToRemove);
	}

}

int  PoleCandidatesList::GetNumberOfPoleCandidates()
{
	return m_numberOfPoleCandidates;
}
