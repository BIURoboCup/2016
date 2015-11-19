/*
 * GoTo.h
 *
 *  Created on: Mar 11, 2012
 *      Author: isri
 */
#ifndef GOTO_H
#define GOTO_H
#include "Structs/XYLocation.h"
#include "Localization.h"
#include "IncludesFile.h"
#include "WalkingAndActions.h"
#include "Enums/PlayerMode.h"


#define FIELD_HIGHT                     (600)
#define EPSILON 	                    (0.00001)
#define REASONABLE_NOISE_DISTANCE       	(40)  //cm
#define KICKABLE_DISTANCE               	(10)   //cm
#define START_TURNING_DISTANCE          	(20)  //cm
#define STOP_TURNING_ANGLE              	(15)  //deg
#define MINIMUM_ANGLE_TO_KICK				(7)	  //deg
#define GO_N_FROM_BALL						(50)  //cm
#define Y_LOWER_BOUND						(-10) //cm
#define RAIN_DANCE_DISTANCE_CM				(20)  //cm
#define FAR_DISTANCE				    	(50)  //cm
#define SLOWING_DOWN_DISTANCE				(30)  //cm
#define FAR_DISTANCE_CM 					(50)  //cm
//#define KICKABLE_DISTANCE_CM 				()  //cm
#define CLOSE_DISTANCE_CM	 				(25)  //cm
#define ZERO_ANGLE_RIGHT_FAR 				(10)	  //deg
#define ZERO_ANGLE_LEFT_FAR 				(-ZERO_ANGLE_RIGHT_FAR)
#define ZERO_ANGLE_RIGHT_CLOSE 				(23)  //deg
#define ZERO_ANGLE_LEFT_CLOSE 				(-ZERO_ANGLE_RIGHT_CLOSE)
#define DISTANCE_TO_START_DETECTING_BALL	(30)  //cm
#define OFFSET_FROM_POLE					(50)
#define OFFSET_IN_LOWER_FIELD               (150)
#define Y_POLE								(600)
#define MAX_ANGLE_FROM_BALL					(70)
#define GOALIE_ANGLE_FOR_RAIN_DANCE			(80)
#define DEFENCE_AREA_Y_CM					(240)
#define RAIN_DANCE_ANGLE_CORRECTION			(35)
#define RAIN_DANCE_ANGLE_CORRECTION_BACK	(45)
#define OFFENSIVE_POSITION_Y_CM				(520)
#define POLE_X_CM							(70)
#define ANGLE_MISTAKE						(Angle)(8.0)
#define REASONABLE_NOISE_TIMES				(2)
#define DISTANCE_OUT_OF_RAIN_DANCE          (30)
#define MIN_X_DIFF_FROM_N_CM_POINT			(30)

#define GET_OFFSET(yVariable)          ( ( (yVariable) > DEFENCE_AREA_Y_CM ) ?  OFFSET_FROM_POLE : OFFSET_IN_LOWER_FIELD )

extern bool IsBallDataRelevant(BallLocation *ballLocation);
extern bool IsBallDataFromNow(BallLocation *ballLocation);

typedef enum GoToType_e
{
	XY   = 0,
	BALL = 1,
	UNDEFTYPE = 3
}GoToType_t;

typedef enum StopStartThread_e
{
	STOP   		  = 0,
	START  		  = 1
}StopStartThread_t;

typedef enum ThreadStat_e
{
	IDLE   		  = 0,
	STARTED		  = 1
}ThreadStat_t;

typedef enum GoToBallStates_e
{

	STATE_GO_DIRECTLY_TO_BALL = 0,
	STATE_DECISION_MAKING     ,
	STATE_RAIN_DANCE          ,
	STATE_GO_N_CM_FROM_BALL   ,
//	STATE_GO_TO_LINE_L        = 4,
//	STATE_GO_TO_LINE_G        = 5,
//	STATE_ON_LINE_TO_BALL     = 6,
//	STATE_ON_GOAL_TO_BALL     = 7,
	STATE_GO_KICK			  ,
	STATE_GOTO_UNINITIALIZED
}GoToBallStates_t;

typedef enum QuickAndDirtyState
{
	FIRST_TIME,
	FAR_AND_TURN,
	FAR_NO_TURN,
	CLOSE,
	OUT_OF_FIELD
} QuickAndDirtyState;

class GoTo : public MyThreadClass
{

private:
	Localization*			m_Localization;		//pointer to the localization object
	static GoTo*			m_Instance;			//Instance to the GoTo class
	GoToType_t 				m_GoToType;			//The type of go to
	XYLocation_t 			m_XY;				//XY point
	Angle					m_angleToReach;		//Angle to reach
	StopStartThread_t		m_ThreadStartStop;  //holds the class thread stop start
	WalkingAndActions*		m_Motion;			//holds pointer to motion
	bool					m_IsReached;        //holds whether we arrived to the ball or not
	GoToBallStates_t        m_PrevState;		//holds the prev state of go to ball
	GoToBallStates_t        m_CurrentState;		//holds the current state of go to ball
	ThreadStat_t			m_ThreadStat;		//holds the thread status
	bool					m_isKeepMeAlive;	//Indicates whether the thread can be killed or not.
	static PlayerMode_t		m_PlayerMode;
	Location*				m_myLocPtr;


	GoTo();
	void GoToBall();
	string PrintStateGoTo(GoToBallStates_e state);
	void GoToXY();
	BallLocation GetBallLocation();
	virtual void InternalThreadFunc();
	void CalculateABParametersOFOppGoalBallEquation(double *aParam, double *bParam, double x, double y);
	void CalculateABParametersOFOppGoalBallEquationPlumb(double *aParam, double *bParam, double x, double y);
	void CalculateABParametersOFBallOurGoalEquation(double *aParam, double *bParam, double x, double y);
	double CalculateYValue(double aParam, double bParam, double x);
	double CalculateDistanceBetweenTwoPoints(double x1, double y1, double x2, double y2);
	double CalculateDistanceBetweenPointAndLine(double aParam, double bParam, double x, double y);
	double CalculateAngleFromXY(double x1, double y1, double x2, double y2, double myAngle);
	double ATanDeg(double value);
	double GetXSpeed(double distance, double angle, bool finalXY);
	double GetTurnSpeed(double angle, double distance, bool smooth);
	void CalculateXYProjectionOnALine(double aParam, double bParam, double x, double y, double *xProj, double *yProj);
	void QuadraticEquation(double aParam, double bParam, double cParam, double *x1, double *x2);
	void CalculateNPoint(double xBall, double yBall, double radius, double *xPoint, double *yPoint);
	bool IsPossibleToKick(Location myLocation);

	bool m_isNeedToTurn;
	bool m_separateAxis;

	//GoToBall state machine
	void StateDecisionMaking();
	//void StateGoToLineG();
	//void StateGoToLineL();
	void StateGoDirectlyToBall(GoToBallStates_t nextState);
	void StateGoKick();
	void StateRainDance();
	void StateGoNcmFromBall();




public:

	static void SetPlayerMode(PlayerMode_t playerMD);
	static GoTo *GetInstance();
	void StartGoToBall();
	void StartGoToXY(XYLocation_t xy,double angle = (double)(-1000));
	ThreadStat_t GetThreadStatus();
	void ResetIsReached();
	bool IsReached();
	void Stop();
//	void Check();
	bool IsKeepMeAlive();
	bool QuickAndDirty(int x,int y,int a=-1000,bool dontGoInCircles=false);
};

#endif
