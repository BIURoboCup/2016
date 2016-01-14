/*
 * BrainAndMotion.h
 *
 *  Created on: Sep 27, 2015
 *      Author: root
 */

#ifndef SRC_BRAINANDMOTION_H_
#define SRC_BRAINANDMOTION_H_

//=========================/ INCLUDES /==========================//

#include <unistd.h>
#include <string.h>
#include <libgen.h>

#include "Camera.h"
#include "Point.h"
#include "mjpg_streamer.h"
#include "minIni.h"
#include "LinuxCamera.h"
#include "ColorFinder.h"
#include "Action.h"
#include "Head.h"
#include "Walking.h"
#include "MX28.h"
#include "MotionManager.h"
#include "LinuxMotionTimer.h"
#include "LinuxCM730.h"
#include "LinuxActionScript.h"
#include "JointData.h"
#include "Vision.h"


using namespace Robot;//added

//=========================/ DEFINES /===========================//

//#ifdef MX28_1024
//#define MOTION_FILE_PATH    "/home/robot/Robot1/Data/motion_1024.bin"
//#else
//#define MOTION_FILE_PATH    "/home/robot/Robot1/Data/motion_4096.bin"
//#endif

#ifdef MX28_1024
#define MOTION_FILE_PATH    "/home/robot/Desktop/Walk/Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "/home/robot/Desktop/Walk/Data/motion_4096.bin"
#endif

#define INI_FILE_PATH    "/home/robot/Robot1/Data/config.ini"

#define TURN_HARD_RIGHT_DIR -6
#define TURN_HARD_LEFT_DIR   6


//=========================/ STRUCTURES /=======================//
struct S_BrainBall
{
	int x_place[10];
	int x_detail;
	int x_ball_spot;
	float radius;
	bool found;
	Point2f center;

	S_BrainBall(): x_detail(10),x_ball_spot(5),radius(0),found(false),center(0,0)
    {}
};

enum E_State
{
	NOP				= 0,
	Init 	 		= 1,
	Stop  	 		= 2,
	GoToBall 		= 3,
	KickBall 		= 4,
	LookForBall		= 5,
	FindGate 		= 6,
	ChangeSpot		= 7,
	GoForKick		= 8,
	EXIT			= 9,
};


struct S_StateStruct
{
	E_State state;
	std::string stateString;
};


enum E_WalkState
{
	Forward	= 1,
	Stand  	= 2,
	Right  	= 3,
	Left    = 4,
};

enum E_HeadPosition
{
	LookingStraight		= 1,
	LookingRight 		= 2,
	LookingLeft  		= 3,
	LUnknown        	= 4,
	LookingStraightDown	= 5,
};
//===================/ FUNCTION DECLARATIONS /===================//
void 	play();

void 	GoTo(const double& dir,std::string state,int x_ball_spot);
void 	GoToUpdateDirection(const double& dir);
void    GoStop();

float 	GetTilt();
void 	SetTilt(int headPan_, int headTilt_ );
void 	Reset(Robot::CM730 *cm730, int id);
void 	printenable();


void	startLog(ofstream& logFile,std::string logFilePath);
void	writeToLog(ofstream& logFile, std::string text);
void    writeToLog(ofstream& logFile, std::string text1, std::string text2);
void    writeToLog(ofstream& logFile, std::string text1, std::string text2, double dir , int ballSpot);
void 	endLog(ofstream& logFile);



#endif /* SRC_BRAINANDMOTION_H_ */
