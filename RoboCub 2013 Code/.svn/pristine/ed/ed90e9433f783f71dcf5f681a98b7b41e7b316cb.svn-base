
#include "Striker.h"
#include "MyRobot.h"

//#define DEBUG_STRIKER_ON
//#define DEBUG_STRIKER_STATE_MACHINE
//#define DEBUG_STRIKER_LOOK_FOR_THE_BALL
//#define DEBUG_STRIKER_LOOK_FOR_THE_BALL_EXTENDED
//#define DEBUG_STRIKER_GO_TO_THE_BALL
//#define DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
//#define DEBUG_STRIKER_SHOULD_I_GO_TO_BALL
//#define DEBUG_STRIKER_POSIION_YOURSELF

Striker* Striker::m_uniqueInstance = 0;

Striker::Striker()
{
	cout << "Striker::Striker()\n";
	m_otherStrikerId = 1;
	m_goalieId = 2;
	m_didIGoLastTime = false;
	m_isLastCommunicationOn = false;
}

Striker::~Striker()
{
	cout << "Striker::~Striker()\n";
}

Striker* Striker::GetInstance()
{
	if (m_uniqueInstance == 0)
	{
		m_uniqueInstance = new Striker();
	}
	return m_uniqueInstance;
}

State_t Striker::DoStateGameControllerReady()
{
	XYLocation_s positionLocation;

	if (MyRobot::GetPlayerID() == ID_STRIKER1)
	{
		if (m_communication->GetkickOffTeam() == m_communication->teamId)
		{
			positionLocation.x = OFFENSIVE_POSITION_STRIKER1_X;
			positionLocation.y = OFFENSIVE_POSITION_STRIKER1_Y;
		}
		else
		{
			positionLocation.x = DEFENSIVE_POSITION_STRIKER1_X;
			positionLocation.y = DEFENSIVE_POSITION_STRIKER1_Y;
		}
	}
	else if (MyRobot::GetPlayerID() == ID_STRIKER2)
	{
		if (m_communication->GetkickOffTeam() == m_communication->teamId)
		{
			positionLocation.x = OFFENSIVE_POSITION_STRIKER2_X;
			positionLocation.y = OFFENSIVE_POSITION_STRIKER2_Y;
		}
		else
		{
			positionLocation.x = DEFENSIVE_POSITION_STRIKER2_X;
			positionLocation.y = DEFENSIVE_POSITION_STRIKER2_Y;
		}
	}
	else
	{
		throw "UNKNOWN_ROBOT_ID";
	}

	m_vision->SetHeadTrackingStatus(GOAL_TRACKING);
	m_goTo->Stop();

	while(exitProgram == false)
	{
		m_goTo->QuickAndDirty(positionLocation.x,positionLocation.y ,0);

		if (!IsStateReady())
		{
			m_vision->SetHeadTrackingStatus(BALL_TRACKING);
			m_goTo->Stop();
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		usleep(100 *1000);
	}
	return STATE_HANDLE_GAME_CONTROLLER_STATES; // TODO Fix this. need to be in a while.
												// TODO add ready points with an id for each striker, instead of these calculations
}

State_t Striker::RunState(State_t cur_state)
{
#ifdef DEBUG_STRIKER_STATE_MACHINE
	cout << "[Striker]:current state [" << PrintState(cur_state) << "] time: "
			<< GetCurrentTime() << "\n";
#endif

	switch (cur_state)
	{
	case STATE_INITIALIZE:
		return DoStateInitialize();
	case STATE_LOOK_FOR_THE_BALL:
		return DoStateLookForTheBall();
	case STATE_GO_TO_THE_BALL:
		return DoStateGoToTheBall();
	case STATE_SHOULD_I_GO_TO_THE_BALL:
		return DoStateShouldIGoToTheBall();
	case STATE_POSITION_YOURSELF:
		return DoStatePositionYourself();
	case STATE_IS_IT_POSSIBLE_TO_KICK:
		return DoStateIsItPossibleToKick();
	case STATE_KICK:
		return DoStateKick();
	case STATE_DRIBBLE:
		//return DoStateDribble();
		return DoStateKick();
	case STATE_PASS_AWAY:
		return DoStatePassAway();
	case STATE_HANDLE_GAME_CONTROLLER_STATES:
		return DoStateHandleGameControllerStates();
	case STATE_GAME_CONTROLLER_INITIAL:
		return DoStateGameControllerInitial();
	case STATE_GAME_CONTROLLER_SET:
		return DoStateGameControllerSet();
	case STATE_GAME_CONTROLLER_READY:
		return DoStateGameControllerReady();
	case STATE_GAME_CONTROLLER_PLAYING:
		return DoStateGameControllerPlaying();
	case STATE_GAME_CONTROLLER_FINISHED:
		return DoStateGameControllerFinished();
	default:
		throw "[Striker::RunState]: Unknown state\n";
	}
}

State_t Striker::DoStateInitialize()
{
	int counter = 0;
	BallLocation ballLocation, lastBallLocation;

	int numOfBallsForAvgDistance = 0;
	double sumOfDistancesFromBall = 0;
	double averageDistanceFromBall;

	m_motion->StartAction(STAND_UP);
	m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	ballLocation = m_localization->GetBallLocation();

	//cout << "m_communication->GetkickOffTeam() = " << (int)m_communication->GetkickOffTeam() << "\n";

	if ((m_communication->GetkickOffTeam() == m_communication->teamId))	// we are the kickoff team
	{
		return STATE_LOOK_FOR_THE_BALL;
	}

	while ((!IsBallDataRelevant(&ballLocation)) && (counter < MAX_TIME_TO_WAIT_KICKOFF * 20))
	{
		if(!IsStatePlaying())
		{
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		ballLocation = m_localization->GetBallLocation();
		usleep(50 * 1000);
		counter++;
	}

	while (counter < MAX_TIME_TO_WAIT_KICKOFF * 20)
	{
		if(!IsStatePlaying())
		{
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		lastBallLocation = ballLocation;
		ballLocation = m_localization->GetBallLocation();

		if (ballLocation.time != lastBallLocation.time)
		{
			sumOfDistancesFromBall += ballLocation.distanceFromMe;
			numOfBallsForAvgDistance++;
			averageDistanceFromBall = sumOfDistancesFromBall/numOfBallsForAvgDistance;
		}

		if (numOfBallsForAvgDistance > MIN_NUM_OF_BALLS_FOR_STATIC_BALL_DECISION_STATE_INITIALIZE)
		{
			if (fabs(averageDistanceFromBall - ballLocation.distanceFromMe) > MAX_DIFF_TO_START_PLAY_STATE_INITIALIZE)
			{
				break;
			}
		}

		usleep(50 * 1000);
		counter++;
	}

	return STATE_LOOK_FOR_THE_BALL;
}

State_t Striker::DoStateLookForTheBall()
{
	Location myLocation;
	BallLocation myBallLocation;
	BallLocation otherStrikerBallLocation;
	myLocation = m_localization->GetMyLocation();
	myBallLocation = m_localization->GetBallLocation();
	otherStrikerBallLocation = m_communication->GetFriendData(m_otherStrikerId)->ballLocation;

	TimeStamp currentTime;
	long timePassed;
	int iterationTimeForGoToBallWithoutSeeingIt = 500 * 1000;
	Angle angleFromMe;
	bool isFirstTime = true, isFullTurnCompleted = false;
	bool isHalfTurnCompleted = false;
	Angle firstLookingAngle;
	XYLocation_s xyLocation1;
	XYLocation_s xyLocation2;
	bool isXYLocation1 = true;

	if(MyRobot::GetPlayerID() == ID_STRIKER1)
	{
		xyLocation1.x = LOOK_FOR_BALL_POSITION1_STRIKER1_X;
		xyLocation1.y = LOOK_FOR_BALL_POSITION1_STRIKER1_Y;
		xyLocation2.x = LOOK_FOR_BALL_POSITION2_STRIKER1_X;
		xyLocation2.y = LOOK_FOR_BALL_POSITION2_STRIKER1_Y;
	}
	else
	{
		xyLocation1.x = LOOK_FOR_BALL_POSITION1_STRIKER2_X;
		xyLocation1.y = LOOK_FOR_BALL_POSITION1_STRIKER2_Y;
		xyLocation2.x = LOOK_FOR_BALL_POSITION2_STRIKER2_X;
		xyLocation2.y = LOOK_FOR_BALL_POSITION2_STRIKER2_Y;
	}

	while (exitProgram == false && IsBallDataRelevant(&myBallLocation) == false && IsBallDataRelevant(&otherStrikerBallLocation) == false)
	{
		/*#ifdef #ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL_EXTENDED
				cout << "[Striker::DoStateLookForTheBall]: Looking for the ball...\n";
		#endif*/
		if (!IsStatePlaying())
		{
			if(m_goTo->GetThreadStatus() == STARTED)
			{
				m_goTo->Stop();
			}
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		m_ballLocationFromFriends = CalculateBallLocationFromFriends();

		if (IsCommunicationOn() && m_ballLocationFromFriends.isRelevant)
		{
			#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL
				cout << "[Striker::DoStateLookForTheBall]: Get the ball location from friends.\n";
			#endif

			if(m_goTo->GetThreadStatus() == STARTED)
			{
				m_goTo->Stop();
			}
			timePassed = 0;

			// Go to ball location (without seeing it).
			while (exitProgram == false && IsCommunicationOn() && m_ballLocationFromFriends.isRelevant && !IsBallDataRelevant(&otherStrikerBallLocation))
			{
				if (!IsStatePlaying())
				{
					return STATE_HANDLE_GAME_CONTROLLER_STATES;
				}
				m_ballLocationFromFriends = CalculateBallLocationFromFriends();
				#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL
					cout << "[Striker::DoStateLookForTheBall]: Going to ["
						 << m_ballLocationFromFriends.x << " , " << m_ballLocationFromFriends.y << "].\n";
				#endif
				m_goTo->StartGoToXY(m_ballLocationFromFriends);

				if (IsBallDataRelevant(&myBallLocation) || timePassed >= MAX_TIME_GO_TOWARDS_BALL || m_goTo->IsReached())
				{
					break;
				}

				usleep(iterationTimeForGoToBallWithoutSeeingIt);
				timePassed += iterationTimeForGoToBallWithoutSeeingIt;
				myBallLocation = m_localization->GetBallLocation();
				myLocation = m_localization->GetMyLocation();
				otherStrikerBallLocation = m_communication->GetFriendData(m_otherStrikerId)->ballLocation;
			}
		}
		// Had no idea where the ball is - start turning.
		else
		{
			if (isFirstTime == true)
			{
				isFirstTime = false;
				firstLookingAngle = myLocation.angle;
				#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL
					cout << "[Striker::DoStateLookForTheBall]: first angle = " << firstLookingAngle << ".\n";
				#endif
			}
			angleFromMe = m_localization->GetBallLocation().angleFromMe;
			if(!isFullTurnCompleted)
			{
				if (angleFromMe < 0 )
				{
/*					#ifdef #ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL_EXTENDED
						cout << "[Striker::DoStateLookForTheBall]: Can't find the ball - Last location was ["
						<< angleFromMe << "]\n" << "Start turning left. "
						<< "current angle: " << myLocation.angle <<"\n";
					#endif
*/					m_motion->StartWalkingAsync(0, 0, LOOK_FOR_BALL_TURNNING_ANGLE_SPEED);
				}
				else
				{
					#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL
						cout << "[Striker::DoStateLookForTheBall]: Can't find the ball - Last location was ["
						<< angleFromMe << "]\n" << "Start turning left. "
						<< "current angle: " << myLocation.angle <<"\n";
					#endif
					m_motion->StartWalkingAsync(0, 0, -LOOK_FOR_BALL_TURNNING_ANGLE_SPEED);
				}
				if( (
					 ( (firstLookingAngle + ((Angle)180.0 + ANGLE_MISTAKE) > myLocation.angle &&
					 firstLookingAngle + ((Angle)180.0 - ANGLE_MISTAKE) < myLocation.angle) )
					 ||
					 (firstLookingAngle + ((Angle)180.0 + ANGLE_MISTAKE) < myLocation.angle &&
					 firstLookingAngle + ((Angle)180.0 - ANGLE_MISTAKE) > myLocation.angle) )
					 &&
					 (!isHalfTurnCompleted))
				{
					#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL
						cout << "[Striker::DoStateLookForTheBall]: half turn has been completed.\n";
						cout << "[Striker::DoStateLookForTheBall]: current angle: " << myLocation.angle
							 << ". target angle: " << firstLookingAngle << ".\n";
					#endif
					isHalfTurnCompleted = true;
				}
				if ( (isHalfTurnCompleted) && fabs(myLocation.angle - firstLookingAngle) < ANGLE_MISTAKE)
				{
					#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL
						cout << "[Striker::DoStateLookForTheBall]: full turn has been completed.\n";
						cout << "[Striker::DoStateLookForTheBall]: current angle: " << myLocation.angle
							 << ". target angle: " << firstLookingAngle << ".\n";
					#endif
					isFullTurnCompleted = true;
					isHalfTurnCompleted = false;
				}
			}
			// A full turn has been completed.
			else
			{
				if(m_goTo->GetThreadStatus() == IDLE)
				{
					if(m_goTo->IsReached() == false)
					{
						if(isXYLocation1)
						{
							#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL_EXTENDED
								cout << "[Striker::DoStateLookForTheBall]: Can't find the ball - going to point [" <<
										xyLocation1.x << " , " << xyLocation1.y << "].  my location" << myLocation.x<< "    "<<myLocation.y <<"\n";
							#endif
							m_goTo->StartGoToXY(xyLocation1);
						}
						else
						{
							#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL_EXTENDED
								cout << "[Striker::DoStateLookForTheBall]: Can't find the ball - going to point [" <<
										xyLocation2.x << " , " << xyLocation2.y <<"].  my location" << myLocation.x<< "    "<<myLocation.y << "\n";
							#endif
							m_goTo->StartGoToXY(xyLocation2);
						}
					}
					else
					{
						#ifdef DEBUG_STRIKER_LOOK_FOR_THE_BALL_EXTENDED
						cout <<" reached to point " << (isXYLocation1 ? "1" : "2") << " \n";
						#endif
						isFullTurnCompleted = false;
						isXYLocation1 = !isXYLocation1;
						isFirstTime = true;
						m_goTo->ResetIsReached();
					}
				}
			}
		}
		usleep(150 * 1000);
		myLocation = m_localization->GetMyLocation();
		myBallLocation = m_localization->GetBallLocation();
		otherStrikerBallLocation = m_communication->GetFriendData(m_otherStrikerId)->ballLocation;
	}
	// The ball has been found.
	if(m_goTo->GetThreadStatus() == STARTED)
	{
		m_goTo->Stop();
	}
	return STATE_SHOULD_I_GO_TO_THE_BALL;
}

XYLocation_t Striker::CalculateBallLocationFromFriends()
{
	AllLocationData_t* friendStrikerData;
	AllLocationData_t* friendGoalieData;

	BallLocation friendStrikerBallLocation;
	BallLocation friendGoalieBallLocation;

	XYLocation_t ballLocationAbs;

	long strikerTimeDiff;
	long goalieTimeDiff;

	double strikerWeight, goalieWeight;
	double weightedX, weightedY;

	#ifdef DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
		//cout << "Striker::CalculateBallLocationFromFriends Start\n";
	#endif

	if (IsCommunicationOn())
	{
		#ifdef DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
			cout << "Striker::CalculateBallLocationFromFriends IsCommunicationOn = true\n";
		#endif

		friendStrikerData	= m_communication->GetFriendData(m_otherStrikerId);
		friendGoalieData	= m_communication->GetFriendData(m_goalieId);

		friendStrikerBallLocation 	= friendStrikerData->ballLocation;
		friendGoalieBallLocation 	= friendGoalieData->ballLocation;

		#ifdef DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
			cout << "Striker::CalculateBallLocationFromFriends Got friend data\n";
		#endif

		strikerTimeDiff = friendStrikerData->ballDiff;
		goalieTimeDiff 	= friendGoalieData->ballDiff;

		// checks whether friend ball location is relevant or not
		// (taking into consideration the time that passed and the ball distance from the friend).
		bool isStrikerBallLocationRelevant 	= (strikerTimeDiff < MAX_VALID_TIME_BALL_LOCATION_FROM_FRIEND	&&
											  friendStrikerBallLocation.distanceFromMe < MAX_VALID_DISTANCE_BALL_LOCATION_FROM_FRIEND &&
											  strikerTimeDiff > 0);
		bool isGoalieBallLocationRelevant 	= (goalieTimeDiff < MAX_VALID_TIME_BALL_LOCATION_FROM_FRIEND	&&
											  friendGoalieBallLocation.distanceFromMe < MAX_VALID_DISTANCE_BALL_LOCATION_FROM_FRIEND &&
											  goalieTimeDiff > 0);

		ballLocationAbs.isRelevant = isStrikerBallLocationRelevant || isGoalieBallLocationRelevant;

		if (ballLocationAbs.isRelevant == false)
		{
			ballLocationAbs.isRelevant = false;
			#ifdef DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
				cout << "Striker::CalculateBallLocationFromFriends: Data from friends is not relevant\n";
				cout << "StrikerTimeDiff = " << strikerTimeDiff << " GoalieTimeDiff = " << goalieTimeDiff << "\n";
			#endif
		}
		else // ball is relevant from both friends
		{
			#ifdef DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
				cout << "Striker::CalculateBallLocationFromFriends: have both goalie and striker ball pos\n";
				cout << "Goalie's ball position : x = "  << friendGoalieBallLocation.x << ", y = " << friendGoalieBallLocation.y << ", timeDiff = "<< goalieTimeDiff << " relevant = " << isGoalieBallLocationRelevant << "\n";
				cout << "Striker's ball position : x = "  << friendStrikerBallLocation.x << ", y = " << friendStrikerBallLocation.y << ", timeDiff = "<< strikerTimeDiff << " relevant = " << isStrikerBallLocationRelevant << "\n";
			#endif


			// calculate the weight of the striker location of the ball.
			strikerWeight 	= 	isStrikerBallLocationRelevant *
								((2 * (1 - (double) strikerTimeDiff	/ MAX_VALID_TIME_BALL_LOCATION_FROM_FRIEND))
								+ (1 - (double) friendStrikerBallLocation.distanceFromMe / MAX_VALID_DISTANCE_BALL_LOCATION_FROM_FRIEND));
			// calculate the weight of the goalie location of the ball.
			goalieWeight 	= 	isGoalieBallLocationRelevant *
								((2 * (1 - (double) goalieTimeDiff / MAX_VALID_TIME_BALL_LOCATION_FROM_FRIEND))
								+ (1 - (double) friendGoalieBallLocation.distanceFromMe	/ MAX_VALID_DISTANCE_BALL_LOCATION_FROM_FRIEND));
			// calculate the weighted x coordinate of the ball according to both striker friend and goalie.
			weightedX = ((strikerWeight * friendStrikerBallLocation.x) + (goalieWeight * friendGoalieBallLocation.x)) /
						(strikerWeight + goalieWeight);
			// calculate the weighted y coordinate of the ball according to both striker friend and goalie.
			weightedY = ((strikerWeight * friendStrikerBallLocation.y) + (goalieWeight * friendGoalieBallLocation.y)) /
						(strikerWeight + goalieWeight);
			ballLocationAbs.x = weightedX;
			ballLocationAbs.y = weightedY;
			// if one of the ball locations is valid - the calculation is valid.

			#ifdef DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
				cout << "Striker::CalculateBallLocationFromFriends: ball position after weighing : x = " << ballLocationAbs.x << ", y = " << ballLocationAbs.y << "\n";
			#endif
		}
	}
	else
	{
		ballLocationAbs.isRelevant = false;
	}

/*	#ifdef DEBUG_CALC_BALL_LOCATION_FROM_FRIENDS
		cout << "Striker::CalculateBallLocationFromFriends end\n";
	#endif
*/
	return ballLocationAbs;
}


State_t Striker::DoStateGoToTheBall()
{
	m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	BallLocation ballLocation = m_localization->GetBallLocation();
	m_goTo->StartGoToBall();
	if (!IsStatePlaying())
	{
		return STATE_HANDLE_GAME_CONTROLLER_STATES;
	}
	while ( (IsBallDataRelevant(&ballLocation) == true || m_goTo->IsKeepMeAlive() ) && m_goTo->IsReached()	== false && ShouldIGoToBall()/* && m_goTo->GetThreadStatus() != IDLE*/)
	{
		#ifdef DEBUG_STRIKER_GO_TO_THE_BALL
			cout << "[Striker]: Go to the ball thread is running...\n";
		#endif

		if (!IsStatePlaying())
		{
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}
		usleep(100 * 1000);
		ballLocation = m_localization->GetBallLocation();
	}
	#ifdef DEBUG_STRIKER_GO_TO_THE_BALL
		cout << "[Striker]: Stopping go to the ball thread...\n";
	#endif

	#ifdef DEBUG_STRIKER_GO_TO_THE_BALL
		cout << "[Striker]: " << IsBallDataRelevant(&ballLocation) << m_goTo->IsKeepMeAlive() << m_goTo->IsReached() << ShouldIGoToBall() <<"\n";
	#endif
	m_goTo->Stop();
	if (m_goTo->IsReached() == true)
	{
		return STATE_IS_IT_POSSIBLE_TO_KICK;
	}
	return STATE_LOOK_FOR_THE_BALL;
}

State_t Striker::DoStatePositionYourself()
{
	m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	BallLocation ballLocationFromMe;
	XYLocation_t ballLocation;
	XYLocation_s newPosition;
	cm xCloseBar;
	bool isStop = false;
	ballLocationFromMe = m_localization->GetBallLocation();
	while (!ShouldIGoToBall() && IsStatePlaying())
	{
		#ifdef DEBUG_STRIKER_POSIION_YOURSELF
			cout << "[Striker::DoStatePositionYourself]: my friend should go to the ball.\n";
		#endif
		if (IsBallDataRelevant(&ballLocationFromMe) == false)
		{
			ballLocation = CalculateBallLocationFromFriends();
		}
		else
		{
			ballLocation.x = ballLocationFromMe.x;
			ballLocation.y = ballLocationFromMe.y;
		}

/**************** decide X coordinate ********************/

		if (ballLocation.x < X_HALF_FIELD_COORDINATE )
		{
			newPosition.x = POSITION_IN_RIGHT_SIDE;
		}
		else
		{
			newPosition.x = POSITION_IN_LEFT_SIDE;
		}

/************ decide Y coordinate *************************/

		if( ballLocation.y < Y_LOW_FIELD_COORDINATE )
		{
			newPosition.y = POSITION_Y_LOW_FIELD;
		}
		else if ( ballLocation.y < Y_MID_LOW_FIELD_COORDINATE )
		{
			newPosition.y = POSITION_Y_MID_LOW_FIELD;
		}
		else if ( ballLocation.y < Y_MID_HIGH_FIELD_COORDINATE )
		{
			newPosition.y = POSITION_Y_MID_HIGH_FIELD;
		}
		else
		{
			newPosition.y = POSITION_Y_HIGH_FIELD;
		}



		// Defensive position
//		else
//		{
//			// The ball is between 180 cm to 300 cm in the field - stand 100 cm behind it.
//			if (ballLocation.y > DANGER_ZONE_DISTANCE_Y)
//			{
//				newPosition.y = ballLocation.y - DIFF_DISTANCE_FROM_BALL_Y;
//			}
//			// The ball is between 0 cm to 180 cm in the field - stand in the middle between it and our goal.
//			else
//			{
//				newPosition.y = ballLocation.y / 2;
//			}
//			if (ballLocation.x > 0)
//			{
//				xCloseBar = X_BAR_RIGHT_DEFENCE;
//			}
//			else
//			{
//				xCloseBar = X_BAR_LEFT_DEFENCE;
//			}
//			newPosition.x = (((xCloseBar / 2) - (ballLocation.x))
//					* newPosition.y) / (ballLocation.y) + ballLocation.x;
//		}

		if(IsBallDataRelevant(&ballLocationFromMe) && ballLocationFromMe.distanceFromMe < 100)
		{
			#ifdef DEBUG_STRIKER_POSIION_YOURSELF
				cout << "[Striker::DoStatePositionYourself]: the ball is less than 100 cm from me - I'll stop.\n";
			#endif
			m_goTo->Stop();
			isStop = true;
		}
		else
		{
			#ifdef DEBUG_STRIKER_POSIION_YOURSELF
				cout << "[Striker::DoStatePositionYourself]: either the ball is far than 100 cm or I can't see it.\n";
			#endif
			if( !(isStop && IsBallDataRelevant(&ballLocationFromMe) && ballLocationFromMe.distanceFromMe < 130) )
			{
				#ifdef DEBUG_STRIKER_POSIION_YOURSELF
					if(!isStop)
					{
						cout << "[Striker::DoStatePositionYourself]: I didn't stop before.\n";
					}
					if(!IsBallDataRelevant(&ballLocationFromMe))
					{
						cout << "[Striker::DoStatePositionYourself]: I can't see the ball.\n";
					}
					if(!(ballLocationFromMe.distanceFromMe < 130))
					{
						cout << "[Striker::DoStatePositionYourself]: The ball is more than 130 cm away.\n";
					}
				#endif
				m_goTo->StartGoToXY(newPosition);
				isStop = false;
			}
		}
		usleep(150 * 1000);
		ballLocationFromMe = m_localization->GetBallLocation();
	}
	if (IsStatePlaying())
	{
		return STATE_LOOK_FOR_THE_BALL;
	}
	return STATE_HANDLE_GAME_CONTROLLER_STATES;
}

bool Striker::IsIBlocked()
{
	return false;
	//	throw "Guy should implement it in vision";
}


State_t Striker::DoStateShouldIGoToTheBall()
{
	// Assumption: the robot sees the ball.
	BallLocation myBallLocation;

	if (!IsStatePlaying())
	{
		return STATE_HANDLE_GAME_CONTROLLER_STATES;
	}
	if (ShouldIGoToBall() == false)
	{
		myBallLocation = m_localization->GetBallLocation();
		if (IsBallDataRelevant(&myBallLocation) == true)
		{
			BallLocation friendStrikerBallLocation = m_communication->GetFriendData(m_otherStrikerId)->ballLocation;

			if ((IsBallDataRelevant(&friendStrikerBallLocation) == false) ||
					(myBallLocation.distanceFromMe < friendStrikerBallLocation.distanceFromMe))
			{
				return STATE_GO_TO_THE_BALL;
			}
		}
		return STATE_POSITION_YOURSELF;
	}
	return STATE_GO_TO_THE_BALL;
}

bool Striker::ShouldIGoToBall()
{
	bool shouldIGoToBall;
	bool isCommunication = IsCommunicationOn();
	// No communication
	if (!isCommunication && !m_isLastCommunicationOn)
	{
		#ifdef DEBUG_STRIKER_SHOULD_I_GO_TO_BALL
			cout << "[Striker::ShouldIGoToBall]: No communication, not now and not the time before that - I'm going to the ball\n";
		#endif
		m_didIGoLastTime = true;
		return true;
	}
	m_isLastCommunicationOn = true;
	// The other striker doesn't see the ball
	if (!IsBallDataRelevant(&m_communication->GetFriendData(m_otherStrikerId)->ballLocation))
	{
		#ifdef DEBUG_STRIKER_SHOULD_I_GO_TO_BALL
			cout << "[Striker::ShouldIGoToBall]: My striker friend doesn't see the ball\n";
		#endif
		m_didIGoLastTime = true;
		return true;
	}

	// We both see the ball
	if (IsBallDataRelevant(m_localization->GetBallLocationByRef()))
	{
		shouldIGoToBall = (m_communication->GetFriendData(m_otherStrikerId)->ballLocation.distanceFromMe
				>= m_localization->GetBallLocation().distanceFromMe)
				||
				(m_didIGoLastTime && m_communication->GetFriendData(m_otherStrikerId)->ballLocation.distanceFromMe + 30
						>= m_localization->GetBallLocation().distanceFromMe);
		#ifdef DEBUG_STRIKER_SHOULD_I_GO_TO_BALL
		if(shouldIGoToBall)
		{
			m_didIGoLastTime = true;
			cout << "[Striker::ShouldIGoToBall]: I'm closer than my friend - I'm going to the ball\n";
		}
		else
		{
			m_didIGoLastTime = false;
			cout << "[Striker::ShouldIGoToBall]: my friend is closer than me - I'm not going to the ball\n";
		}
		#endif
		return shouldIGoToBall;
	}
	// The other striker sees the ball and I don't.
	return false;
}

State_t Striker::DoStateIsItPossibleToKick()
{
	Location myLocation = m_localization->GetMyLocation();

	if (!IsStatePlaying())
	{
		return STATE_HANDLE_GAME_CONTROLLER_STATES;
	}
	if (IsIBlocked())
	{
		return STATE_PASS_AWAY;
	}
	if (CalculateDistanceFromMe(OPPONENT_GOAL_X, OPPONENT_GOAL_Y)
			> KICKABLE_DISTANCE_FROM_OPPONENT_GOAL
			&& myLocation.y < Y_HALF_FIELD_COORDINATE
			&& myLocation.y > Y_MAX_DANGER_ZONE_DONT_DRIBBLE)
	{
		return STATE_DRIBBLE;
	}
	return STATE_KICK;
}

/*cm Striker::CalculateDistance(cm fromXLocation, cm fromYLocation, cm toXLocation, cm toYLocation)
 {
 cm tempY = yLocation - m_localization->GetMyLocation().y;
 cm tempX = xLocation - m_localization->GetMyLocation().x;
 return (sqrt(tempY * tempY + tempX * tempX));
 }
 */

cm Striker::CalculateDistance(cm fromXLocation, cm fromYLocation,
		cm toXLocation, cm toYLocation)
{
	cm tempX = fromXLocation - toXLocation;
	cm tempY = fromYLocation - toYLocation;
	return (sqrt(tempY * tempY + tempX * tempX));
}

cm Striker::CalculateDistanceFromMe(cm xLocation, cm yLocation)
{
	return CalculateDistance(m_localization->GetMyLocation().x, xLocation,
			m_localization->GetMyLocation().y, yLocation);
}

cm Striker::CalculateDistanceFromOtherStriker(cm xLocation, cm yLocation)
{
	return CalculateDistance(
			m_communication->GetFriendData(m_otherStrikerId)->ballLocation.x,
			m_communication->GetFriendData(m_otherStrikerId)->ballLocation.y,
			m_localization->GetMyLocation().y, yLocation);
}

State_t Striker::DoStateKick()
{
	if (!IsStatePlaying())
	{
		return STATE_HANDLE_GAME_CONTROLLER_STATES;
	}

	AdjustForAKickAndKick();

	return STATE_LOOK_FOR_THE_BALL;
}

State_t Striker::DoStateDribble()
{
	m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	// Assumption: The robot is close to the ball face to the opponent goal.
	bool isAlreadyDribbling = false;
	cm prevBallDistanceFromMe;
	BallLocation ballLocation = m_localization->GetBallLocation();
	while (IsBallDataRelevant(&ballLocation) && (ballLocation.angleFromMe
			< VALID_ANGLE_FOR_DRIBBLE && ballLocation.angleFromMe
			> -VALID_ANGLE_FOR_DRIBBLE) && !IsIBlocked()
			&& CalculateDistance(m_localization->GetMyLocation().x,
					m_localization->GetMyLocation().y, OPPONENT_GOAL_X,
					OPPONENT_GOAL_Y) > KICKABLE_DISTANCE_FROM_OPPONENT_GOAL)
	{
		if (!IsStatePlaying())
		{
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}
		if (!isAlreadyDribbling)
		{
			AdjustYourselfTowardTheBallForDribble();
			m_motion->StartWalkingAsync(DRIBBLE_X_SPEED, 0, 0);
			isAlreadyDribbling = true;
		}
		prevBallDistanceFromMe = ballLocation.distanceFromMe;
		usleep(500 * 1000);
		ballLocation = m_localization->GetBallLocation();
		if (ballLocation.distanceFromMe > prevBallDistanceFromMe + DISTANCE_FROM_BALL_THRESHOLD)
		{
			return STATE_SHOULD_I_GO_TO_THE_BALL;
		}
	}
	return STATE_LOOK_FOR_THE_BALL;
}

State_t Striker::DoStatePassAway()
{
	// Assumption: the ball located between the robot's legs.
	m_vision->SetHeadTrackingStatus(KICKING_BALL_TRACKING);
	if (!IsStatePlaying())
	{
		return STATE_HANDLE_GAME_CONTROLLER_STATES;
	}
	BallLocation ballLocation = m_localization->GetBallLocation();
	if (ballLocation.x < 0)
	{
		m_motion->StartAction(PASS_LEFT);
	}
	else
	{
		m_motion->StartAction(PASS_RIGHT);
	}
	m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	return STATE_LOOK_FOR_THE_BALL;
}


/*void Striker::GoToBall()
 {
 BallLocation ballLocation;
 //bool isBallInSight = false;
 //Location location;
 while(1)
 {
 ballLocation = m_localization->GetBallLocation();
 //cout << "Distance from ball " << ballLocation.distanceFromMe << endl;
 //cout << "ballLocation.isInSight = " << ballLocation.isInSight << endl;
 //location = m_localization->GetMyLocation();
 //isBallInSight = false;
 //cout << "Angle is: " << ballLocation.angleFromMe ;
 while (ballLocation.isInSight == false)
 {
 //isBallInSight = true;
 usleep(500000);

 ballLocation = m_localization->GetBallLocation();
 if (ballLocation.isInSight == false)
 m_motion->StopWalking();

 }
 if (ballLocation.distanceFromMe > FAR_DISTANCE_CM)
 {
 if(ballLocation.angleFromMe > ZERO_ANGLE_LEFT_FAR && ballLocation.angleFromMe < ZERO_ANGLE_RIGHT_FAR)
 {
 m_motion->StartWalking(RUNNING_MAX_FORWARD_SPEED, 0, 0);
 }
 else if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_FAR)
 {
 if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_FAR - 10)
 {
 m_motion->StartWalking(RUNNING_MAX_FORWARD_SPEED / 5, 0, MAX_TURN_LEFT_SPEED );
 }
 else
 {
 m_motion->StartWalking(RUNNING_MAX_FORWARD_SPEED / 4, 0, MAX_TURN_LEFT_SPEED/2 );
 }
 }
 else
 {
 if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_FAR+10)
 {
 m_motion->StartWalking(RUNNING_MAX_FORWARD_SPEED / 5, 0, MAX_TURN_RIGHT_SPEED );
 }
 else
 {
 m_motion->StartWalking(RUNNING_MAX_FORWARD_SPEED / 4, 0, MAX_TURN_RIGHT_SPEED/2 );
 }
 }
 }
 else if (ballLocation.distanceFromMe > SLOWING_DOWN_DISTANCE)
 {
 if(ballLocation.angleFromMe > ZERO_ANGLE_LEFT_FAR && ballLocation.angleFromMe < ZERO_ANGLE_RIGHT_FAR)
 {
 m_motion->StartWalking(RUNNING_MAX_FORWARD_SPEED / 3 , 0, 0);
 }
 else if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_FAR)
 {
 if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_FAR-10)
 {
 m_motion->StartWalking(0, 0, MAX_TURN_LEFT_SPEED );
 }
 else
 {
 m_motion->StartWalking(0, 0, MAX_TURN_LEFT_SPEED/2 );
 }
 }
 else
 {
 if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_FAR + 10)
 {
 m_motion->StartWalking(0, 0, MAX_TURN_RIGHT_SPEED );
 }
 else
 {
 m_motion->StartWalking(0, 0, MAX_TURN_RIGHT_SPEED/2 );
 }
 }
 }
 // Between kickable position and slowing down position.
 else if (ballLocation.distanceFromMe > KICKABLE_DISTANCE_CM)
 {
 if(ballLocation.angleFromMe > ZERO_ANGLE_LEFT_CLOSE && ballLocation.angleFromMe < ZERO_ANGLE_RIGHT_CLOSE)
 {
 m_motion->StartWalking(RUNNING_MAX_FORWARD_SPEED / 4,0,0);
 }
 else if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_CLOSE)
 {
 if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_CLOSE - 15)
 {
 m_motion->StartWalking(0, 0, MAX_TURN_LEFT_SPEED );
 }
 else
 {
 m_motion->StartWalking(0, 0, MAX_TURN_LEFT_SPEED/2 );
 }
 }
 else
 {
 if (ballLocation.angleFromMe < ZERO_ANGLE_LEFT_CLOSE + 5)
 {
 m_motion->StartWalking(0, 0, MAX_TURN_RIGHT_SPEED );
 }
 else
 {
 m_motion->StartWalking(0, 0, MAX_TURN_RIGHT_SPEED/2 );
 }
 }
 }
 else
 {
 cout << "StopWalking()\n";
 m_motion->StopWalking();
 cout << "SetNeedToDetectGoal(true)\n";
 if (f_idan == false)
 {
 f_idan = true;
 m_vision->SetNeedToDetectGoal(true);
 while(m_vision->GetGoalCenterOffset() == 1000)
 {
 usleep(50*1000);
 }
 cout << "Goal has been detected\n";
 double offsetFromGoal = m_vision->GetGoalCenterOffset();
 m_vision->SetNeedToDetectGoal(false);
 m_vision->SetDetectBallNearYou(true);

 cout << "offsetFromGoal = " << offsetFromGoal << "\n";
 if(offsetFromGoal > 15 ) //turn right
 {
 m_motion->StartWalking(-3,-20,11);
 usleep(1000 * abs(offsetFromGoal) * 1000 / 9.7);
 m_motion->StartAction(STAND_UP);
 }
 else if(offsetFromGoal < -15 ) //turn left
 {
 m_motion->StartWalking(-3,20,-11);
 usleep(1000 * abs(offsetFromGoal) * 1000 / 9.7);
 m_motion->StartAction(STAND_UP);
 }
 else
 {
 m_vision->SetNeedToDetectGoal(false);
 m_vision->SetDetectBallNearYou(true);
 while(ballLocation.isInSight == false)
 {
 usleep(1000 * 100);
 }
 m_motion->StopWalking();
 if(ballLocation.angleFromMe < 0)
 {
 m_motion->StartAction(KICK_LEFT);
 }
 else
 {
 m_motion->StartAction(KICK_RIGHT);
 }
 f_idan=false;
 }
 }
 else
 {
 m_vision->SetNeedToDetectGoal(false);
 m_vision->SetDetectBallNearYou(true);
 while(ballLocation.isInSight == false)
 {
 usleep(1000 * 100);
 }
 m_motion->StopWalking();
 if(ballLocation.angleFromMe < 0)
 {
 m_motion->StartAction(KICK_LEFT);
 }
 else
 {
 m_motion->StartAction(KICK_RIGHT);
 }
 f_idan=false;
 }
 m_vision->SetNeedToDetectGoal(false);
 //cout << "Angle from goal is: " << m_vision->GetGoalCenterOffset() << endl;
 }
 cout << "Walking mode is: " << m_motion->GetWalkingModeByString() << "\n";
 cout << "X speed is: " << m_motion->GetMotionValue(X_SPEED) << "\n";
 cout << "Y speed is: " << m_motion->GetMotionValue(Y_SPEED) << "\n";
 cout << "Angle is: " << m_motion->GetMotionValue(ROTATION_ANGLE) << "\n";
 //    	cout << "X head angle is: " << m_motion->GetHeadAngleValue(X_ANGLE) << "\n";
 //		cout << "Y head angle is: " << m_motion->GetHeadAngleValue(Y_ANGLE) << "\n";
 //		cout << "distance from ball: " << ballLocation.distanceFromMe << "\n";
 //		cout << "ball angle from me ball: " << ballLocation.angleFromMe << "\n";
 //		cout << "z offset: " << Walking::GetInstance()->Z_OFFSET << "\n";
 //		cout << "\n \n \n \n \n \n \n \n \n\n\n\n\n\n\n\n\n\n\n\n\n" << endl;
 }
 }*/
