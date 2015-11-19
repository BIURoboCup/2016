/*
 * BallHandler.h
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#ifndef BALLHANDLER_H_
#define BALLHANDLER_H_

#include "LocalizationTypedefs.h"
#include "CM730.h"
#include "Head.h"
#include "AI/ListCombinedData.h"
#include "Enums/PlayerMode.h"

class Localization;

//TODO why we need this?
#define CAM_HEIGHT_WALK (35.0)
#define CAM_HEIGHT_WALK_BACK (35.0)
#define CAM_HEIGHT_STAND (35.0)

class BallHandler {
public:
	BallHandler(Localization* loc);
	void 			setBall(Ball& ball);
	void			setBall(bool isBallInSight);
	BallLocation* 	getBallLocationByRef() { return &m_ballLocation;}
	void 			calcBallData();
	PlayerMode_t	static m_playerMode;

	//TODO I think this one isn't needed!
	ListCombinedData* getBallList() {return &m_ballBuffer;}

	void 			static SetPlayerMode(PlayerMode_t playerMode){ m_playerMode = playerMode; };

private:
	BallLocation 	m_ballLocation;
	Ball			m_ballFromVision;
	bool			m_isBallDetected;
	bool 			m_newBallPic;
	myTimeval* 		m_currTimePtr;
	myTimeval* 		m_prevTimePtr;
	us*				m_usPassedPtr;
	Location*		m_robotLocationPtr;

	ListCombinedData m_ballBuffer;//TODO replace with STL list<>
};

#endif /* BALLHANDLER_H_ */
