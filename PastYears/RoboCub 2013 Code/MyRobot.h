/*
 * Robot.h
 *
 *  Created on: 24 Sep 2011
 *      Author: miller
 *	 Revised: by Guy, 6th October 2011
 */

#ifndef MYROBOT_H_
#define MYROBOT_H_

#include "General/MyThreadClass.h"
#include "Striker.h"
#include "Goalie/Goalie.h"
#include "AI/Goalkeeper.h"
#include "DebugMode.h"
#include "Vision.h"
#include "Communication/Communication.h"
#include "Localization/Localization.h"
#include "CM730.h"
#include "LinuxCM730.h"
#include "Enums/PlayerMode.h"

using namespace Robot;

class MyRobot
{

private:

	static PlayerMode_e 		m_playerMode;
	static RobotID_e 			m_robotID;

	static Striker*				m_striker;
	static Goalkeeper* 			m_goalie;
	static DebugMode* 			m_debugMode;
	static Localization* 		m_localization;
	static Communication* 		m_communication;
	static Vision* 				m_vision;

	static int			m_visionFlag;

public:


	MyRobot(PlayerMode_e playerMode);
	~MyRobot();

	static PlayerMode_e GetPlayerMode();
	static RobotID_e GetPlayerID();
	static void SetPlayerID(RobotID_e robotID);

	void Process();

	static void SetVisionFlag(int visionFlag);

	static void SigIntHandler(int sig);

};

#endif /* MYROBOT_H_ */
