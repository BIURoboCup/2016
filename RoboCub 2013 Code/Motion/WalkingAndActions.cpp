/*
 * WalkingAndActions.cpp
 *
 *  Created on: Dec 22, 2011
 *      Author: isri
 */
#include "WalkingAndActions.h"
#include "Localization.h"
#include "MyRobot.h"

//extern bool IsBallDataRelevant(BallLocation *ballLocation);

#define HIP_PITCH_OFFSET_ON

//#define DEBUG_PRINT_MY_LOCATION
//#define DEBUG_PRINT_BALL_LOCATION
//#define DEBUG_WALKING_AND_ACTIONS_ON
//#define DEBUG_WALKING_AND_ACTIONS_EXTENDED_ON
//#define DEBUG_WALKING_AND_ACTIONS_WALKING_MODE_ON
//#define DEBUG_WALKING_AND_ACTIONS_KICK_ON
//#define DEBUG_WALKING_AND_ACTIONS_SIGNAL_ON
//#define DEBUG_FALLEN_HANDLER
//#define DEBUG_CHECK_STATUS
//#define DEBUG_STOP_WALKING
//#define DEBUG_SET_WALKING_MODE
//#define DEBUG_START_ACTION
//#define DEBUG_WALKING_AND_ACTIONS_CHANGE_OFFSET


bool WalkingAndActions::m_isLeftButtonPressedLocalization = false;
bool WalkingAndActions::m_isMiddleButtonPressedLocalization = false;
bool WalkingAndActions::m_isLeftButtonPressedAI = false;
bool WalkingAndActions::m_isMiddleButtonPressedAI = false;

WalkingAndActions::~WalkingAndActions() {}

//Singleton instance
WalkingAndActions* WalkingAndActions::m_instance = 0;

/*****************************************************************************************
 * Method Name:   GetInstance
 * Description:   This method is a static function that returns the instance of the class
 * 				  that there will be only one instance of that class type (SINGLETON)
 * Return Values: Return WalkingAndActions* -  the instance of the class
 *****************************************************************************************/
WalkingAndActions* WalkingAndActions::GetInstance()
{
	if (m_instance == 0)
	{
		m_instance = new WalkingAndActions();
	}
	return m_instance;
}

/*****************************************************************************************
 * Function Name: (constructor)
 * Description: This constructor initialize the CM730 and MX28 with 4096 fw, adds the action
 * 				adds the action, walking modules to the motion manager, loading walking INI
 * 				file and enable the motion manage
 *****************************************************************************************/
WalkingAndActions::WalkingAndActions() : m_linux_cm730(U2D_DEV_NAME0), m_cm730(&m_linux_cm730)
{
	cout << "Walking and Actions c'tor\n";

	try
	{
		m_running_ini 	= new minIni(RUNNING_INI_FILE_PATH);
		m_walking_ini 	= new minIni(WALKING_INI_FILE_PATH);
	}
	catch (const char* str)
	{
		cout << "Walking and Actions error: " << str << "\n";
	}
	catch (...)
	{
		cout << "One of the motion configuration files is missing\n";
	}

	m_motionManager = MotionManager::GetInstance();
	m_walking 		= Walking::GetInstance();
	m_action 		= Action::GetInstance();
	m_head			= Head::GetInstance();
	//////////////////// Framework Initialize ////////////////////////////

	if (m_motionManager->Initialize(&m_cm730) == false)
	{
		m_linux_cm730.SetPortName(U2D_DEV_NAME1);
		if (m_motionManager->Initialize(&m_cm730) == false)
		{
			printf("Fail to initialize Motion Manager!\n");
			exit(0);
		}
	}
	m_motionManager->AddModule((MotionModule*) Action::GetInstance());
	m_motionManager->AddModule((MotionModule*) Walking::GetInstance());
	m_motionManager->AddModule((MotionModule*) Head::GetInstance());
	LinuxMotionTimer::Initialize(m_motionManager);
	/////////////////////////////////////////////////////////////////////

	///////////////////////// MX 28 Initialization //////////////////////
	Walking::GetInstance()->BALANCE_ENABLE = true;
	int firm_ver = 0;
	if (m_cm730.ReadByte(JointData::ID_HEAD_PAN, MX28::P_VERSION, &firm_ver, 0) != CM730::SUCCESS)
	{
			fprintf(stderr,	"Can't read firmware version from Dynamixel ID %d!! \n\n", JointData::ID_HEAD_PAN);
			exit(0);
	}

	if (0 < firm_ver && firm_ver < 27)
	{
		fprintf(stderr, "MX-28's firmware is not support 4096 resolution!! \n");
		fprintf(stderr, "Upgrade MX-28's firmware to version 27(0x1B) or higher.\n\n");
		exit(0);
	}

	else if (27 <= firm_ver)
	{
		Action::GetInstance()->LoadFile((char*) MOTION_FILE_PATH);
	}

	else
	{
		exit(0);

	}
	//////////////////////////////////////////////////////////////////////////////////////
 	m_walking->m_Joint.SetEnableBody(false, true);
    m_head->m_Joint.SetEnableBody(false, true);
    m_action->m_Joint.SetEnableBody(false, true);
    SetWalkingMode(RUNNING);
    m_runningHipPitchOffsetForward = m_walking->HIP_PITCH_OFFSET;
    m_runningHipPitchOffsetBackward = m_runningHipPitchOffsetForward - 8;
    m_motionManager->SetEnable(true);
    m_myThread = 0;

//	signal(SIGUSR2, WalkingAndActions::FallenHandler);
	pthread_t threadCheckStatus;
 	int isThreadCheckStatus;
 	MotionStatus::FALLEN = STANDUP;
 	isThreadCheckStatus = pthread_create( &threadCheckStatus, NULL, WalkingAndActions::CheckStatus, (void*)pthread_self());


}

/*****************************************************************************************
 * Function Name: SetWalkingMode
 * Description: This function Loads the settings for the walking module to a walking mode
 * 				the function sets the max walking speed to 13 and -13.
 * 				the function stops the motion, and needs to be enabled.
 *
 *****************************************************************************************/
void WalkingAndActions::SetWalkingMode(WalkingModes_t walkingMode)
{
	if(m_walkingMode == walkingMode)
	{
		return;
	}
	StopWalking();
	#ifdef DEBUG_SET_WALKING_MODE
		cout << "[WalkingAndActions::SetWalkingMode]: Toggling walking mode to [" << ConvertWalkingModeToString(walkingMode) << "]\n";
	#endif
	usleep(500 * 1000);
	switch(walkingMode)
	{
	//if changed to walking mode, change the max speed for the axis
	//and load ini file, and change the walking mode variable

	case RUNNING:
		m_walking->LoadINISettings(m_running_ini);
		m_maxXSpeed      = RUNNING_MAX_FORWARD_SPEED;
		m_minXSpeed      = RUNNING_MAX_BACKWARD_SPEED;
		m_maxYSpeed		= MAX_LEFT_SPEED;
		m_minYSpeed		= MAX_RIGHT_SPEED;
		m_maxLeftTurningAngle  = MAX_TURN_LEFT_SPEED;
		m_maxRightTurningAngle = MAX_TURN_RIGHT_SPEED;
		m_walkingMode		   = RUNNING;
	break;

	case WALKING:
		StartAction(STAND_UP_WALKING);
		m_walking->LoadINISettings(m_walking_ini);
		m_maxXSpeed      = WALKING_MAX_FORWARD_SPEED;
		m_minXSpeed      = WALKING_MAX_BACKWARD_SPEED;
		m_maxYSpeed		= MAX_LEFT_SPEED;
		m_minYSpeed		= MAX_RIGHT_SPEED;
		m_maxLeftTurningAngle  = MAX_TURN_LEFT_SPEED;
		m_maxRightTurningAngle = MAX_TURN_RIGHT_SPEED;
		m_walkingMode		   = WALKING;
	break;

	default:
		printf("No such walkingMode type as [%d]\n",walkingMode);
	break;
	}
}

/*****************************************************************************************
 * Function Name: GetWalkingMode
 * Description:   This function return the walking mode loaded
 * Return Value:  Return the walking mode
 *****************************************************************************************/
WalkingModes_t WalkingAndActions::GetWalkingMode()
{
	return m_walkingMode;
}

/*****************************************************************************************
 * Function Name: GetWalkingModeByString
 * Description:   This function return the walking mode loaded as a string
 * Return Value:  Return the walking mode as a string
 *****************************************************************************************/
string WalkingAndActions::GetWalkingModeByString()
{
	return ConvertWalkingModeToString(m_walkingMode);
}

string WalkingAndActions::ConvertWalkingModeToString(WalkingModes_t walkingMode)
{
	switch(walkingMode)
	{
	case WALKING:
		return "walking";
		break;
	case RUNNING:
		return "running";
		break;
	default:
		return "No such WalkingMode";
		break;
	}
	return "No such WalkingMode";
}


/*****************************************************************************************
 * Function Name: StopWalking
 * Description: This function makes the walking stops. if the x axis speed is over 20
 * 				than decrease the speed to 20 and then stop  the walking
 *****************************************************************************************/
void WalkingAndActions::StopWalking()
{
	//int counter=0;
	//for(int i = 0; i < 1; i++)
	//{
	#ifdef DEBUG_STOP_WALKING
		cout << "WalkingAndActions::StopWalking()\n";
	#endif
	//check if the robot is making any action, if so throw exception
	if( IsActionOnGoing() == true )
	{
		#ifdef DEBUG_STOP_WALKING
			cout << "[StopWalking]: Action is on going...\n";
		#endif
		return;
		//throw "robot is in action mode";
	}
	#ifdef DEBUG_STOP_WALKING
		cout << "[StopWalking]: before StopThreadRunning\n";
	#endif

	StopThreadRunning();

	#ifdef DEBUG_STOP_WALKING
		cout << "[StopWalking]: after StopThreadRunning, IsWalking == "<< IsWalking() << "\n";
	#endif

	//no need to stop walking if the robot is not walking
	if( IsWalking() == true )
	{
		#ifdef DEBUG_STOP_WALKING
			cout << "[StopWalking]: Walking is still on...\n";
		#endif
		//if robot is in running mode and x_axis speed is over 10, the robot should decrease his speed
		//before stopping immediately so it won't fall
		while( m_walking->X_MOVE_AMPLITUDE > 10 )
		{
			m_walking->X_MOVE_AMPLITUDE -= 10;
			//sleep for 100 ms
			usleep(100*1000);
		}

		#ifdef DEBUG_STOP_WALKING
			cout << "[StopWalking]: Walking is being stopped...\n";
		#endif
		m_walking->Stop();

		//wait until the motion is stopped
		while( IsWalking() == true )
		{
			usleep(8*1000);
		}
		#ifdef DEBUG_STOP_WALKING
			cout << "[StopWalking]: Walking has been stopped.\n";
		#endif
	//}
	}
}


/*****************************************************************************************
 * Function Name: IsWalking
 * Description: This function returns the walking status of the robot
 * Return Value: Return the status of walking
 *****************************************************************************************/
bool WalkingAndActions::IsWalking()
{
	return m_walking->IsRunning();
}


/*****************************************************************************************
 * Function Name: IsActionOnGoing
 * Description:   This function returns the action status of the robot
 * Return Value:  Return the status of action
 *****************************************************************************************/
bool WalkingAndActions::IsActionOnGoing()
{
	return m_action->IsRunning();
}


/*****************************************************************************************
 * Function Name: ResetWalkingSpeeds
 * Description:   This function resets the walking params
 * Return Value:  void
 *****************************************************************************************/
void WalkingAndActions::ResetWalkingParams()
{
	//initialize the speeds to 0
	m_walking->X_MOVE_AMPLITUDE = 0;
	m_walking->A_MOVE_AMPLITUDE = 0;
	m_walking->Y_MOVE_AMPLITUDE = 0;
}


/*****************************************************************************************
 * Function Name: StartWalking
 * Description:   This function starts the walking motion. if walking already started,
 * 				  return false. the function zeros the speeds.
 * Return Values: Return false when walking already started, and True on success
 *****************************************************************************************/
bool WalkingAndActions::StartWalking()
{
	return StartWalking(0,0,0);
}


void* WalkingAndActions::StartWalking(void *This)
{
	WalkingAndActions *walkingAndActions = ((WalkingAndActions*)(This));
	walkingAndActions->StartWalking(walkingAndActions->m_xSpeed, walkingAndActions->m_ySpeed, walkingAndActions->m_angle);
	return NULL;
}


/*****************************************************************************************
 * Function Name: StartWalking
 * Description:   This function starts the walking motion with speed values. if walking already started,
 * 				  return false. if the arguments of speed values exceeds the max and min values
 * 				  throw an exception. the function zeros the speeds.
 * 				  The method change speed as it increases or decreases in steps
 * Arguments:	  xSpeed - the x axis speed
 * 				  ySpeed - the y axis speed
 * 				  angle  - strength of angle turn
 * Return Values: Return false when walking already started, and True on success
 *****************************************************************************************/
bool WalkingAndActions::StartWalking(double xSpeed, double ySpeed, double angle)
{
	bool isHipPitchOffsetSet = false;
	double oldXSpeed = GetMotionValue(X_SPEED);
	#ifdef DEBUG_WALKING_AND_ACTIONS_EXTENDED_ON
       printf("[StartWalking]: starting... \n");
	#endif
   	sigset_t mask;
   	pthread_sigmask(SIG_BLOCK, &mask, NULL);
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR2);
   	bool isSigmaskSuccess = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	#ifdef DEBUG_WALKING_AND_ACTIONS_SIGNAL_ON
   		if(isSigmaskSuccess)
   		{
   			printf("[StartWalking]: Sigmask failed\n");
   		}
   		else
   		{
   			printf("[StartWalking]: Sigmask success\n");
   		}
	#endif


   	pthread_mutex_lock(& m_walkingAndActionsMutex);
	#ifdef DEBUG_WALKING_AND_ACTIONS_ON
		printf("[StartWalking]: Mutex locked\n");
	#endif


	//check if the robot is walking, if not start walking.
	if(IsWalking() == false)
	{
		//start the walking.
		m_walking->Start();
	}

	//cout << "locking mutex in StartwalkingSafe\n";
	while( IsActionOnGoing() == true )
	{
		usleep(8000);
	}
	//walking enable control of the body
	m_walking->m_Joint.SetEnableBodyWithoutHead(true,true);


	//check if the values of the arguments are not exceeding the
	if (xSpeed < m_minXSpeed) xSpeed=m_minXSpeed;
	if (xSpeed > m_maxXSpeed) xSpeed=m_maxXSpeed;
	if (ySpeed < m_minYSpeed) ySpeed=m_minYSpeed;
	if (ySpeed > m_maxYSpeed) ySpeed=m_maxYSpeed;
	if (angle < m_maxRightTurningAngle) angle=m_maxRightTurningAngle;
	if (angle >  m_maxLeftTurningAngle) angle=m_maxLeftTurningAngle;

	if (ySpeed < MAX_SPEED_TO_SLOW_FIRST)
	{
		while ( (ySpeed - GetMotionValue(Y_SPEED)) > 0 && m_isThreadRunning)
		{
			ChangeMotionValue(Y_SPEED,1);
			usleep(40*1000);
		}

		while ( (ySpeed - GetMotionValue(Y_SPEED)) < 0 && m_isThreadRunning)
		{
			ChangeMotionValue(Y_SPEED,-1);
			usleep(40*1000);
		}
	}
	if (angle < MAX_SPEED_TO_SLOW_FIRST)
	{
		while ( (angle - GetMotionValue(ROTATION_ANGLE)) > 0 && m_isThreadRunning)
		{
			ChangeMotionValue(ROTATION_ANGLE,1);
			usleep(40*1000);
		}

		while ( (angle - GetMotionValue(ROTATION_ANGLE)) < 0 && m_isThreadRunning)
		{
			ChangeMotionValue(ROTATION_ANGLE,-1);
			usleep(40*1000);
		}
	}
	while ( (xSpeed - GetMotionValue(X_SPEED)) > 10 && m_isThreadRunning)
	{
		#ifdef HIP_PITCH_OFFSET_ON
			if(oldXSpeed <= 0 && xSpeed > 0 && GetMotionValue(X_SPEED) >= 0 && GetMotionValue(X_SPEED) < 3 && m_walkingMode == RUNNING)
			{
				m_walking->HIP_PITCH_OFFSET = m_runningHipPitchOffsetForward;
				isHipPitchOffsetSet = true;
			}
		#endif
		ChangeMotionValue(X_SPEED,3);
		usleep(100*1000);
	}

	while ( (xSpeed - GetMotionValue(X_SPEED)) < -10 && m_isThreadRunning)
	{
		#ifdef HIP_PITCH_OFFSET_ON
			if(oldXSpeed >= 0 && xSpeed < 0 && GetMotionValue(X_SPEED) <= 0 && GetMotionValue(X_SPEED) > -3 && m_walkingMode == RUNNING)
			{
				m_walking->HIP_PITCH_OFFSET = m_runningHipPitchOffsetBackward;
				isHipPitchOffsetSet = true;
			}
		#endif
		ChangeMotionValue(X_SPEED,-3);
		usleep(100*1000);
	}

	#ifdef HIP_PITCH_OFFSET_ON
		if(oldXSpeed >= 0 && xSpeed < 0 && !isHipPitchOffsetSet && m_walkingMode == RUNNING)
		{
			m_walking->HIP_PITCH_OFFSET = m_runningHipPitchOffsetBackward;
		}
		if(oldXSpeed <= 0 && xSpeed > 0 && !isHipPitchOffsetSet && m_walkingMode == RUNNING)
		{
			m_walking->HIP_PITCH_OFFSET = m_runningHipPitchOffsetForward;
		}
	#endif
	m_walking->X_MOVE_AMPLITUDE = xSpeed;

	while ( (ySpeed - GetMotionValue(Y_SPEED)) > 0 && m_isThreadRunning)
	{
		ChangeMotionValue(Y_SPEED,1);
		usleep(40*1000);
	}
	while ( (ySpeed - GetMotionValue(Y_SPEED)) < 0 && m_isThreadRunning)
	{
		ChangeMotionValue(Y_SPEED,-1);
		usleep(40*1000);
	}
	while ( (angle - GetMotionValue(ROTATION_ANGLE)) > 0 && m_isThreadRunning)
	{
		ChangeMotionValue(ROTATION_ANGLE,1);
		usleep(40*1000);
	}
	while ( (angle - GetMotionValue(ROTATION_ANGLE)) < 0 && m_isThreadRunning)
	{
		ChangeMotionValue(ROTATION_ANGLE,-1);
		usleep(40*1000);
	}

	m_isThreadRunning = false;
	pthread_mutex_unlock(& m_walkingAndActionsMutex);
	#ifdef DEBUG_WALKING_AND_ACTIONS_ON
		printf("[StartWalking]: Mutex unlocked\n");
	#endif

	#ifdef DEBUG_WALKING_AND_ACTIONS_CHANGE_OFFSET
		cout << "[WalkingAndActions::StartWalking]: HIP_PITCH_OFFSET = " << m_walking->HIP_PITCH_OFFSET << "\n";
	#endif

	isSigmaskSuccess = pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
	#ifdef DEBUG_WALKING_AND_ACTIONS_SIGNAL_ON
	if(isSigmaskSuccess)
	{
		printf("[StartWalking]: Sigmask failed\n");
	}
	else
	{
		printf("[StartWalking]: Sigmask success\n");
	}
	#endif
	//cout << "leaving StartwalkingSafe\n";
	//return success
	return true;
}

///*****************************************************************************************
// * Function Name: StartWalking
// * Description:   This function starts the walking motion with speed values. if walking already started,
// * 				  return false. if the arguments of speed values exceeds the max and min values
// * 				  throw an exception. the function zeros the speeds.
// * Arguments:	  xSpeed - the x axis speed
// * 				  ySpeed - the y axis speed
// * 				  angle  - strength of angle turn
// * Return Values: Return false when walking already started, and True on success
// *****************************************************************************************/
//bool WalkingAndActions::StartWalking(double xSpeed, double ySpeed, double angle)
//{
//	pthread_mutex_lock(& m_walkingAndActionsMutex);
//
//	while( IsActionOnGoing() == true )
//	{
//		usleep(8000);
//	}
//	//walking enable control of the body
//	m_walking->m_Joint.SetEnableBodyWithoutHead(true,true);
//
//	//check if the values of the arguments are not exceeding the
//	if( ( xSpeed < m_minXSpeed )            || ( xSpeed > m_maxXSpeed )  	||
//		( ySpeed < m_minYSpeed )     		|| ( ySpeed > m_maxYSpeed )		||
//		( angle < m_maxRightTurningAngle )  || ( angle >  m_maxLeftTurningAngle )
//	  )
//	{
//		throw "arguments are exceeding max values";
//	}
//
//	m_walking->X_MOVE_AMPLITUDE = xSpeed;
//	m_walking->Y_MOVE_AMPLITUDE = ySpeed;
//	m_walking->A_MOVE_AMPLITUDE = angle;
//
//	//check if the robot is walking, if not start walking.
//	if(IsWalking() == false )
//	{
//		//start the walking.
//		m_walking->Start();
//	}
//
//	pthread_mutex_unlock(& m_walkingAndActionsMutex);
//
//	//return success
//	return true;
//}

void WalkingAndActions::StartWalkingAsync(double xSpeed, double ySpeed, double angle)
{
	if(xSpeed == GetMotionValue(X_SPEED) && ySpeed == GetMotionValue(Y_SPEED) && angle == GetMotionValue(ROTATION_ANGLE))
	{
		return;
	}
	m_xSpeed = xSpeed;
	m_ySpeed = ySpeed;
	m_angle = angle;
	StopThreadRunning();
   	m_isThreadRunning = true;
   	if (MyRobot::GetPlayerMode() == STRIKER_MODE)
	{
   		pthread_create( &m_myThread, NULL, WalkingAndActions::StartWalking, (void*)(this));
	}
   	else
   	{
   		StartWalking(xSpeed, ySpeed, angle);
   	}
}

/*****************************************************************************************
 * Function Name: ChangeMotionValue
 * Description:   This function increases the value of a chosen axis
 * 				  if the new value exceeds the maximum value than maximum value is taken
 * 				  (same for minimum)
 * Arguments:	  motionArg - chosen axis to change the value to
 * 				  increaseDecreaseSpeedBy - increase(+) or decrease (-) the speed by
 * Return Values: Return the value of the chosen axis
 *****************************************************************************************/
double WalkingAndActions::ChangeMotionValue(MotionArgs_t motionArg, double increaseDecreaseSpeedBy)
{
	double *value;
	double max;
	double min;


	//check if the the robot is in motion, if not throw exception
	if (IsActionOnGoing() == true)
	{
//		throw "robot is not in motion";
		return 0;
	}

	if(IsWalking() == false)
	{
		StartWalking();
	}

	switch(motionArg)
	{
	case X_SPEED:
		//get the pointer to m_walking->X_MOVE_AMPLITUDE
		value = &(m_walking->X_MOVE_AMPLITUDE);

		//get the pointer to m_maxXSpeed
		max = m_maxXSpeed;

		//get the pointer to m_minXSpeed
		min = m_minXSpeed;

		break;

	case Y_SPEED:
		//get the pointer to m_walking->Y_MOVE_AMPLITUDE
		value = &(m_walking->Y_MOVE_AMPLITUDE);

		//get the pointer to m_maxLeftYawSpeed
		max = m_maxYSpeed;

		//get the pointer to m_maxRightYawSpeed
		min = m_minYSpeed;

		break;

	case ROTATION_ANGLE:
		//get the pointer to m_walking->Y_MOVE_AMPLITUDE
		value = &(m_walking->A_MOVE_AMPLITUDE);

		//get the pointer to m_maxLeftTurningAngle
		max = m_maxLeftTurningAngle;

		//get the pointer to m_maxRightTurningAngle
		min = m_maxRightTurningAngle;

		break;

	default: //if no such walking attribute throw exception

		throw "no such walking attribute";

		break;
	}

	//if the new target value is about to cross the max value than insert the max in the vaule
	if( ( *value + increaseDecreaseSpeedBy ) > max)
	{
		*value = max;
	}
	//if the new target value is about to cross the max value than insert the max in the vaule
	else if( ( *value + increaseDecreaseSpeedBy ) < min)
	{
		*value = min;
	}
	//else insert calculate new attribute
	else
	{
		*value += increaseDecreaseSpeedBy;
	}


	//return the new set attribute
	return *value;
}

/*****************************************************************************************
 * Function Name: GetMotionValue
 * Description:   This method returns the motion value by a given attribute
 * Arguments:	  motionArg - chosen axis to change the value to return
 * Return Values: Return the value of the chosen axis
 *****************************************************************************************/
double WalkingAndActions::GetMotionValue(MotionArgs_t motionArg)
{

	double currentSpeed;
	switch(motionArg)
	{
		//return the X axis speed
		case X_SPEED:
			currentSpeed = m_walking->X_MOVE_AMPLITUDE;
			break;

		//return the Y axis speed
		case Y_SPEED:
			currentSpeed = m_walking->Y_MOVE_AMPLITUDE;
			break;

		//return the ROTATION_ANGLE
		case ROTATION_ANGLE:
			currentSpeed = m_walking->A_MOVE_AMPLITUDE;
			break;

		//if no such attribute throw exception
		default:
			throw "no such walking attribute";
			break;
	}


	return currentSpeed;
}


/*****************************************************************************************
 * Method Name:   StartAction
 * Description:   This method gets an action and execute it
 * Arguments:     action - action to perform
 *****************************************************************************************/
void WalkingAndActions::StartAction(Actions_t action)
{
	#ifdef DEBUG_START_ACTION
		cout << "[StartAction]: Action type: " << action << "\n";
	#endif
	#ifdef DEBUG_START_ACTION
		printf("[StartAction]: Stopping StartWalking thread...\n");
	#endif
	StopThreadRunning();
	#ifdef DEBUG_START_ACTION
		printf("[StartAction]: StartWalking thread stopped.\n");
	#endif


	sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR2);
	bool isSigmaskSuccess = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	#ifdef DEBUG_START_ACTION
	if(isSigmaskSuccess)
	{
		printf("[StartAction]: Sigmask failed\n");
	}
	else
	{
		printf("[StartAction]: Sigmask success\n");
	}
	#endif
	pthread_mutex_lock(& m_walkingAndActionsMutex);

	#ifdef DEBUG_START_ACTION
		printf("[StartAction]: Mutex has been locked. \n");
	#endif

	//check if in walking mode
	if( IsWalking() == true )
	{
		#ifdef DEBUG_START_ACTION
			printf("[StartAction]: The robot is walking - StopWalking...\n");
		#endif
		StopWalking();
		#ifdef DEBUG_START_ACTION
			printf("[StartAction]: The robot has stopped walking.\n");
		#endif
	}
	while( IsActionOnGoing() == true )
	{
		#ifdef DEBUG_START_ACTION
			printf("[StartAction]: An action is ongoing...\n");
		#endif
		usleep(8000);
	}
	#ifdef DEBUG_START_ACTION
		printf("[StartAction]: Walking and actions has been stopped. \n");
	#endif
	//action module take control of the body without head
	m_action->m_Joint.SetEnableBodyWithoutHead(true,true);

	//action module start the action
	#ifdef DEBUG_START_ACTION
		printf("[StartAction]: Starting the action...\n");
	#endif
	m_action->Start( (int)action );

	#ifdef DEBUG_START_ACTION
		cout << "[StartAction]: Check if action is still running...\n";
	#endif
	while(m_action->IsRunning())
	{
		usleep(8000);
	}
	#ifdef DEBUG_START_ACTION
		cout << "[StartAction]: Action: " << (int)action << " has been done\n";
	#endif
	ResetWalkingParams();
	#ifdef DEBUG_START_ACTION
		cout << "[StartAction]: Walking Params has been reseted\n";
	#endif


	pthread_mutex_unlock(& m_walkingAndActionsMutex);
	#ifdef DEBUG_START_ACTION
		printf("[StartAction]: Mutex has been unlocked. \n");
	#endif

	pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
}

/*****************************************************************************************
 * Function Name: MoveHead
 * Description:   this function moves the head according to requested angles
 * Arguments:	  xSpeed - the x axis head angle
 * 				  ySpeed - the y axis head angle
 *****************************************************************************************/
void WalkingAndActions::MoveHead(double xHeadAngle,double yHeadAngle)
{
	m_head->m_Joint.SetEnableHeadOnly(true, true);
	m_head->MoveByAngle(xHeadAngle,yHeadAngle);
}

/*****************************************************************************************
 * Method Name:   ChangeHeadAngleValue
 * Description:   This method changes the head angle of a given attribute
 *                returns the head angle value of the given attribute after the change
 * Arguments:	  headArg - chosen axis to get the value to return
 * Return Values: Return the value of the chosen axis
 *****************************************************************************************/
double  WalkingAndActions::ChangeHeadAngleValue(HeadArgs_t headArg,double increaseDecreaseAngleBy)
{
	double value;

	    m_head->m_Joint.SetEnableHeadOnly(true, true);
		switch(headArg)
		{
		case X_ANGLE:
			m_head->MoveByAngleOffset(increaseDecreaseAngleBy,0);
			value = m_head->GetPanAngle();
			break;

		case Y_ANGLE:
			m_head->MoveByAngleOffset(0,increaseDecreaseAngleBy);
			value = m_head->GetTiltAngle();
			break;

		default: //if no such walking attribute throw exception
			throw "no such walking attribute";
			break;
		}

return value;
}

/*****************************************************************************************
 * Method Name:   GetHeadAngleValue
 * Description:   This method returns the head angle value by a given attribute
 * Arguments:	  headArg - chosen axis to get the value to return
 * Return Values: Return the value of the chosen axis
 *****************************************************************************************/
double WalkingAndActions::GetHeadAngleValue(HeadArgs_t headArg)
{
	switch(headArg)
		{
			//return the X axis speed
			case X_ANGLE:
				return m_head->GetPanAngle();
				break;

			//return the Y axis speed
			case Y_ANGLE:
				return m_head->GetTiltAngle();
				break;

			//if no such attribute throw exception
			default:
				throw "no such head angle attribute";
				break;
		}
		throw "no such walking attribute";
}
void WalkingAndActions::ResetHeadParams()
{
	m_head->MoveToHome();
}

/*****************************************************************************************
 * Method Name:   GetCM730
 * Description:   This is a static method that returns the instance of the CM730 class
 * Return Values: Return CM730 -  the instance of the class
 *****************************************************************************************/
CM730* WalkingAndActions::GetCM730()
{
	return &m_cm730;
}

/*****************************************************************************************
 * Method Name:   FallenHandler
 * Description:   This is a static method that handles the falling scenario - and get the robot up.
 * Return Values: void
 *****************************************************************************************/
void WalkingAndActions::FallenHandler(int sig)
{
	signal(SIGUSR2, SIG_IGN);

	WalkingAndActions* walkingAndActions = WalkingAndActions::GetInstance();

    if(MotionStatus::FALLEN != STANDUP)
    {
		if(MotionStatus::FALLEN == FORWARD)
		{
			#ifdef DEBUG_FALLEN_HANDLER
				cout << "[FallenHandler]: StartAction with Get_UP_FORWARD - calling...\n";
			#endif
				walkingAndActions->StartAction(GET_UP_FORWARD);   // FORWARD GETUP
			#ifdef DEBUG_FALLEN_HANDLER
				cout << "[FallenHandler]: StartAction with Get_UP_FORWARD - done\n";
			#endif
		}
		else if(MotionStatus::FALLEN == BACKWARD)
		{
			#ifdef DEBUG_FALLEN_HANDLER
				cout << "[FallenHandler]: StartAction with Get_UP_BACKWARD - calling...\n";
			#endif
				walkingAndActions->StartAction(GET_UP_BACKWARD);   // BACKWARD GETUP
			#ifdef DEBUG_FALLEN_HANDLER
				cout << "[FallenHandler]: StartAction with Get_UP_BACKWARD - done\n";
			#endif
		}
    }
    signal(SIGUSR2, FallenHandler);
}

/*****************************************************************************************
 * Method Name:   CheckStatus
 * Description:   This is a static method that checks whether the robot has fallen and signal.
 * Return Values: void*
 *****************************************************************************************/
void *WalkingAndActions::CheckStatus(void *ptr)
{
	#ifdef DEBUG_CHECK_STATUS
		bool isPrintSignaling = true;
	#endif
	cout << "CheckStatus Internal Thread. tid = " << (long int)syscall(224) << endl;
	sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR2);
	bool isSigmaskSuccess = pthread_sigmask(SIG_BLOCK, &mask, NULL);
	BallLocation ballLocation;
	Location* myLocation = Localization::GetInstance()->GetMyLocationByRef();

	while(exitProgram == false)
	{
		#ifdef DEBUG_CHECK_STATUS
			isPrintSignaling = true;
		#endif


		if(MotionStatus::BUTTON == 1)
		{
			m_isLeftButtonPressedLocalization = true;
			m_isLeftButtonPressedAI = true;
		}
		if(MotionStatus::BUTTON == 2)
		{
			m_isMiddleButtonPressedLocalization = true;
			m_isMiddleButtonPressedAI = true;
		}

		ballLocation = Localization::GetInstance()->GetBallLocation();

		#ifdef DEBUG_PRINT_BALL_LOCATION
				cout << "BallLocation : (x,y) = ("<< (int)ballLocation.x << "," <<
						(int)ballLocation.y << ") Distance = " <<
						(int)ballLocation.distanceFromMe << " angle = " <<
						(int)ballLocation.angleFromMe << ". ";
		#endif
		#ifdef DEBUG_PRINT_MY_LOCATION
				cout << "MyLocation : (x,y) = ("<< (int)myLocation->x << "," <<
						(int)myLocation->y << ") Angle = " <<
						(int)myLocation->angle << "\n";
		#endif
		if(MotionStatus::FALLEN != STANDUP)
		{
			#ifdef DEBUG_CHECK_STATUS
				if(isPrintSignaling)
				{
					cout << "[CheckStatus]: The Robot fell signaling\n";
					isPrintSignaling = false;
				}
			#endif
			//pthread_kill((unsigned int)ptr, SIGUSR2);
			if(MotionStatus::FALLEN == FORWARD)
			{
				#ifdef DEBUG_CHECK_STATUS
					cout << "[FallenHandler]: StartAction with Get_UP_FORWARD - calling...\n";
				#endif
				WalkingAndActions::GetInstance()->StartAction(GET_UP_FORWARD);   // FORWARD GETUP
				#ifdef DEBUG_CHECK_STATUS
					cout << "[FallenHandler]: StartAction with Get_UP_FORWARD - done\n";
				#endif
			}
			else if(MotionStatus::FALLEN == BACKWARD)
			{
				#ifdef DEBUG_CHECK_STATUS
					cout << "[FallenHandler]: StartAction with Get_UP_BACKWARD - calling...\n";
				#endif
					WalkingAndActions::GetInstance()->StartAction(GET_UP_BACKWARD);   // BACKWARD GETUP
				#ifdef DEBUG_CHECK_STATUS
					cout << "[FallenHandler]: StartAction with Get_UP_BACKWARD - done\n";
				#endif
			}
		}
		usleep(1000 * 250);
	}
	cout << "Check Status Thread exited\n";

	return NULL;
}

void WalkingAndActions::StopThreadRunning()
{
	if(m_myThread != 0)
	{
		m_isThreadRunning = false;
		pthread_join(m_myThread,NULL);
	}
}
bool WalkingAndActions::IsThreadRunning()
{
	return m_isThreadRunning;
}

