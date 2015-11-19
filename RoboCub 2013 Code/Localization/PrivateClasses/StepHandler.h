/*
 * StepHandler.h
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#ifndef STEPHANDLER_H_
#define STEPHANDLER_H_

#include "../Localization.h"

class StepHandler {
public:
	StepHandler(Localization*);
	Diff 		getDiffFromStep();

private:
	Localization*		m_loc;
	myTimeval* 			m_currTimePtr;
	myTimeval* 			m_prevTimePtr;
	us*					m_usPassedPtr;
	WalkingAndActions* 	m_motion;
	double				m_toCmY;
	double				m_toCmX;
	double				m_toCmBackY;
	double				m_toDeg;
};

#endif /* STEPHANDLER_H_ */
