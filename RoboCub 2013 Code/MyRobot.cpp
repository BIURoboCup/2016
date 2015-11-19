/*
 * Robot.cpp
 *
 *  Created on: 24 Sep 2011
 *      Author: miller
 *      Revised: by Guy, 6th October 2011
 */

#include "MyRobot.h"

using namespace Robot;

int 			MyRobot::m_visionFlag;
RobotID_e 		MyRobot::m_robotID;
Striker* 		MyRobot::m_striker;
Goalkeeper* 	MyRobot::m_goalie;
DebugMode* 		MyRobot::m_debugMode;
Vision* 		MyRobot::m_vision;
Communication* 	MyRobot::m_communication;
PlayerMode_e	MyRobot::m_playerMode;
Localization* 	MyRobot::m_localization;

MyRobot::MyRobot(PlayerMode_e playerMode)
{
	m_playerMode = playerMode;

	m_vision = Vision::GetInstance();
	cout << "*************VisionFlag = "<< m_visionFlag << endl;

	m_vision->ProgramStartInitializations(m_visionFlag);
	m_localization = Localization::GetInstance();
	m_communication = Communication::GetInstance();
	m_communication->MyLocationData->playerMode = playerMode;
	GoTo::SetPlayerMode(playerMode);
	BallHandler::SetPlayerMode(playerMode);


	cout << "Done Calibrating Camera\n";
	sleep(1);

	switch(playerMode)
	{
	case DEBUG_MODE:
		m_debugMode = DebugMode::GetInstance();
		break;
	case STRIKER_MODE:
		m_striker = Striker::GetInstance();
		break;
	case GOALIE_MODE:
		m_goalie = Goalkeeper::GetInstance();
		break;
	}

}


MyRobot::~MyRobot() {}

void MyRobot::Process()
{
	cout<<"MyRobot::Process()"<<endl;
	
	//_localization->StartInternalThread();
	m_communication->StartInternalThread();
	m_vision->StartInternalThread();



	switch(m_playerMode)
	{
	case DEBUG_MODE:
		m_debugMode->StartInternalThread();
		m_debugMode->WaitForInternalThreadToExit();
		break;
	case STRIKER_MODE:
		m_striker->StartInternalThread();
		m_striker->WaitForInternalThreadToExit();
		break;
	case GOALIE_MODE:
		m_goalie->StartInternalThread();
		m_goalie->WaitForInternalThreadToExit();
		break;
	}


	//_localization->WaitForInternalThreadToExit();
	m_communication->WaitForInternalThreadToExit();
	m_vision->WaitForInternalThreadToExit();
}

PlayerMode_e MyRobot::GetPlayerMode()
{
	return m_playerMode;
}

void MyRobot::SetVisionFlag(int visionFlag)
{
	m_visionFlag = visionFlag;
}

RobotID_e MyRobot::GetPlayerID()
{
	return m_robotID;
}

void MyRobot::SetPlayerID(RobotID_e robotID)
{
	m_robotID = robotID;
}

/*****************************************************************************************
 * Method Name:   Sighandler
 * Description:   This is a static method that all not used signals can register to - for exiting the program.
 * Return Values: void*
 *****************************************************************************************/
void MyRobot::SigIntHandler(int sig)
{

	cout << "quitting...\n";
	exitProgram = true;

	m_communication->WaitForInternalThreadToExit();
	m_vision->WaitForInternalThreadToExit();

	switch(m_playerMode)
	{
		case DEBUG_MODE:
			m_debugMode->WaitForInternalThreadToExit();
			break;
		case STRIKER_MODE:
			m_striker->WaitForInternalThreadToExit();
			break;
		case GOALIE_MODE:
			m_goalie->WaitForInternalThreadToExit();
			break;
	}

	cout << "MyRobot: after waiting for vision, communication and AI. doing the stand up and sit down\n";
	sleep(2);
	Walking::GetInstance()->ForceNotRunning();
	WalkingAndActions::GetInstance()->StartAction(STAND_UP);
	sleep(0.5);
	WalkingAndActions::GetInstance()->StartAction(SIT_DOWN);
	sleep(1);
    exit(0);
}
