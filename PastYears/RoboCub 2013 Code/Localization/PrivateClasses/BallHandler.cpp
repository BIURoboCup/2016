/*
 * BallHandler.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */
#include "../Localization.h"
#include "BallHandler.h"

PlayerMode_t	BallHandler::m_playerMode = DEBUG_MODE;

BallHandler::BallHandler(Localization* locPtr) :
m_currTimePtr(locPtr->getCurrTimeByRef()),
m_prevTimePtr(locPtr->getPrevTimeByRef()),
m_usPassedPtr(locPtr->getUsPassedByRef()),
m_robotLocationPtr(locPtr->GetMyLocationByRef()) {}

/*
 * when vision detects ball:
 */
void BallHandler::setBall( Ball& ball){
	setBall(true);
	ball.setAngles();//picture and head angles
	m_ballFromVision=ball;
}

/*
 * called by vision or locally!
 */
void BallHandler::setBall(bool isBallDetected){
	m_isBallDetected=isBallDetected;
	m_newBallPic=true;
}

void BallHandler::calcBallData() {
	m_ballLocation.isInSight=m_isBallDetected;
	if (m_newBallPic && m_isBallDetected) {

		m_ballLocation.time=m_ballFromVision.m_timeBallDetected;
		m_ballLocation.angleFromMe=m_ballFromVision.m_PicHorizonAngle+m_ballFromVision.m_headHorizonAngle;
		m_ballLocation.distanceFromMe=m_ballFromVision.CalculateDistanceFromFrameCenter_ByBallDiameter();

		m_ballLocation.x=m_ballLocation.distanceFromMe*sin(DEG2RAD*(m_ballLocation.angleFromMe+m_robotLocationPtr->angle));
		m_ballLocation.x+=m_robotLocationPtr->x;
		m_ballLocation.y=m_ballLocation.distanceFromMe*cos(DEG2RAD*(m_ballLocation.angleFromMe+m_robotLocationPtr->angle));
		m_ballLocation.y+=m_robotLocationPtr->y;

		if(m_playerMode == GOALIE_MODE)
		{
			m_ballBuffer.AddCombinedData(m_ballLocation, *m_robotLocationPtr);
		}
	}
	m_newBallPic=false;
}
