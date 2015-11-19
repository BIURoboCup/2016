/*
 * JyroHandler.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#include "GyroHandler.h"
#include "../Localization.h"
GyroHandler::GyroHandler(Localization* locPtr):
m_cm730(WalkingAndActions::GetInstance()->GetCM730()),
m_loc(locPtr),
m_currTimePtr(m_loc->getCurrTimeByRef()),
m_prevTimePtr(m_loc->getPrevTimeByRef()),
m_usPassedPtr(m_loc->getUsPassedByRef()) {}

Angle GyroHandler::CheckGyro() {
	int gyro_z = 0;
	float ad;
	//m_cm730->ReadWord(CM730::P_GYRO_Z_L, &gyro_z, 0);
	gyro_z-=512;

	ad=(*m_usPassedPtr)*gyro_z;
	ad*=GYRO2ANGLE_DIFF;
#ifdef LOCALIZATION_DEBUG
	printf("GyroHandler::CheckGyro()\tangleDiff=%f\n",ad);
#endif
	return ad;
}
