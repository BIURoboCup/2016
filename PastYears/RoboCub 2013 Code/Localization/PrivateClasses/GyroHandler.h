/*
 * JyroHandler.h
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#ifndef JYROHANDLER_H_
#define JYROHANDLER_H_

#include "LocalizationTypedefs.h"
#include "CM730.h"
class Localization;

#define GYRO2ANGLE 1.35*1600/512.0 //1.35 is what we calculated to avoid noise
#define GYRO2ANGLE_DIFF -(USEC2SEC)*(GYRO2ANGLE);

class GyroHandler {
public:
	GyroHandler(Localization* locPtr);

	Angle 	CheckGyro();

private:
	CM730* 			m_cm730;
	Localization*	m_loc;
	myTimeval* 		m_currTimePtr;
	myTimeval* 		m_prevTimePtr;
	us*				m_usPassedPtr;
};

#endif /* JYROHANDLER_H_ */
