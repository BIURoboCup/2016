/*
 * StepHandler.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#include "StepHandler.h"

StepHandler::StepHandler (Localization* locPtr) :
m_loc(locPtr),
m_currTimePtr(m_loc->getCurrTimeByRef()),
m_prevTimePtr(m_loc->getPrevTimeByRef()),
m_usPassedPtr(m_loc->getUsPassedByRef()),
m_motion(WalkingAndActions::GetInstance()),
m_toCmY(0.0000008),
m_toCmX(0.000000244),
m_toCmBackY(0.00000055),
m_toDeg(0.000000676439763){}

Diff StepHandler::getDiffFromStep(){
	Diff addedFromStep;
	//TODO correct the inconsistency between us and motion
	//the X & Y axis are different between motion and our axis!!!
	//their right is -y, our right is +x
	//their forward is x, out forward is y
	//their angel to the right is negative, our angle to the right is positive
//	cout<<m_motion->GetMotionValue(Y_SPEED)<<'\t'
//		<<m_motion->GetMotionValue(X_SPEED)<<'\t'
//		<<m_motion->GetMotionValue(ROTATION_ANGLE)<<'\t'
//			<<endl;
	if (MotionStatus::FALLEN == STANDUP)
	{
		if (m_motion->GetWalkingMode()==RUNNING)
		{

			if(abs(m_motion->GetMotionValue(Y_SPEED))<50) //legal value
				addedFromStep.x =-m_motion->GetMotionValue(Y_SPEED)*(*m_usPassedPtr)*m_toCmX;

			if(abs(m_motion->GetMotionValue(X_SPEED))<50) //legal value
			{
				if (m_motion->GetMotionValue(X_SPEED)>0) //moving forward
				{
					addedFromStep.y = m_motion->GetMotionValue(X_SPEED)*(*m_usPassedPtr)*m_toCmY;
				}
				else //moving backwards
				{
					addedFromStep.y = m_motion->GetMotionValue(X_SPEED)*(*m_usPassedPtr)*m_toCmBackY;
				}
			}

			if (abs(m_motion->GetMotionValue(ROTATION_ANGLE))<50) //legal value
					addedFromStep.angle =	-m_motion->GetMotionValue(ROTATION_ANGLE)*(*m_usPassedPtr)*m_toDeg;
		} else {//rain dance mode !
			addedFromStep.x = -m_motion->GetMotionValue(Y_SPEED)*(*m_usPassedPtr)*0.0000002;
			addedFromStep.y = m_motion->GetMotionValue(X_SPEED)*(*m_usPassedPtr)*0.0000006;
			addedFromStep.angle = -m_motion->GetMotionValue(ROTATION_ANGLE)*(*m_usPassedPtr)*0.0000007;
		}
#ifdef LOCALIZATION_DEBUG
		printf("StepHandler::getDiffFromStep():: addedFromStep.angle %f\n",(double)addedFromStep.angle);
#endif
	}
	addedFromStep.time =	*m_currTimePtr;
	return addedFromStep;
}
