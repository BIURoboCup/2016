#ifndef WALKING_MODULE_H
#define WALKING_MODULE_H

#include "LinuxDARwIn.h"
#include "errno.h"
#include <string.h>


//#define CONFIG_INI_FILE_PATH       "/darwin/Data/config.ini"
#define RUNNING_INI_FILE_PATH       "/darwin/Data/running_mode_config.ini"
#define WALKING_INI_FILE_PATH       "/darwin/Data/walking_mode_config.ini"
#define BACKWARD_INI_FILE_PATH      ""
#define MOTION_FILE_PATH   			"/darwin/Data/motion_4096.bin"
#define U2D_DEV_NAME0       		"/dev/ttyUSB0"
#define U2D_DEV_NAME1       		"/dev/ttyUSB1"
#define MAX_TURN_LEFT_SPEED				(25)
#define MAX_TURN_RIGHT_SPEED			(-MAX_TURN_LEFT_SPEED)
#define MAX_LEFT_SPEED					(30)
#define MAX_RIGHT_SPEED					(-MAX_LEFT_SPEED)
#define WALKING_MAX_FORWARD_SPEED		(16)
#define WALKING_MAX_BACKWARD_SPEED		(-17)
#define RUNNING_MAX_FORWARD_SPEED		(40)
#define RUNNING_MAX_BACKWARD_SPEED		(-13)
#define TIME_BETWEEN_SPEED_CHANGE	    (5) //ms
#define CHANGE_PEN_SPEED				(20)
#define HIP_INCREASE_SPEED				(23)
#define MAX_SPEED_TO_SLOW_FIRST         (5)

using namespace std;
using namespace Robot;

typedef enum MotionArgs_e
{
	X_SPEED = 1,
	Y_SPEED = 2,
	ROTATION_ANGLE = 3
}MotionArgs_t;

typedef enum HeadArgs_e
{
	X_ANGLE = 1,
	Y_ANGLE = 2,
}HeadArgs_t;

typedef enum WalkingModes_e
{
	WALKING,
	RUNNING
//	BACKWARDS
}WalkingModes_t;

typedef enum Actions_e
{
	STAND_UP_WALKING = 7,
	STAND_UP = 8,
	KICK_RIGHT = 75,
	KICK_LEFT = 76,
	SIT_DOWN = 15,
	PASS_RIGHT = 72,
	PASS_LEFT = 73,
	GET_UP_FORWARD = 10,
	GET_UP_BACKWARD = 11,
	FALSH_RIGHT = 82,
	FALSH_LEFT = 83,
	JUMP_RIGHT = 97,
	JUMP_LEFT = 98,
	JUMP_MIDDLE = 103,
	HAND_MOVE = 104,
	STEP_RIGHT = 108,
	STEP_LEFT = 107
}Actions_t;

/*typedef struct SpeedArgs_s
{
	double xSpeed;
	double ySpeed;
	double angle;
}SpeedArgs_t;
*/

class WalkingAndActions
{
private:
	WalkingAndActions();
	bool           				m_isInRunningMode;
	bool						m_isThreadRunning;
	MotionManager* 				m_motionManager;
	Walking*       				m_walking;
	Action*        				m_action;
	Head*						m_head;
	minIni*						m_running_ini;
	minIni*						m_walking_ini;
	static WalkingAndActions*	m_instance;
	LinuxCM730 					m_linux_cm730;//(U2D_DEV_NAME0);
	CM730 						m_cm730;//(&m_linux_cm730);
	double						m_maxXSpeed;
	double						m_minXSpeed;
	double						m_maxYSpeed;
	double						m_minYSpeed;
	double						m_maxLeftTurningAngle;
	double						m_maxRightTurningAngle;
	double						m_xSpeed;
	double						m_ySpeed;
	double						m_angle;
//	double						m_maxXHeadAngle;
//	double						m_minXHeadAngle;
//	double						m_maxYHeadAngle;
//	double						m_minYHeadAngle;
	WalkingModes_t				m_walkingMode;
//	sem_t 						m_action_sync;
	pthread_mutex_t				m_walkingAndActionsMutex;
	pthread_t 					m_myThread;
	bool IsWalking(); //v
	bool IsActionOnGoing(); //v
	double 						m_runningHipPitchOffsetForward;
	double 						m_runningHipPitchOffsetBackward;


public:
	~WalkingAndActions();
	void SetWalkingMode(WalkingModes_t walkingMode);
	void StopWalking();
	bool StartWalking();
	void ResetWalkingParams();
	static void* StartWalking(void *ptr);
	bool StartWalking(double xSpeed, double ySpeed, double angle);
	void StartWalkingAsync(double xSpeed, double ySpeed, double angle);
	double  ChangeMotionValue(MotionArgs_t motionArg,double increaseDecreaseSpeedBy);
	WalkingModes_t GetWalkingMode();
	string GetWalkingModeByString();
	string ConvertWalkingModeToString(WalkingModes_t walkingMode);
	static WalkingAndActions *GetInstance();
	double GetMotionValue(MotionArgs_t motionArg);
	void StartAction(Actions_t action);
	CM730* GetCM730();
	static void* CheckStatus(void *ptr);
	static void FallenHandler(int sig);
	void MoveHead(double xHeadAngle,double yHeadAngle);
	double  ChangeHeadAngleValue(HeadArgs_t headArg,double increaseDecreaseAngleBy);
	double GetHeadAngleValue(HeadArgs_t headArg);
	void ResetHeadParams();
	void StopThreadRunning();
	bool IsThreadRunning();
	static bool m_isLeftButtonPressedLocalization;
	static bool m_isMiddleButtonPressedLocalization;
	static bool m_isLeftButtonPressedAI;
	static bool m_isMiddleButtonPressedAI;
};



#endif
