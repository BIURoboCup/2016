/*
 * GoTo.cpp
 *
 *  Created on: Mar 11, 2012
 *      Author: isri
 */
#include "GoTo.h"
#include "Vision.h"

#define SIGN(x) ( (x) >= 0 ? (1) : (-1) )

#define DEBUG_GO_TO_STATE_MACHINE
//#define DEBUG_GO_TO
//#define DEBUG_GO_TO_XY
//#define DEBUG_GO_TO_EXTENDED
//#define DEBUG_GO_TO_STATE_RAIN_DANCE
//#define DEBUG_GO_TO_STATE_RAIN_DANCE_EXTENDED
//#define DEBUG_TO_TO_Get_X_SPEED
#define DEBUG_CORRECTION_NEEDED
//#define DEBUG_GO_TO_N_CM
#define DEBUG_STATE_GO_KICK
//#define DEBUG_HEAD_KICKING_BALL
//#define QandD_DEBUG
//#define DEBUG_GO_TO_STOP
//#define QandD_DEBUG
//#define DEBUG_IS_IT_POSSIBLE_TO_KICK
#define WITH_N_CMS_STATE
//#define DEBUG_QUICK_AND_DIRTY


GoTo* GoTo::m_Instance = 0;
PlayerMode_t GoTo::m_PlayerMode = STRIKER_MODE;

static Vision* m_Vision = Vision::GetInstance();

/*****************************************************************************************
 * Static Method Name:   GetThreadStatus
 * Description:   This Method returns the thread status
 * Return Values: ThreadStat_t
 *****************************************************************************************/
ThreadStat_t GoTo::GetThreadStatus()
{
	return m_ThreadStat;
}



/*****************************************************************************************
 * Static Method Name:   GetInstance
 * Description:   This Method returns an Instance of the class
 * Return Values: An Instance of the class
 *****************************************************************************************/
GoTo* GoTo::GetInstance()
{
	//check to see if already got an instance before
	if(m_Instance == 0)
	{
		m_Instance = new GoTo();
		return m_Instance;
	}

	//return the GoTo instance
	return m_Instance;
}



/*****************************************************************************************
 * Method Name:   GetBallLocation
 * Description:   This function returns the Ball location from localization
 * Return Values: Return BallLocation XY
 *****************************************************************************************/
BallLocation GoTo::GetBallLocation()
{
	return m_Localization->GetBallLocation();
}

/*****************************************************************************************
 * Method Name:   StartGoToBall
 * Description:   This Starts the algorithm of going to the ball
 *****************************************************************************************/
void GoTo::StartGoToBall()
{
	if( GetThreadStatus() == IDLE || m_GoToType != BALL )
	{
		#ifdef DEBUG_GO_TO_BALL
			cout <<"GoTo::StartGoToBall: Start go to ball\n";
		#endif
		m_CurrentState = STATE_DECISION_MAKING;
		m_IsReached = false;
		Stop();
		while( GetThreadStatus() != IDLE )
		{
			usleep( 50 * 1000 );
		}
		m_GoToType = BALL;
		StartInternalThread();
	}
}

/*****************************************************************************************
 * Method Name:   StartGoToXY
 * Description:   This Starts the algorithm of going to XY point
 * Arguments  :   XY point in the field
 *****************************************************************************************/
void GoTo::StartGoToXY(XYLocation_t xy, double angle)
{
	m_XY = xy;
	m_angleToReach = angle;
	m_GoToType = XY;

	if(fabs(angle) <= 180)
	{
		m_isNeedToTurn = true;
	}
	else
	{
		m_isNeedToTurn = false;
	}
	#ifdef DEBUG_GO_TO_XY
		cout << "[GoTo::StartGoToXY]: in startGoToXY " << m_GoToType << "\n";
	#endif
	if( GetThreadStatus() == IDLE || m_GoToType != XY )
	{
		if (GetThreadStatus() != IDLE)
		{
			Stop();
		}

		m_ThreadStat = STARTED; // no one else will enter

		#ifdef DEBUG_GO_TO_XY
			cout << "[GoTo::StartGoToXY]: Starting go to XY...\n";
		#endif
		m_IsReached = false;
	    m_GoToType = XY;
		StartInternalThread();
	}
}

/*****************************************************************************************
 * Method Name:   InternalThreadFunc
 * Description:   This Starts the algorithm of going to...
 * 				  To initiate GoTo XY, one have to set a point first
 *****************************************************************************************/
void GoTo::InternalThreadFunc()
{
	m_isKeepMeAlive = false;
	m_IsReached     = false;
	cout << "GoTo Internal Thread. tid = " << (long int)syscall(224) << endl;
	#ifdef DEBUG_GO_TO
		cout << "[GoTo]: in InternalThreadFunc " << m_GoToType << "\n";
	#endif

	//Check what type of go to defined
	if( m_GoToType == UNDEFTYPE )
	{
		cout << "[GoTo::InternalThreadFunc]: undefined go to type\n";
		return;
	}
	//Start GoTo Ball
	else if( m_GoToType == BALL )
	{
		m_ThreadStartStop = START;
		m_ThreadStat = STARTED;
		#ifdef DEBUG_GO_TO
			cout << "[GoTo]: debug goto ball \n";
		#endif
		GoToBall();
	}
	//Start GoTo XY
	else if( m_GoToType == XY )
	{
		m_ThreadStartStop = START;
		m_ThreadStat = STARTED;
		//printf("debug goto xy \n");
		GoToXY();
	}
	m_ThreadStat = IDLE;
	m_GoToType   = UNDEFTYPE;
}

/*****************************************************************************************
 * Method Name:   GoTo
 * Description:   Constructor of the GoTo class
 *****************************************************************************************/
GoTo::GoTo()
{
	m_GoToType     	  = UNDEFTYPE;
	m_Localization 	  = Localization::GetInstance();
	m_Motion	   	  = WalkingAndActions::GetInstance();
	m_ThreadStat   	  = IDLE;
	m_ThreadStartStop = STOP;
	m_Vision		  = Vision::GetInstance();
	m_myLocPtr		  = m_Localization->GetMyLocationByRef();
}


/*****************************************************************************************
 * Method Name:   GoToBall
 * Description:   This function is the main of the Go To Ball algorithm
 *****************************************************************************************/
void GoTo::GoToBall()
{
	m_PrevState = STATE_GOTO_UNINITIALIZED;
	while(m_ThreadStartStop == START)
	{
		if (m_PrevState != m_CurrentState)
		{
			m_PrevState = m_CurrentState;
			#ifdef DEBUG_GO_TO_STATE_MACHINE
				cout << "[GoTO]:current state of GoToBall [" << PrintStateGoTo(m_CurrentState) << "] with state number [" << m_CurrentState << "] ,time: " << GetCurrentTime() <<  "\n";
			#endif
			switch(m_CurrentState)
			{

				case STATE_DECISION_MAKING:
				//in this state we check what is the current x y of the robot and decide what path to take
					StateDecisionMaking();
				break;
				case STATE_GO_DIRECTLY_TO_BALL:
					//in this state we go directly to the ball
					StateGoDirectlyToBall(STATE_RAIN_DANCE);
				break;

				case STATE_GO_N_CM_FROM_BALL:
					StateGoNcmFromBall();
				break;

				case STATE_GO_KICK:
					StateGoKick();
				break;

				case STATE_RAIN_DANCE:
					StateRainDance();
				break;

				default:
					throw "Unknown GoTo state";
				break;
			}
		}
	}
	//Stop the walking
	m_Motion->StopWalking();
}

string GoTo::PrintStateGoTo(GoToBallStates_e state)
{
    switch(state)
    {
    case STATE_GO_DIRECTLY_TO_BALL:
    	return "STATE_GO_DIRECTLY_TO_BALL";
    case STATE_DECISION_MAKING:
    	return "STATE_DECISION_MAKING";
    case STATE_RAIN_DANCE:
    	return "STATE_RAIN_DANCE";
    case STATE_GO_N_CM_FROM_BALL:
    	return "STATE_GO_N_CM_FROM_BALL";
    case STATE_GO_KICK:
    	return "STATE_GO_KICK";
    default:
    	return "Unknown GoTo state";
    }
}

/*****************************************************************************************
 * Method Name:   GoToXY
 * Description:   This function is the main of the Go To XY point algorithm
 *****************************************************************************************/
void GoTo::GoToXY()
{

	//loop until thread status is STOP
	while(m_ThreadStartStop == START)
	{
	//	#ifdef DEBUG_GO_TO_XY
			cout << "[GoTo::GoToXY]: Going to (" << m_XY.x << "," << m_XY.y << ")\n";
		//#endif
		if( QuickAndDirty(m_XY.x, m_XY.y, m_angleToReach, false) == true )
		{
			m_IsReached = true;
			m_ThreadStartStop = STOP;
		}
		else
		{
			m_IsReached = false;
		}

		//wait for 100 ms
		usleep( 100 * 1000 );
	}
	m_Motion->StopWalking();
}

/*****************************************************************************************
 * Method Name:   CalculateABParametersOFOppGoalBallEquation
 * Description:   This method calculates the A B params of the opponent goal vs ball
 * 				  equation y = Ax + B.
 * Parameters:    x,y point
 * Return:		  aParam bParam
 *****************************************************************************************/
void GoTo::CalculateABParametersOFOppGoalBallEquation(double *aParam, double *bParam, double x, double y)
{
	double epsilon = 0;

	// Check if y equals 600 then we add epsilon so the plumb won't be infinite
	if(y == FIELD_HIGHT)
	{
		epsilon = EPSILON;
	}

	*aParam = (y - FIELD_HIGHT - epsilon) / x;
	*bParam = FIELD_HIGHT;
}


/*****************************************************************************************
 * Method Name:   CalculateABParametersOFOppGoalBallEquationPlumb
 * Description:   This method calculates the A B params of the opponent goal vs ball
 * 				  equation y = Ax + B.
 * Parameters:    x,y point
 * Return:		  aParam bParam
 *****************************************************************************************/
void GoTo::CalculateABParametersOFOppGoalBallEquationPlumb(double *aParam, double *bParam, double x, double y)
{
	double epsilon = 0;

	// Check if y equals 600 then we add epsilon so the plumb won't be infinite
	if(y == FIELD_HIGHT)
	{
		epsilon = EPSILON;
	}

	*aParam	= x / (epsilon + FIELD_HIGHT - y);
	*bParam = (x * x) / (y - FIELD_HIGHT - epsilon) + y;
}

/*****************************************************************************************
 * Method Name:   CalculateABParametersOFBallOurGoalEquatio
 * Description:   This method calculates the A B params of the our goal vs ball
 * 				  equation y = Ax + B.
 * Parameters:    x,y point
 * Return:		  aParam bParam
 *****************************************************************************************/
void GoTo::CalculateABParametersOFBallOurGoalEquation(double *aParam, double *bParam, double x, double y)
{
	//if x is 0 the get the value epsilon
	if(x == 0)
	{
		x = EPSILON;
	}

	*aParam = y/x;
	*bParam = 0;
}



/*****************************************************************************************
 * Method Name:   CalculateYValue
 * Description:   This method calculates Y value of line equetion given a and b params of
 * 				  y = Ax + B and x
 * Parameters:    aParam bParam x
 * Return:		  y point
 *****************************************************************************************/
double GoTo::CalculateYValue(double aParam, double bParam, double x)
{
	return aParam * x + bParam;
}

/*****************************************************************************************
 * Method Name:   CalculateDistanceBetweenTwoPoints
 * Description:   This method calculates distance between two points
 * Parameters:    Point x1 y1 and point x2 y2
 * Return:		  Distance
 *****************************************************************************************/
double GoTo::CalculateDistanceBetweenTwoPoints(double x1, double y1, double x2, double y2)
{
	return sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) );
}


/*****************************************************************************************
 * Method Name:   CalculateDistanceBetweenPointAndLine
 * Description:   This method calculates distance between two points
 * Parameters:    Point x1 y1 and point x2 y2
 * Return:		  Distance
 *****************************************************************************************/
double GoTo::CalculateDistanceBetweenPointAndLine(double aParam, double bParam, double x, double y)
{
	double xProjection, yProjection, temp;
	temp = x - bParam + ( aParam * y );
	xProjection =  ( temp / 2 ) + bParam;
	yProjection =  temp / (2 * aParam);
	return CalculateDistanceBetweenTwoPoints(xProjection, yProjection, x, y);
}


/*****************************************************************************************
 * Method Name:   CalculateXYProjectionOnALine
 * Description:   This method calculates The Projection of X and Y on a line
 * Parameters:    Point A B, x y
 * Return:		  yProj, xProj
 *****************************************************************************************/
void GoTo::CalculateXYProjectionOnALine(double aParam, double bParam, double x, double y, double *xProj, double *yProj)
{
	double aInv;

	//if aParam is 0 assign epsilon because of devision by 0
	if( aParam == 0 )
	{
		aParam = EPSILON;
	}

//	*yProj = (2/3)*( m_Localization->GetBallLocation().y - y );
//	*xProj = ( *yProj - bParam ) / aParam;
//
//	if( fabs(*xProj) > 200 )
//	{
//		*xProj = x;
//	}

	aInv = 1 / aParam;

	//calculate the x projection
	*xProj = ( ( -aInv * x ) + y - bParam ) / ( aParam + aInv );

	//calculate the y projection
	*yProj = ( *xProj * aParam ) + bParam;
}



/*****************************************************************************************
 * Method Name:   CalculateAngleFromXY
 * Description:   This method calculates the angle from XY point
 * Parameters:    Point x1 y1 and point x2 y2 (my location should be x1 x2)
 * Return:		  angle degrees that need to be  turned, and a direction,
 * 				  (-) turn right and (+) turn left ( aligned with speeds )
 * 									y
 * 									|
 * 							second	|  first
 * 						  __________|___________x
 * 						            |
 * 							third	|  fourth
 * 									|
 *****************************************************************************************/
double GoTo::CalculateAngleFromXY(double x1, double y1, double x2, double y2, double myAngle)
{
	double angle = 0;
	double absDiffX , absDiffY, diffAngle;


	// calculate the abs diff between x1 x2 and y1 y2
	absDiffX = fabs(x1 - x2);
	absDiffY = fabs(y1 - y2);

	//first quarter
	if( ( x1 <= x2 ) && ( y1 < y2 ) )
	{
			angle = ATanDeg( absDiffX / absDiffY );
	}

	//second quarter
	else if( ( x1 > x2 ) && ( y1 < y2 ) )
	{
			angle = -ATanDeg( absDiffX / absDiffY );
	}

	//third quarter
	else if( ( x1 > x2 ) && ( y1 >= y2 ) )
	{
			angle = -ATanDeg( absDiffY / absDiffX ) - 90;
	}

	//fourth quarter
	else if( ( x1 <= x2 ) && ( y1 >= y2 ) )
	{
		//check if both are the same then return 0
		if( ( absDiffX == 0 ) && ( absDiffY == 0 ) )
		{
			angle = 0;
		}

		else if( absDiffX == 0 )
		{
			angle = 180;
		}

		else
		{
			angle = ATanDeg( absDiffY / absDiffX ) + 90;
		}
	}

	//calculate the relative angle between me and the ball
	diffAngle = myAngle - angle;

	//condition of turning right
	if( diffAngle >= 180 )
	{
		return -( 360 - diffAngle );
	}

	//condition of turning left
	else if( ( diffAngle < 180 ) && ( diffAngle >= 0 ) )
	{
		return diffAngle;
	}

	//condition of turning right
	else if( ( diffAngle >= -180) && ( diffAngle < 0 ) )
	{
		return diffAngle;
	}

	//condition of turning right
	else
	{
		return 360 - fabs(diffAngle);
	}
}

/*****************************************************************************************
 * Method Name:   ATanDeg
 * Description:   This method returns arc tangent in degrees
 * Parameters:    value
 * Return:		  angle in degrees
 *****************************************************************************************/
double GoTo::ATanDeg(double value)
{
	return atan (value) * 180 / PI;
}

/*****************************************************************************************
 * Method Name:   GetXSpeed
 * Description:   This method returns the x speed given a distance and an angle
 * 				  The X speed will get weight by the angle
 * Parameters:    distance, angle, finalXY
 * Return:		  x speed
 *****************************************************************************************/
double GoTo::GetXSpeed(double distance, double angle, bool finalXY)
{
	double speed;
	// if final state then take the distance into care
	if( finalXY )
	{
		if( distance < SLOWING_DOWN_DISTANCE )
		{
			speed = RUNNING_MAX_FORWARD_SPEED / 5;
		}
		else if( distance < FAR_DISTANCE )
		{
			speed = RUNNING_MAX_FORWARD_SPEED / 1.5;
		}
		else
		{
			speed = RUNNING_MAX_FORWARD_SPEED;
		}
	}
	else
	{
		speed = RUNNING_MAX_FORWARD_SPEED;
	}

	angle = fabs( angle );

	//consider the relative angle and give angle
	if(angle > 50)
	{
		speed = 0;
	}
	else if(angle > 30)
	{
		speed = 0;
	}
	else if(angle > 20)
	{
		speed = 0;
	}
	else if(angle > 12)
	{
		speed = speed * 0.3;
	}
	else if(angle > 5)
	{
		speed = speed * 0.7;
	}

	#ifdef DEBUG_TO_TO_Get_X_SPEED
		cout << "[GoTo::GetXSpeed]: distance = " << distance << "angle = " << angle << "speed = " << speed << "\n";
	#endif
	//return the speed
	return speed;
}

/*****************************************************************************************
 * Method Name:   GetTurnSpeed
 * Description:   This method returns the turn speed given an angle
 * Parameters:    distance, angle, finalXY
 * Return:		  Turn speed
 *****************************************************************************************/
double GoTo::GetTurnSpeed(double angle, double distance, bool smooth)
{
	double factor = 1;
	double turnSpeed = 0;

	//factor for the direction of the turn, -1 means right and 1 means left
	if( angle < 0 )
	{
		factor = -1;
	}

	//take absolute angle
	angle = fabs(angle);
	if( smooth == false)
	{
		//get the speed by the angle
		if( ( angle > 5 ) && ( angle <= 10 ) )
		{
			turnSpeed = MAX_TURN_LEFT_SPEED / 5;
		}
		if( ( angle > 10 ) && ( angle <= 20 ) )
		{
			turnSpeed = (MAX_TURN_LEFT_SPEED * 2) / 5;
		}
		else if( ( angle > 20 ) && ( angle <= 30 ) )
		{
			turnSpeed = MAX_TURN_LEFT_SPEED;
		}
		else if( angle > 30 )
		{
			turnSpeed = MAX_TURN_LEFT_SPEED;
		}
	}
	else
	{
		if( distance > FAR_DISTANCE )
		{
			//get the speed by the angle
			if( ( angle > 10 ) && ( angle <= 20 ) )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED / 6;
			}
			else if( ( angle > 20 ) && ( angle <= 30 ) )
			{
				turnSpeed = ( MAX_TURN_LEFT_SPEED * 2) / 5;
			}
			else if( angle > 30 )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED;
			}
		}
		else if( distance > SLOWING_DOWN_DISTANCE  )
		{
			//get the speed by the angle
			if( ( angle > 5 ) && ( angle <= 10 ) )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED / 9 *2;
			}
			else if( ( angle > 10 ) && ( angle <= 20 ) )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED / 2;
			}
			else if ( angle > 20 )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED;
			}
		}
		else
		{
			//get the speed by the angle
			if( ( angle > 5 ) && ( angle <= 13 ) )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED /4;
			}
			else if( ( angle > 13 ) && ( angle <= 24 ) )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED / 2;
			}
			else if ( angle > 24 )
			{
				turnSpeed = MAX_TURN_LEFT_SPEED;
			}
		}
	}

	//return the speed
	return ( factor * turnSpeed );
}


/*****************************************************************************************
 * Method Name:   IsReachedBall
 * Description:   this method returns true if we reached a destination and false otherwise
 * Return:		  bool
 *****************************************************************************************/
void GoTo::ResetIsReached()
{
	m_IsReached = false;
}


/*****************************************************************************************
 * Method Name:   IsReachedBall
 * Description:   this method returns true if we reached a destination and false otherwise
 * Return:		  bool
 *****************************************************************************************/
bool GoTo::IsReached()
{
	return m_IsReached;
}



bool GoTo::IsKeepMeAlive()
{
	return m_isKeepMeAlive;
}

/*****************************************************************************************
 * Method Name:   Stop
 * Description:   this method stops the thread
 *****************************************************************************************/
void GoTo::Stop()
{
	#ifdef DEBUG_GO_TO_STOP
		cout <<"GoTo::Stop: stopping GoTo thread.\n";
	#endif
	m_ThreadStartStop = STOP;
	while( GetThreadStatus() != IDLE )
	{
		usleep(50 * 1000);
	}
	#ifdef DEBUG_GO_TO_STOP
		cout <<"GoTo::Stop: GoTo thread stopped.\n";
	#endif

	cout << "GoTo Thread exited\n";

}

/*****************************************************************************************
 * Method Name:   StateDecisionMaking
 * Description:   this state checks the x y of the Robot and the projection of the robot
 * 				  X Y to the G and L lines and decide where to go
 *****************************************************************************************/
void GoTo::StateDecisionMaking()
{
	BallLocation ballLocation;
	Location     myLocation;
//	double		 Yg, Xg;
//	double		 Yl, Xl;
//	double		 Ag,Bg;
//	double		 Al,Bl;
//	double 		 Dl,Dg;
	double       x,y;


	myLocation = m_Localization->GetMyLocation();
	ballLocation = m_Localization->GetBallLocation();

	#ifdef DEBUG_GO_TO_EXTENDED
			cout << "[GoTo::StateGoNcmFromBall]: ball location x, y #" << ballLocation.x << " " << ballLocation.y << "\n";
			cout << "[GoTo::StateGoNcmFromBall]: my location x, y #" << myLocation.x << " " << myLocation.y << "\n";
	#endif


	//if the distance from ball is lower than rain dance distance start rain dance
	if( ballLocation.distanceFromMe < RAIN_DANCE_DISTANCE_CM )
	{
		m_CurrentState = STATE_RAIN_DANCE;
		return;
	}

	//if the robot located between the opponent goal and the ball,
	//go directly to the ball
	if( myLocation.y > ballLocation.y )
	{
		m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
		return;
	}

	//get the N cm point from the ball
	CalculateNPoint(ballLocation.x, ballLocation.y, GO_N_FROM_BALL, &x, &y);

	if( ( y < DEFENCE_AREA_Y_CM ) && ( fabs(x) > fabs(myLocation.x) ))
	{
		m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
		return;
	}


	//if my Location y is greater than the y point, go directly to ball than.
	if( y < myLocation.y || m_PlayerMode == GOALIE_MODE )
	{
		m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
		return;
	}

	if( ballLocation.y > OFFENSIVE_POSITION_Y_CM && fabs(ballLocation.x) < POLE_X_CM )
	{
		m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
		return;
	}

	#ifdef WITH_N_CMS_STATE
	m_CurrentState = STATE_GO_N_CM_FROM_BALL;
	#else
	m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
	#endif
//	//calculate all needed params for making a decision
//	CalculateABParametersOFOppGoalBallEquation(&Ag, &Bg, ballLocation.x, ballLocation.y);
//	CalculateABParametersOFBallOurGoalEquation(&Al, &Bl, ballLocation.x, ballLocation.y);
//	CalculateXYProjectionOnALine(Ag, Bg, myLocation.x, myLocation.y, &Xg, &Yg);
//	CalculateXYProjectionOnALine(Al, Bl, myLocation.x, myLocation.y, &Xl, &Yl);
//	Dg = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y,Xg,Yg);
//	Dl = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y,Xl,Yl);
//#ifdef DEBUG_GO_TO
//	//if(counter == 3)
//	//	exit(1);
//
//	//counter++;
//	cout << "[GoTo::StateDecisionMaking]: G line A, B #" << Ag << " " << Bg << "\n";
//	cout << "[GoTo::StateDecisionMaking]: L line A, B #" << Al << " " << Bl << "\n";
//
//	cout << "[GoTo::StateDecisionMaking]: ball location x, y #" << ballLocation.x << " " << ballLocation.y << "\n";
//	cout << "[GoTo::StateDecisionMaking]: my location x, y #" << myLocation.x << " " << myLocation.y << "\n";
//	cout << "[GoTo::StateDecisionMaking]: Yg Yl #" << Yg << " " << Yl << "\n";
//	//m_CurrentState = STATE_DECISION_MAKING;
//	//usleep (50 * 1000);
//	//return;
//#endif
//	//check if Yg and Yl both over Yb then go straight to ball or if robots
//	//y location is Yb
//	if(
//			( (Yg >= ballLocation.y) && (Yl >= ballLocation.y) ) ||
//			( myLocation.y >= ballLocation.y )
//	  )
//	{
//		m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
//	}
//
//	// if the distance to line g is bigger then the distance to line L
//	// or line l y projection is over Yb and Yg is lower than balllocation y go to line g
//	else if(
//			( ( Dg <= Dl ) || ( Yl >= ballLocation.y ) ) &&
//			( Yg < ballLocation.y )
//	       )
//	{
//		m_CurrentState = STATE_GO_TO_LINE_G;
//	}
//
//	// if the distance to line L is bigger then the distance to line G
//	// or line g y projection is over Yb and Yl is lower than balllocation y go to line l
//	else /*if(
//			( ( Dl < Dg ) || ( Yg >= ballLocation.y ) ) &&
//			( Yl < ballLocation.y )
//		   )*/
//	{
//		m_CurrentState = STATE_GO_TO_LINE_L;
//	}
//	cout << "[GoTo::StateDecisionMaking]: Leaving" << "\n";

}

void GoTo::StateGoNcmFromBall()
{
	BallLocation ballLocation, lastBallLocation;
	Location     myLocation;
	double		 Yg, Xg;
	double 		 Dg,LastDg = 0;
	double       angleFromXY;
	double		 xSpeed;
	double		 turnSpeed;
	double       distanceIncreased = 0;
	int			 reasonableNoiseCounter = 0;

	m_Vision->SetHeadTrackingStatus(BALL_TRACKING);
	lastBallLocation = m_Localization->GetBallLocation();

	//calculate Xg and Yg distanced N from ball
	CalculateNPoint(ballLocation.x, ballLocation.y, GO_N_FROM_BALL, &Xg, &Yg);
	Dg = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y, Xg, Yg);

	if (fabs(Xg - myLocation.x) <  MIN_X_DIFF_FROM_N_CM_POINT)
	{
		//start walking on idle
		m_Motion->StartWalkingAsync(0, 0, 0);

		m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
		return;
	}


	while( ( m_CurrentState == STATE_GO_N_CM_FROM_BALL ) && ( m_ThreadStartStop == START ) )
	{
		//get location and ball location
		myLocation = m_Localization->GetMyLocation();
		ballLocation = m_Localization->GetBallLocation();

		//calculate all needed params for making a decision
		#ifdef DEBUG_GO_TO_EXTENDED
				cout << "[GoTo::StateGoNcmFromBall]: ball location x, y #" << ballLocation.x << " " << ballLocation.y << "\n";
				cout << "[GoTo::StateGoNcmFromBall]: my location x, y #" << myLocation.x << " " << myLocation.y << "\n";
		#endif

		//if ball noise is greater than reasonable noise, go back to decision making state
		if( CalculateDistanceBetweenTwoPoints(ballLocation.x, ballLocation.y, lastBallLocation.x, lastBallLocation.y) > REASONABLE_NOISE_DISTANCE )
		{
			reasonableNoiseCounter++;
			if(reasonableNoiseCounter > REASONABLE_NOISE_TIMES)
			{
				#ifdef DEBUG_GO_TO_N_CM
					cout << " > -----------------------\nREASONABLE_NOISE_DISTANCE\n\n";
				#endif
				//start walking on idle
				m_Motion->StartWalkingAsync(0, 0, 0);
				m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
				return;
			}
		}
		reasonableNoiseCounter = 0;

		if (fabs(ballLocation.angleFromMe) > MAX_ANGLE_FROM_BALL || ballLocation.distanceFromMe < 40)
		{
			//start walking on idle
			m_Motion->StartWalkingAsync(0, 0, 0);

			m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
			return;
		}

		else
		{
			//calculate Xg and Yg distanced N from ball
			CalculateNPoint(ballLocation.x, ballLocation.y, GO_N_FROM_BALL, &Xg, &Yg);
			LastDg = Dg;
			Dg = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y, Xg, Yg);

			if( Dg - LastDg > 40 )
			{
				distanceIncreased++;
			}
			#ifdef DEBUG_GO_TO
					cout << "[GoTo::StateGoNcmFromBall]: going to x, y #" << Xg << " " << Yg << "\n";
					cout << "[GoTo::StateGoNcmFromBall]: distance   #" <<  Dg << "\n\n";
			#endif

			if( Dg < DISTANCE_TO_START_DETECTING_BALL)
			{
				Vision::GetInstance()->SetHeadTrackingStatus(BALL_TRACKING);
			}


			//if the distance between the robot and the line is under N cm,
			//turn towards the ball
			if( Dg < START_TURNING_DISTANCE )
			{
				#ifdef DEBUG_GO_TO
						cout << "[GoTo::StateGoNcmFromBall]: going to ball \n";
				#endif
				m_CurrentState = STATE_GO_DIRECTLY_TO_BALL;
				m_Motion->StartWalking(0,0,0);

				return;
			}

			// go to Xg Yg
			else
			{
				//get my relative angle from me and x,y point vector
				angleFromXY = CalculateAngleFromXY(myLocation.x, myLocation.y, Xg, Yg, (double)myLocation.angle);

				//get x speed
				xSpeed = GetXSpeed(Dg, angleFromXY, true);

				//get turn speed from angle
				turnSpeed = GetTurnSpeed(angleFromXY,Dg,false);

				//start walking with parameters
				m_Motion->StartWalkingAsync(xSpeed,0,turnSpeed);
			}
		}

		lastBallLocation = ballLocation;

		//wait for 150 ms
		usleep( 150 * 1000 );
	}
}


/*****************************************************************************************
 * Method Name:   StateGoDirectlyToBall
 * Description:   this state makes the robot go to the ball on either line l or g where
 * 				  only the next step changes
 * Arguments:	  nextState - the next wanted state
 *****************************************************************************************/
void GoTo::StateGoDirectlyToBall(GoToBallStates_t nextState)
{
	BallLocation ballLocation;
	BallLocation lastBallLocation;
	double xSpeed ,turnSpeed;
	int reasonableNoiseCounter = 0;

	lastBallLocation = m_Localization->GetBallLocation();

	while(
			( m_CurrentState == STATE_GO_DIRECTLY_TO_BALL) &&
			( m_ThreadStartStop == START )
		 )
	{
		ballLocation = m_Localization->GetBallLocation();
		//if ball noise is greater than reasonable noise, go back to decision making state
		if( CalculateDistanceBetweenTwoPoints(ballLocation.x, ballLocation.y, lastBallLocation.x, lastBallLocation.y) > REASONABLE_NOISE_DISTANCE )
		{
			reasonableNoiseCounter++;
			if(reasonableNoiseCounter > REASONABLE_NOISE_TIMES)
			{
				cout << " > -----------------------\nREASONABLE_NOISE_DISTANCE\n\n";
				//start walking on idle
				m_Motion->StartWalkingAsync(0, 0, 0);
				m_CurrentState = STATE_DECISION_MAKING;
				return;
			}
		}
		reasonableNoiseCounter = 0;
		if( ballLocation.distanceFromMe > RAIN_DANCE_DISTANCE_CM)
		{
			//get x speed
			xSpeed = GetXSpeed(ballLocation.distanceFromMe, -ballLocation.angleFromMe, true);

			//get turn speed from angle
			turnSpeed = GetTurnSpeed(-ballLocation.angleFromMe, ballLocation.distanceFromMe, false);

			//start walking with parameters
			m_Motion->StartWalkingAsync(xSpeed,0,turnSpeed);
		}
		else
		{
			//stop the walking
			m_Motion->StopWalking();

			//else stop walking
			m_CurrentState = STATE_RAIN_DANCE;

			return;
		}
		lastBallLocation = ballLocation;
		usleep( 150 * 1000 );
	}
}

/*****************************************************************************************
 * Method Name:   StateGoKick
 * Description:   in this thread we sign that we reached the ball and stop the thread
 *****************************************************************************************/
void GoTo::StateGoKick()
{

	//maybe we should insert some adjustments according to the kick
	BallLocation ballLocation;

	m_Vision->SetHeadTrackingStatus(KICKING_BALL_TRACKING);
	ballLocation = m_Localization->GetBallLocation();

	#ifdef DEBUG_STATE_GO_KICK
		cout << "[GoTo]::StateGoKick. ball distance = " << ballLocation.distanceFromMe << " time since = " << TimeStamp() - ballLocation.time << "\n";
	#endif

	for (int i = 0; i < 3; i++)
	{
		while ((m_Vision->IsClearToKickDistanceFromBall() == false) && IsBallDataRelevant(&ballLocation))
		{
			//cout << "Starting Walking Async\n";
			if(ballLocation.angleFromMe > ZERO_ANGLE_LEFT_CLOSE && ballLocation.angleFromMe < ZERO_ANGLE_RIGHT_CLOSE)
			{
				m_Motion->StartWalkingAsync(RUNNING_MAX_FORWARD_SPEED / 4,0,0);
			}
			else if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_CLOSE)
			{
				if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_CLOSE - 15)
				{
					m_Motion->StartWalkingAsync(0, 0, MAX_TURN_LEFT_SPEED );
				}
				else
				{
					m_Motion->StartWalkingAsync(0, 0, MAX_TURN_LEFT_SPEED / 2 );
				}
			}
			else
			{
				if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_CLOSE + 5)
				{
					m_Motion->StartWalkingAsync(0, 0, MAX_TURN_RIGHT_SPEED );
				}
				else
				{
					m_Motion->StartWalkingAsync(0, 0, MAX_TURN_RIGHT_SPEED / 2 );
				}
			}
			//cout << "After Starting Walking Async\n";
			usleep(50 * 1000);
			ballLocation = m_Localization->GetBallLocation();
		}

		if (!IsBallDataRelevant(&ballLocation))
		{
			m_IsReached = false;
			//stop the thread
			m_ThreadStartStop = STOP;
			//go to type is undefined
			m_GoToType = UNDEFTYPE;
			return;
		}

		usleep(1000 * 50);
		ballLocation = m_Localization->GetBallLocation();
	}
	cout << "Out of For of GoTo::kick\n";
	if(!IsBallDataRelevant(&ballLocation) || ballLocation.distanceFromMe > SLOWING_DOWN_DISTANCE)
	{
		m_IsReached = false;
	}

	//reached the ball
	m_IsReached = true;

	//stop the thread
	m_ThreadStartStop = STOP;

}


/*****************************************************************************************
 * Method Name:   StateRainDance
 * Description:   this state makes the robot circle around the ball
 *****************************************************************************************/
void GoTo::StateRainDance()
{
	BallLocation ballLocation, lastBallLocation;
	Location 	 myLocation;
	double       angleToTurn;
	double       angleFromBallToMe;
	int 		 counter = 0;
	int 		 amIOutExhaustedCounter = 0;
	//bool		 correction = false;

	myLocation = m_Localization->GetMyLocation();

	m_isKeepMeAlive = true;
	#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
			cout << "[GoTo::StateRainDance]: m_isKeepMeAlive: " << m_isKeepMeAlive << "\n";
	#endif

	if ( myLocation.y < OFFENSIVE_POSITION_Y_CM )
	{
		if (m_PlayerMode == STRIKER_MODE)
		{
			m_Motion->SetWalkingMode(WALKING);
			m_Vision->SetHeadTrackingStatus(GOAL_TRACKING);
			m_Vision->ResetSeeGoalFrames();
		}

		cout << "into sleeping do i see goal\n";
		while ((!m_Vision->DoISeeTheGoal() && (m_PlayerMode == STRIKER_MODE ) && (amIOutExhaustedCounter < 40) && (m_ThreadStartStop == START) ))
		{
			amIOutExhaustedCounter++;
			usleep( 100 * 1000 );
		}
		cout << "out of sleeping do i see goal\n";
	}
	m_Vision->SetHeadTrackingStatus(KICKING_BALL_TRACKING);
	ballLocation = m_Localization->GetBallLocation();

#ifdef DEBUG_HEAD_KICKING_BALL
	cout << "GoTo::Into Tracking Kicking Ball\n";
#endif

	cout << "into sleeping do i see kicking ball tracking\n";
	while ( ( (counter < 15) && (!IsBallDataFromNow(&ballLocation)) ) || (counter < 30 && !IsBallDataRelevant(&ballLocation) && (m_ThreadStartStop == START) ))
	{
		ballLocation = m_Localization->GetBallLocation();
		counter++;
		usleep( 100 * 1000 );
	}
	cout << "out of sleeping do i see kicking ball tracking\n";

	#ifdef DEBUG_HEAD_KICKING_BALL
		cout << "GoTo::Out of tracking Kicking Ball\n";
	#endif

	m_isKeepMeAlive = false;
	#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
		cout << "[GoTo::StateRainDance]: m_isKeepMeAlive: " << m_isKeepMeAlive << "\n";
	#endif

	counter = 0;
	while( (m_CurrentState == STATE_RAIN_DANCE) && (m_ThreadStartStop == START) && ( m_Localization->GetBallLocation().distanceFromMe < DISTANCE_OUT_OF_RAIN_DANCE))
	{
		#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
			cout << "[GoTo::StateRainDance]: Current state == STATE_RAIN_DANCE && m_ThreadStartStop == START\n";
		#endif
		//get ball and my location
		ballLocation = m_Localization->GetBallLocation();
		myLocation   = m_Localization->GetMyLocation();

		angleFromBallToMe = (double)(myLocation.angle);

		#ifdef DEBUG_GO_TO_EXTENDED
			cout << "[GoTo::StateRainDance]: angle From me is: " << angleFromBallToMe << "\n";
		#endif

		//calculate the turn angle
		angleToTurn = -CalculateAngleFromXY(ballLocation.x ,ballLocation.y, 0, 600 , 0 );

		#ifdef DEBUG_GO_TO_EXTENDED
			cout << "[GoTo::StateRainDance]: angle from ball to goal is: " << angleToTurn << "\n";
		#endif

		//calculate the angle to turn
		angleToTurn =  angleFromBallToMe - angleToTurn;

		#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
				cout << "[GoTo::StateRainDance]: angle to turn is " << angleToTurn << "\n";
		#endif

		if( IsPossibleToKick(myLocation) )
		{
			#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
				cout << "[GoTo::StateRainDance]: IsPossibleToKick(myLocation)\n";
			#endif
			m_isKeepMeAlive = true;
			#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
					cout << "[GoTo::StateRainDance]: m_isKeepMeAlive: " << m_isKeepMeAlive << "\n";
			#endif

			m_Motion->SetWalkingMode(RUNNING);
			#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
				cout << "[GoTo::StateRainDance]: m_Motion->GetWalkingMode(): " << m_Motion->GetWalkingMode() << "\n";
			#endif
			//usleep(750*1000);
			m_Vision->SetHeadTrackingStatus(KICKING_BALL_TRACKING);
			counter = 0;
			ballLocation = m_Localization->GetBallLocation();
			while ( (counter < 30) && !IsBallDataRelevant(&ballLocation) && (m_ThreadStartStop == START) )
			{
				counter++;
				ballLocation = m_Localization->GetBallLocation();
				usleep( 100 * 1000 );
			}
			m_isKeepMeAlive = false;
			#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
					cout << "[GoTo::StateRainDance]: m_isKeepMeAlive: " << m_isKeepMeAlive << "\n";
			#endif

			m_CurrentState = STATE_GO_KICK;
			return;
		}

/*		else if ((correction == true) && (fabs(ballLocation.angleFromMe) < 7))
		{
			correction = false;
			m_Motion->StopWalking();
		}
*/
		#ifdef DEBUG_CORRECTION_NEEDED
		else if (fabs(ballLocation.angleFromMe) > RAIN_DANCE_ANGLE_CORRECTION)
		{
//			m_Motion->SetWalkingMode(RUNNING);
//			counter=0;
//			correction = true;
			if (ballLocation.angleFromMe > 0)
			{
				#ifdef DEBUG_CORRECTION_NEEDED
					cout << "[GoTo::StateRainDance]: angle is = " << ballLocation.angleFromMe << " turning right\n";
				#endif
				m_Motion->StartWalkingAsync(0,-20,0);
			}
			else
			{
				#ifdef DEBUG_CORRECTION_NEEDED
					cout << "[GoTo::StateRainDance]: angle is = " << ballLocation.angleFromMe << " turning left\n";
				#endif
				m_Motion->StartWalkingAsync(0,20,0);
			}
		}

		else if(fabs(ballLocation.angleFromMe) > RAIN_DANCE_ANGLE_CORRECTION_BACK)
		{
			#ifdef DEBUG_CORRECTION_NEEDED
				cout << "[GoTo::StateRainDance]: angle is = " << ballLocation.angleFromMe << " walking backwards\n";
			#endif
			m_Motion->StartWalkingAsync(-10,0,0);
		}
		#endif

		else if( angleToTurn < 0 )
		{
			#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE_EXTENDED
					cout << "[GoTo::StateRainDance]: turning left\n";
			#endif
			// change walking mode to WALKING.
			m_Motion->SetWalkingMode(WALKING);
			//turn left
			m_Motion->StartWalkingAsync(-3, 20, -11);
			usleep( 50 * 1000 );
		}
		else
		{
			#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE_EXTENDED
					cout << "[GoTo::StateRainDance]: turning right\n";
			#endif
			// change walking mode to WALKING.
			m_Motion->SetWalkingMode(WALKING);
			//turn right
			m_Motion->StartWalkingAsync(-3, -20, 11);
			usleep( 50 * 1000 );
		}

		//get ball location to check if it changed
		lastBallLocation =  m_Localization->GetBallLocation();

		//sleep for 50ms
		usleep( 50 * 1000 );
		#ifdef DEBUG_CORRECTION_NEEDED
			counter++;
		#endif
	}
	m_Motion->SetWalkingMode(RUNNING);
	m_CurrentState = STATE_DECISION_MAKING;
	m_Vision->SetHeadTrackingStatus(BALL_TRACKING);
	return;
#ifdef DEBUG_THIS_ONE
	m_Motion->SetWalkingMode(RUNNING);
	m_Vision->SetHeadTrackingStatus(KICKING_BALL_TRACKING);
	counter = 0;
	ballLocation = m_Localization->GetBallLocation();
	while ( (counter < 20) && !IsBallDataRelevant(&ballLocation) && (m_ThreadStartStop == START) )
	{
		counter++;
		ballLocation = m_Localization->GetBallLocation();
		usleep( 100 * 1000 );
	}
	m_isKeepMeAlive = false;
	#ifdef DEBUG_GO_TO_STATE_RAIN_DANCE
		cout << "[GoTo::StateRainDance]: m_isKeepMeAlive: " << m_isKeepMeAlive << "\n";
	#endif
	ballLocation = m_Localization->GetBallLocation();
	if (ballLocation.distanceFromMe < DISTANCE_OUT_OF_RAIN_DANCE)
	{
		m_CurrentState = STATE_GO_KICK;
	}
	else
	{
		m_CurrentState = STATE_DECISION_MAKING;
	}
#endif
}


/*****************************************************************************************
 * Method Name:   QuadraticEquation
 * Description:   this method solves quadratic equation
 * Arguments:	  aParam, bParam, cParam
 * Return:		  x1, x2
 *****************************************************************************************/
void GoTo::QuadraticEquation(double aParam, double bParam, double cParam, double *x1, double *x2)
{
	double delta;

	//calculate the delta
	delta = sqrt( ( bParam * bParam ) - 4 * aParam * cParam );

	//calculate x1, x2
	*x1 = ( -bParam - delta ) / ( 2 * aParam );
	*x2 = ( -bParam + delta ) / ( 2 * aParam );
}

/*****************************************************************************************
 * Method Name:   CalculateNPoint
 * Description:   this method calculates the point located N cm from the ball on line G
 * Arguments:	  xBall, yBall, radius - cm from ball,
 * Return:		  xPoint, yPoint
 *****************************************************************************************/
void GoTo::CalculateNPoint(double xBall, double yBall, double radius, double *xPoint, double *yPoint)
{
	double aParam, bParam, cParam;
	double x1, x2, y1, y2;

	if( xBall == 0)
	{
		*yPoint = yBall - radius;
		*xPoint = 0;
	}
	else
		{
		//calculate aParam
		aParam = 1 + ( ( 600 - yBall ) / xBall ) * ( ( 600 - yBall ) / xBall );

		//calculate bParam
		bParam = -2 * xBall - ( 1200 * ( ( 600 - yBall ) / xBall ) ) + ( 2 * yBall * ( ( 600 - yBall ) / xBall ) );

		//calculate cParam
		cParam = ( xBall * xBall ) + 360000 + ( yBall * yBall ) - ( 1200 * yBall ) - ( radius * radius );

		QuadraticEquation(aParam, bParam, cParam, &x1, &x2);

		//calculate y1 and y2
		y1 = -x1 * ( ( 600 - yBall ) / xBall ) + 600;
		y2 = -x2 * ( ( 600 - yBall ) / xBall ) + 600;

		//select the relevant point
		if( y1 < yBall )
		{
			*yPoint = y1;
			*xPoint = x1;
		}
		else
		{
			*yPoint = y2;
			*xPoint = x2;
		}
	}
}

/*void GoTo::Check()
{
	m_Motion->StartWalkingAsync(10,0,0);
}
*/



/*****************************************************************************************
 * Method Name:   SetPlayerMode
 * Description:   This function sets player mode
 * Arguments:	  player mode,
 *****************************************************************************************/
void GoTo::SetPlayerMode(PlayerMode_t playerMD)
{
	m_PlayerMode = playerMD;

}


/*****************************************************************************************
 * Method Name:   IsPossibleToKick
 * Description:   this return true if in kickable position
 * Arguments:	  Location,
 * Return:		  xPoint, yPoint
 *****************************************************************************************/
bool GoTo::IsPossibleToKick(Location myLocation)
{
	double bParam;
	double aParam;
	double angle;

	if( m_PlayerMode == GOALIE_MODE )
	{
		if( fabs(myLocation.angle) < GOALIE_ANGLE_FOR_RAIN_DANCE )
		{
			return true;
		}
		return false;
	}

	angle = 90 - fabs( (double)myLocation.angle );

	#ifdef DEBUG_IS_IT_POSSIBLE_TO_KICK
		cout << "[GoTo::StateIsItPossibleToKick]: my location x, y #" << myLocation.x << " " << myLocation.y << " Angle = " << myLocation.angle << "\n";
	#endif


	//if the angle is over 90, return false
	if ( fabs( (double)myLocation.angle ) > 90 )
	{
		return false;
	}
	//if the angle is 90, the slop is inf so remove epsion from the angle
	else if ( angle == 90 )
	{
		angle = 90 - EPSILON;
	}

	//calculate the a and b params
	aParam = SIGN( (double)myLocation.angle ) * tan(  (angle * PI) / 180  );
	bParam = ( -aParam ) * myLocation.x + myLocation.y;

	//if the x,y is over the POLE OFFSET retrun false
	if ( fabs( ( Y_POLE - bParam ) / aParam ) > GET_OFFSET(myLocation.y) )
	{
		#ifdef DEBUG_IS_IT_POSSIBLE_TO_KICK
				//cout << "[GoTo::IsPossibleToKick]: false 	if ( fabs( ( Y_POLE - bParam ) / aParam ) > OFFSET_FROM_POLE )\n";
		#endif

		return false;
	}

	#ifdef DEBUG_IS_IT_POSSIBLE_TO_KICK
			//cout << "[GoTo::IsPossibleToKick]: true\n";
	#endif

	#ifdef DEBUG_IS_IT_POSSIBLE_TO_KICK
		cout << "[GoTo::StateGoNcmFromBall]: my location x, y #" << myLocation.x << " " << myLocation.y << " Angle = " << myLocation.angle << "\n";
		cout << "aparan " << aParam << "bparam " << bParam << "\n";
	#endif
	//return true
	return true;
}


bool GoTo::QuickAndDirty(int x,int y,int a,bool dontGoInCircles)
{
	//default value for a == -1000 which means use your own angle
	static QuickAndDirtyState state = FIRST_TIME;
	static int staticX,staticY,staticA;
	static HeadTrackingStatus_e headTrackingMode;

	if ( staticX != x || staticY != y || staticA != a)
	{
		staticX = x;
		staticY = y;
		staticA = a;
		state = FIRST_TIME;
	}

	//handle localization fail!
	if (m_myLocPtr->y > 700 || m_myLocPtr->y < -100 || fabs(m_myLocPtr->x) > 300)
	{
		if (state != OUT_OF_FIELD)
		{
			headTrackingMode=m_Vision->GetHeadTrackingStatus();
			m_Vision->SetHeadTrackingStatus(GOAL_TRACKING);
			m_Vision->ResetSeeGoalFrames();
			#ifdef QandD_DEBUG
				cout << "Out of field first time \n\n";
			#endif
		}
		state = OUT_OF_FIELD;
		if (m_Vision->DoISeeTheGoal())
		{
			state = FIRST_TIME;
			m_Vision->SetHeadTrackingStatus(headTrackingMode);
			#ifdef QandD_DEBUG
				cout << "Out of field finished \n\n";
			#endif
		}
	}
	else if (state==OUT_OF_FIELD) //i'm not really out of field
	{
		state=FIRST_TIME;
	}

	int distance = sqrt((x - m_myLocPtr->x)*(x - m_myLocPtr->x) + (y - m_myLocPtr->y)*(y - m_myLocPtr->y));
	int fowardSpeed = 0 ,sideSpeed = 0 ,angleSpeed = 0;
	Angle angleDiff = 0;
	if (a == -1000)
	{
		a = m_myLocPtr->angle;
	}
	angleDiff = a - int(m_myLocPtr->angle);
	if (abs(int(angleDiff)) < 20 && abs(x - m_myLocPtr->x) < 15 && abs(y - m_myLocPtr->y) < 15 )
	{
		m_Motion->StopWalking();
		return true;
	}

	Angle targetAngle;


	targetAngle = RAD2DEG * atan2( (x - m_myLocPtr->x ) + EPSILON ,y - m_myLocPtr->y);
	Angle toRurnTo=targetAngle - m_myLocPtr->angle;

	if (state == FIRST_TIME || state == FAR_AND_TURN || state == FAR_NO_TURN)
	{
		if (distance > 70)
		{
			if (fabs(toRurnTo) > 50)
			{
				state = FAR_AND_TURN;
			}
			else
			{
				state = FAR_NO_TURN;
			}
		}
		else //distance <= 70
		{
			state = CLOSE;
		}
	}
	else if (state == CLOSE)
	{
		if (distance > 200)
			state = FIRST_TIME;
	}

	if (dontGoInCircles && state!=OUT_OF_FIELD)
	{
		state = CLOSE;
	}

	if (state == FAR_NO_TURN)
	{ //far with small angle
		fowardSpeed = cos(DEG2RAD*(targetAngle-m_myLocPtr->angle)) * RUNNING_MAX_FORWARD_SPEED;
	}
	else if (state == FAR_AND_TURN)
	{ //far with big angle
		fowardSpeed = 12;
	}
	else if (state == CLOSE)
	{ //close
		if (fabs(cos(DEG2RAD * (targetAngle-m_myLocPtr->angle)))>0.1)
			fowardSpeed = (cos(DEG2RAD*(targetAngle-m_myLocPtr->angle))) * MIN(23,fabs(cos(DEG2RAD*(targetAngle-m_myLocPtr->angle))*distance));
		if (fabs(sin(DEG2RAD * (targetAngle-m_myLocPtr->angle)))>0.1)
			sideSpeed = (sin(DEG2RAD*(targetAngle-m_myLocPtr->angle))) * MIN(23,fabs(sin(DEG2RAD*(targetAngle-m_myLocPtr->angle))*distance));

		toRurnTo = (a - int(m_myLocPtr->angle)) / 2;
	}
	else if (state == OUT_OF_FIELD)
	{ //localization failed!
		toRurnTo = 20;
	}

	angleSpeed = toRurnTo;

	#ifdef QandD_DEBUG
		printf("me: [%d,%d,%d]  \ttarget: [%d,%d,%d]\twalk: %d*%d*%d\tdistance: %d\ttargetA: %d\ttoTurn: %d\tstate=%d\n"
				,int(m_myLocPtr->x),int(m_myLocPtr->y),int(m_myLocPtr->angle)
				,x,y,a
				,fowardSpeed,sideSpeed,angleSpeed,distance,int(targetAngle)
				,int(toRurnTo)
				,state
				);
	#endif


	fowardSpeed = MAX(fowardSpeed, -13);
	angleSpeed = MIN(MAX(angleSpeed, -10),10);
	sideSpeed = 0;
	#ifdef DEBUG_QUICK_AND_DIRTY
		cout << "fowardSpeed = " << fowardSpeed << ". sideSpeed = " << sideSpeed << ". angleSpeed = " << angleSpeed << "\n";
	#endif
	m_Motion->StartWalkingAsync(fowardSpeed, -sideSpeed, -angleSpeed);

	return false;
}




/*****************************************************************************************
 * Method Name:   StateGoToLineL
 * Description:   this state checks the x y of the Robot and the projection of the robot
 * 				  X Y to the G and L lines and decide where to go
 *****************************************************************************************/
//void GoTo::StateGoToLineL()
//{
//	BallLocation ballLocation, lastBallLocation;
//	Location     myLocation;
//	double		 Yg, Xg;
//	double		 Yl,Xl;
//	double		 Ag,Bg;
//	double		 Al,Bl;
//	double 		 Dl,Dg;
//	double       angleFromXY;
//	double		 xSpeed;
//	//double		 turnSpeed;
//
//	lastBallLocation = m_Localization->GetBallLocation();
//
//	while( ( m_CurrentState == STATE_GO_TO_LINE_L ) && ( m_ThreadStartStop == START) )
//	{
//		//get location and ball location
//		myLocation = m_Localization->GetMyLocation();
//		ballLocation = m_Localization->GetBallLocation();
//
//		if( IsBallDataRelevant(&ballLocation) )
//		{
//			ballLocation = lastBallLocation;
//		}
//
//		//calculate all needed params for making a decision
//		CalculateABParametersOFOppGoalBallEquation(&Ag, &Bg, ballLocation.x, ballLocation.y);
//		CalculateABParametersOFBallOurGoalEquation(&Al, &Bl, ballLocation.x, ballLocation.y);
//		CalculateXYProjectionOnALine(Ag, Bg, myLocation.x, myLocation.y, &Xg, &Yg);
//		CalculateXYProjectionOnALine(Al, Bl, myLocation.x, myLocation.y, &Xl, &Yl);
//		Dg = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y,Xg,Yg);
//		Dl = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y,Xl,Yl);
//#ifdef DEBUG_GO_TO
//		cout << "[GoTo::StateGoToLineL]: G line A, B #" << Ag << " " << Bg << "\n";
//		cout << "[GoTo::StateGoToLineL]: L line A, B #" << Al << " " << Bl << "\n";
//		cout << "[GoTo::StateGoToLineL]: ball location x, y #" << ballLocation.x << " " << ballLocation.y << "\n";
//		cout << "[GoTo::StateGoToLineL]: my location x, y #" << myLocation.x << " " << myLocation.y << "\n";
//		cout << "[GoTo::StateGoToLineL]: Yg Yl #" << Yg << " " << Yl << "\n";
//		cout << "[GoTo::StateGoToLineL]:                                       Distance: " << Dl << "\n";
//#endif
//		//if ball noise is greater than reasonable noise, go back to decision making state
//		if( CalculateDistanceBetweenTwoPoints(ballLocation.x, ballLocation.y, lastBallLocation.x, lastBallLocation.y) > REASONABLE_NOISE_DISTANCE )
//		{
//			//start walking on idle
//			m_Motion->StartWalkingAsync(0, 0, 0);
//
//			m_CurrentState = STATE_DECISION_MAKING;
//			return;
//		}
//
//		else
//		{
//
//			//if the distance between the robot and the line is under 10 cm,
//			//turn towards the ball
//			if( Dl < START_TURNING_DISTANCE )
//			{
//				//get my relative angle from me and x,y point vector
//				angleFromXY = (double)(ballLocation.angleFromMe); //CalculateAngleFromXY(myLocation.x, myLocation.y, ballLocation.x, ballLocation.y, (double)myLocation.angle);
//
//				//if the angle to the ball is under 10 go to ball state
//				if( fabs(angleFromXY) < STOP_TURNING_ANGLE )
//				{
//					m_CurrentState = STATE_ON_LINE_TO_BALL;
//
//					return;
//				}
//
//				//turn to ball
//				else
//				{
//					//get turn speed from angle
//					//turnSpeed = -GetTurnSpeed(angleFromXY);
//
//#ifdef DEBUG_GO_TO
//
//		cout << "[GoTo::StateGoToLineL]: Turning to ball angle from ball is "  << angleFromXY <<"\n\n";
//#endif
//
//					//start walking with parameters
//					//m_Motion->StartWalkingAsync(0,0,turnSpeed);
//				}
//			}
//
//			// if the distance to line l is bigger then the distance to line g
//			// or line g y projection is over Yb and Yl is lower than balllocation y go to line l
//			else if(
//					( ( Dl < Dg ) || ( Yg >= ballLocation.y ) ) &&
//					( Yl < ballLocation.y )
//			  )
//			{
//				//get my relative angle from me and x,y point vector
//				angleFromXY = CalculateAngleFromXY(myLocation.x, myLocation.y, Xl, Yl, (double)myLocation.angle);
//
//
//				//get x speed
//				xSpeed = GetXSpeed(Dl, angleFromXY, true);
//
//				//get turn speed from angle
//			//	turnSpeed = GetTurnSpeed(angleFromXY);
//
//#ifdef DEBUG_GO_TO
//		cout << "[GoTo::StateGoToLineL]: Robots angle:" << (double)myLocation.angle  << "\n";
//		cout << "[GoTo::StateGoToLineL]: Turning to point on line l with angle:" << angleFromXY << "\n";
////		cout << "[GoTo::StateGoToLineL]: speeds x: " <<  xSpeed <<  " angle: " << turnSpeed << "\n\n";
////		m_Motion->StopWalking();
////		m_Motion->StartAction(SIT_DOWN);
////		exit(1);
//
//#endif
//					//start walking with parameters
//		//			m_Motion->StartWalkingAsync(xSpeed,0,turnSpeed);
//
//			}
//			else
//			{
//				//start walking with parameters
//				m_Motion->StartWalkingAsync(0,0,0);
//
//				//change state to decision making
//				m_CurrentState = STATE_DECISION_MAKING;
//
//				return;
//			}
//
//		}
//
//		lastBallLocation = ballLocation;
//		//wait for 50 ms
//		usleep( 50 * 1000 );
//	}
//
//	cout << "[GoTo::StateGoToLineL]: Leaving" << "\n";
//}


/*****************************************************************************************
 * Method Name:   StateGoToLineG
 * Description:   this state checks the x y of the Robot and the projection of the robot
 * 				  X Y to the G and L lines and decide where to go
 *****************************************************************************************/
//void GoTo::StateGoToLineG()
//{
//	BallLocation ballLocation, lastBallLocation;
//	Location     myLocation;
//	double		 Yg, Xg;
//	double		 Yl, Xl;
//	double		 Ag,Bg;
//	double		 Al,Bl;
//	double 		 Dl,Dg;
//	double       angleFromXY;
//	double		 xSpeed;
//	//double		 turnSpeed;
//
//	lastBallLocation = m_Localization->GetBallLocation();
//
//	while( ( m_CurrentState == STATE_GO_TO_LINE_G ) && ( m_ThreadStartStop == START ) )
//	{
//		//get location and ball location
//		myLocation = m_Localization->GetMyLocation();
//		ballLocation = m_Localization->GetBallLocation();
//
//		if( IsBallDataRelevant(&ballLocation) )
//		{
//			ballLocation = lastBallLocation;
//		}
//
//		//calculate all needed params for making a decision
//		CalculateABParametersOFOppGoalBallEquation(&Ag, &Bg, ballLocation.x, ballLocation.y);
//		CalculateABParametersOFBallOurGoalEquation(&Al, &Bl, ballLocation.x, ballLocation.y);
//		CalculateXYProjectionOnALine(Ag, Bg, myLocation.x, myLocation.y, &Xg, &Yg);
//		CalculateXYProjectionOnALine(Al, Bl, myLocation.x, myLocation.y, &Xl, &Yl);
//		Dg = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y,Xg,Yg);
//		Dl = CalculateDistanceBetweenTwoPoints(myLocation.x, myLocation.y,Xl,Yl);
//
//#ifdef DEBUG_GO_TO
//		cout << "[GoTo::StateGoToLineG]: G line A, B #" << Ag << " " << Bg << "\n";
//		cout << "[GoTo::StateGoToLineG]: L line A, B #" << Al << " " << Bl << "\n";
//		cout << "[GoTo::StateGoToLineG]: ball location x, y #" << ballLocation.x << " " << ballLocation.y << "\n";
//		cout << "[GoTo::StateGoToLineG]: my location x, y #" << myLocation.x << " " << myLocation.y << "\n";
//		cout << "[GoTo::StateGoToLineG]: Yg Yl #" << Yg << " " << Yl << "\n";
//#endif
//
//		//if ball noise is greater than reasonable noise, go back to decision making state or the angle
//		//from ball is greater then 80
//
//		if (CalculateDistanceBetweenTwoPoints(ballLocation.x, ballLocation.y, lastBallLocation.x, lastBallLocation.y) > REASONABLE_NOISE_DISTANCE)
//		{
//			//start walking on idle
//			m_Motion->StartWalkingAsync(0, 0, 0);
//
//			m_CurrentState = STATE_DECISION_MAKING;
//			return;
//		}
//
//
//		else
//		{
//
//			//if the distance between the robot and the line is under 10 cm,
//			//turn towards the ball
//			if( Dg < START_TURNING_DISTANCE )
//			{
//				//get my relative angle from me and x,y point vector
//				angleFromXY = (double)(ballLocation.angleFromMe); //CalculateAngleFromXY(myLocation.x, myLocation.y, ballLocation.x, ballLocation.y, (double)myLocation.angle);
//
//				//if the angle to the ball is under 10 go to ball state
//				if( fabs(angleFromXY) < STOP_TURNING_ANGLE )
//				{
//					m_CurrentState = STATE_ON_GOAL_TO_BALL;
//
//					return;
//				}
//
//				//turn to ball
//				else
//				{
//					//get turn speed from angle
//					//turnSpeed = -GetTurnSpeed(angleFromXY);
//
//					//start walking with parameters
//					//m_Motion->StartWalkingAsync(0,0,turnSpeed);
//				}
//			}
//
//			// if the distance to line g is bigger then the distance to line L
//			// or line l y projection is over Yb and Yg is lower than balllocation y go to line g
//			else if(
//					( ( Dg <= Dl ) || ( Yl >= ballLocation.y ) ) &&
//					( Yg < ballLocation.y )
//			  )
//			{
//				//get my relative angle from me and x,y point vector
//				angleFromXY = CalculateAngleFromXY(myLocation.x, myLocation.y, Xg, Yg, (double)myLocation.angle);
//
//				//get x speed
//				xSpeed = GetXSpeed(Dg, angleFromXY, true);
//
//				//get turn speed from angle
//			//	turnSpeed = GetTurnSpeed(angleFromXY);
//
//				//start walking with parameters
////				m_Motion->StartWalkingAsync(xSpeed,0,turnSpeed);
//			}
//			else
//			{
//				//start walking with parameters
//				m_Motion->StartWalkingAsync(0,0,0);
//
//				//change state to decision making
//				m_CurrentState = STATE_DECISION_MAKING;
//
//				return;
//			}
//		}
//
//		lastBallLocation = ballLocation;
//
//		//wait for 50 ms
//		usleep( 50 * 1000 );
//	}
//
//	cout << "[GoTo::StateGoToLineG]: Leaving" << "\n";
//}
