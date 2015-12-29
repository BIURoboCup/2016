/*
 * Goalkeeper.cpp
 *
 */

#include "Goalkeeper.h"
#include "Localization/PrivateClasses/LocalizationTypedefs.h"

#define DEBUG_GOALKEEPER_STATE_MACHINE
//#define DEBUG_GOALKEEPER_STEP
#define DEBUG_GOALKEEPER_JUMP
//#define DEBUG_GOALKEEPER_TURN_TO_MIDDLE
#define DEBUG_GOALKEEPER_SEND_THE_BALL_AWAY
#define DEBUG_GOALKEEPER_GO_BACK_TO_GOAL_CENTER
#define DEBUG_GOALKEEPER_SHOULD_I_GO_TO_BALL

#define STEP_BY_HEAD_ANGLE
Goalkeeper* Goalkeeper::m_uniqueInstance = 0;

Goalkeeper::Goalkeeper() {
	cout << "Goalkeeper::Goalkeeper()" << endl;
	m_ballLocation = m_localization->GetBallLocation();
	m_listCombinedData = m_localization->getBallList();
	m_stepSide = DONT_MOVE;
	m_jumpSide = DONT_JUMP;
	m_firstStrikerId = 1;
	m_secondStrikerId = 2;
	m_currentTime.update();
	m_myLocationPtr = m_localization->GetMyLocationByRef();
}

Goalkeeper::~Goalkeeper() {
}

Goalkeeper* Goalkeeper::GetInstance() {
	if (m_uniqueInstance == 0)

	{
		m_uniqueInstance = new Goalkeeper();
	}
	return m_uniqueInstance;
}

State_t Goalkeeper::DoStateGameControllerReady() {
	XYLocation_s positionLocation;

	positionLocation.x = 0;
	positionLocation.y = GOALKEEPER_POSITION_Y;	//a little ahead of the goal line
	m_vision->SetHeadTrackingStatus(GOAL_TRACKING);

	while (exitProgram == false) {
		m_goTo->QuickAndDirty(positionLocation.x, positionLocation.y, 0);

		if (!IsStateReady()) {
			m_vision->SetHeadTrackingStatus(BALL_TRACKING);
			m_goTo->Stop();
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		usleep(100 * 1000);
	}

	return STATE_HANDLE_GAME_CONTROLLER_STATES;
}

State_t Goalkeeper::RunState(State_t cur_state) {
	switch (cur_state) {
	case STATE_INITIALIZE:
		return DoStateInitialize();
	case STATE_POSITION_YOURSELF:
		return DoStatePositionYourself();
	case STATE_JUMP_TO_SAVE:
		return DoStateJumpToSave();
	case STATE_TURN_TO_MIDDLE:
		return DoStateTurnToMiddle();
	case STATE_SEND_THE_BALL_AWAY:
		return DoStateSendTheBallAway();
	case STATE_GO_BACK_TO_GOAL_CENTER:
		return DoStateGoBackToGoalCenter();
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
	case STATE_PENALTY_MODE:
		return DoStatePenaltyMode();
	default:
		throw "[Goalkeeper::RunState]: Unknown state\n";
	}
}

State_t Goalkeeper::DoStateInitialize() {
#ifdef DEBUG_GOALKEEPER_STATE_MACHINE
	cout << "[Goalkeeper]::DoStateInitialize\n";
#endif
	m_motion->StartAction(STAND_UP);
	m_vision->SetHeadTrackingStatus(BALL_TRACKING);

	return STATE_POSITION_YOURSELF;
}

State_t Goalkeeper::DoStatePositionYourself() {
#ifdef DEBUG_GOALKEEPER_STATE_MACHINE
	cout << "[Goalkeeper]::DoStatePositionYourself\n";
#endif

	while (exitProgram == false) {
		if (!IsStatePlaying()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		if (WalkingAndActions::m_isMiddleButtonPressedAI == true) {
			WalkingAndActions::m_isMiddleButtonPressedAI = false;
			return STATE_GO_BACK_TO_GOAL_CENTER;
		}

		m_ballLocation = m_localization->GetBallLocation();

		if (IsBallDataRelevant(&m_ballLocation) && (m_myLocationPtr->y) < 175) {
			m_vision->SetHeadTrackingStatus(BALL_TRACKING);

			m_jumpSide = IsBallComing();
			if (m_jumpSide != DONT_JUMP) {
				return STATE_JUMP_TO_SAVE;
			}

			if (IsBallInTheDangerZone()) {
				//m_motion->StopWalking();
				if (ShouldIGoToBall() == true) {
					return STATE_SEND_THE_BALL_AWAY;
				}
			}

			m_stepSide = GetStepSide();
			DoGoalkeeperStep(m_stepSide);

			if (fabs(m_myLocationPtr->angle) > 15) {
				m_goTo->QuickAndDirty(m_myLocationPtr->x, GOALKEEPER_POSITION_Y,
						0, true);
			} else {
				m_motion->StopWalking();
			}
		}

		else {
			if (fabs(m_myLocationPtr->angle) > 15) {
				m_vision->SetHeadTrackingStatus(GOAL_TRACKING);
			} else {
				m_vision->SetHeadTrackingStatus(BALL_TRACKING);
			}

			if (m_goTo->QuickAndDirty(GOALKEEPER_POSITION_X,
					GOALKEEPER_POSITION_Y, 0, false)) {
				m_motion->StopWalking();
			}
		}
		usleep(GOALKEEPER_ITERATION_TIME);
	}
	return STATE_POSITION_YOURSELF;
}

State_t Goalkeeper::DoStatePenaltyMode() {
#ifdef DEBUG_GOALKEEPER_STATE_MACHINE
	cout << "[Goalkeeper]::DoStatePenaltyMode\n";
#endif

	while (exitProgram == false) {
		if (!IsStatePlaying()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		m_ballLocation = m_localization->GetBallLocation();

		if (IsBallDataRelevant(&m_ballLocation)) {
			m_jumpSide = IsBallComing();
			if (m_jumpSide != DONT_JUMP) {
				return STATE_JUMP_TO_SAVE;
			}
		}
		usleep(GOALKEEPER_ITERATION_TIME);
	}
	return STATE_PENALTY_MODE;
}

State_t Goalkeeper::DoStateJumpToSave() {
#ifdef DEBUG_GOALKEEPER_STATE_MACHINE
	cout << "[Goalkeeper]::DoStateJumpToSave\n";
#endif

	m_listCombinedData->RemoveAllCombinedDatas();

	if (m_jumpSide == JUMP_LEFT_SIDE) {
		m_motion->StartAction(JUMP_LEFT);
		m_localization->setStepAside('L');

		sleep(1);
		//m_motion->StartAction(SIT_DOWN);
		//sleep(1);
		//m_motion->StartAction(STAND_UP);
	} else if (m_jumpSide == JUMP_RIGHT_SIDE) {
		m_motion->StartAction(JUMP_RIGHT);
		m_localization->setStepAside('R');
		sleep(1);
		//m_motion->StartAction(SIT_DOWN);
		//sleep(1);
		//m_motion->StartAction(STAND_UP);
	} else if (m_jumpSide == JUMP_MIDDLE_SIDE) {
		m_motion->StartAction(JUMP_MIDDLE);
		m_motion->StartAction(HAND_MOVE);
		m_motion->StartAction(STAND_UP);
	}
	//Stand up
	return STATE_TURN_TO_MIDDLE;
}

State_t Goalkeeper::DoStateTurnToMiddle() {
#ifdef DEBUG_GOALKEEPER_STATE_MACHINE
	cout << "[Goalkeeper]::DoStateTurnToMiddle\n";
#endif

	m_turnCount = 0;
	int turnCountSide = 0;
	Angle angleStartTurning;
	myTimeval timeStartedTurning;

	timeStartedTurning.update();

	if (m_jumpSide == JUMP_RIGHT_SIDE) {
		turnCountSide = MAX_TURN_COUNT_TO_MIDDLE_FOR_RIGHT_JUMP;
		m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	} else if (m_jumpSide == JUMP_LEFT_SIDE) {
		turnCountSide = MAX_TURN_COUNT_TO_MIDDLE_FOR_LEFT_JUMP;
		m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	} else if (m_jumpSide == JUMP_MIDDLE_SIDE) {
		return STATE_SEND_THE_BALL_AWAY;
	}

	while (m_turnCount < turnCountSide) {
		m_ballLocation = m_localization->GetBallLocation();
		m_currentTime.update();

		if (!IsStatePlaying()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		switch (m_jumpSide) {
		case DONT_JUMP:
			return STATE_POSITION_YOURSELF;
		case JUMP_RIGHT_SIDE:
			m_motion->StartWalkingAsync(0, 0, -MAX_TURN_LEFT_SPEED);
			break;
		case JUMP_LEFT_SIDE:
			m_motion->StartWalkingAsync(0, 0, MAX_TURN_LEFT_SPEED);
			break;
		case JUMP_MIDDLE_SIDE:		//probably unnecessary
			return STATE_SEND_THE_BALL_AWAY;
		}

		if (m_currentTime - timeStartedTurning > MIN_TIME_FOR_TURNING) {
			if (IsBallDataRelevant(&m_ballLocation)) {
				m_motion->StopWalking();
				return STATE_SEND_THE_BALL_AWAY;
			}
		}

		m_turnCount++;
		usleep(GOALKEEPER_ITERATION_TIME);
	}

#ifdef DEBUG_GOALKEEPER_TURN_TO_MIDDLE
	cout << "Done Goalkeeper turn to middle by time\n";
#endif

	m_motion->StopWalking();

	m_vision->SetHeadTrackingStatus(GOAL_TRACKING);
	angleStartTurning = m_myLocationPtr->angle;
#ifdef DEBUG_GOALKEEPER_TURN_TO_MIDDLE
	cout << "[Goalkeeper]::DoStateTurnToMiddle. angleStartTurning = " << angleStartTurning << "\n";
#endif
	while (fabs(m_myLocationPtr->angle) > MAX_ANGLE_FOR_GOALKEEPER_POSITION) {
		if (!IsStatePlaying()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		if (angleStartTurning < 0) {
			m_motion->StartWalkingAsync(0, 0, -MAX_TURN_LEFT_SPEED);
		} else {
			m_motion->StartWalkingAsync(0, 0, MAX_TURN_LEFT_SPEED);
		}

		if (IsBallDataRelevant(&m_ballLocation)) {
			m_motion->StopWalking();
			return STATE_SEND_THE_BALL_AWAY;
		}

		usleep(GOALKEEPER_ITERATION_TIME);
	}

	m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	m_motion->StopWalking();
	return STATE_POSITION_YOURSELF;
}

State_t Goalkeeper::DoStateSendTheBallAway() {
#ifdef DEBUG_GOALKEEPER_STATE_MACHINE
	cout << "[Goalkeeper]::DoStateSendTheBallAway\n";
#endif

	bool isGoToBallActive = false;
	bool isBallReached;

	double diffYFromBall;

#ifdef DEBUG_GOALKEEPER_SEND_THE_BALL_AWAY
	cout << "[Goalkeeper]::GoTo Started\n";
#endif

	do {
		m_ballLocation = m_localization->GetBallLocation();

		diffYFromBall = m_ballLocation.y - m_myLocationPtr->y;

		if (!isGoToBallActive) {
			m_goTo->StartGoToBall();
			isGoToBallActive = true;
		}

		if (!IsStatePlaying()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		usleep(GOALKEEPER_ITERATION_TIME);

	} while (m_myLocationPtr->y < MAX_Y_TO_SEND_THE_BALL_AWAY
			&& diffYFromBall < MAX_Y_DIFF_FROM_BALL_TO_RETURN_TO_GOAL
			&& (m_goTo->IsKeepMeAlive()
					|| (IsBallDataRelevant(&m_ballLocation)
							&& (!m_goTo->IsReached())))); // TODO: add condition that the ball is out of danger zone with a second threshold

	isBallReached = m_goTo->IsReached();
	m_goTo->Stop();

	m_vision->SetHeadTrackingStatus(KICKING_BALL_TRACKING);
	m_ballLocation = m_localization->GetBallLocation();
	m_currentTime.update();

#ifdef DEBUG_GOALKEEPER_SEND_THE_BALL_AWAY
	cout << "[Goalkeeper]::GoTo Stopped. IsReached = " << isBallReached
			<< ", Time since ball detected = "
			<< m_currentTime - m_ballLocation.time << "\n";
#endif

	if (!IsBallDataRelevant(&m_ballLocation)) {
#ifdef DEBUG_GOALKEEPER_SEND_THE_BALL_AWAY
		cout
				<< "[Goalkeeper]::Ball Data is not relevant, going to STATE_GO_BACK_TO_GOAL_CENTER\n";
#endif
		m_vision->SetHeadTrackingStatus(BALL_TRACKING);
		return STATE_POSITION_YOURSELF;
	}

	if (isBallReached == true) {
		AdjustForAKickAndKick();
		m_motion->StartAction(STAND_UP);
		m_vision->SetHeadTrackingStatus(BALL_TRACKING);
		return STATE_POSITION_YOURSELF; // TODO: kick and go to goalie's position
	}

	if (IsBallInTheDangerZone()) {
#ifdef DEBUG_GOALKEEPER_SEND_THE_BALL_AWAY
		cout
				<< "[Goalkeeper]::Ball is inside danger zone, going to send the ball away\n";
#endif
		m_vision->SetHeadTrackingStatus(BALL_TRACKING);
		return STATE_SEND_THE_BALL_AWAY;
	}

	return STATE_POSITION_YOURSELF;
}

bool Goalkeeper::IsBallInTheDangerZone() {
	CombinedData_t* iterator = m_listCombinedData->m_head;
	int numOfCombinedData = m_listCombinedData->GetNumberOfCombinedDatas();
//	if( (m_ballLocation.x > -BALL_IN_DANGER_ZONE_X && m_ballLocation.x < BALL_IN_DANGER_ZONE_X) && (m_ballLocation.y < BALL_IN_DANGER_ZONE_Y) )
	if (numOfCombinedData < MIN_NUM_OF_BALLS_TO_STATIC_BALL_DECISION) {
		return false;
	}

	double avgY = 0;
	double avgX = 0;

	for (; iterator != NULL; iterator = iterator->m_next) {
		avgY += iterator->ballLocation.y;
		avgX += iterator->ballLocation.x;
	}
	avgY = avgY / numOfCombinedData;
	avgX = avgX / numOfCombinedData;

	double varianceY = 0;
	for (iterator = m_listCombinedData->m_head; iterator != NULL; iterator =
			iterator->m_next) {
		varianceY += (iterator->ballLocation.y - avgY)
				* (iterator->ballLocation.y - avgY);
	}

	double stdY = sqrt(varianceY / numOfCombinedData);

//	cout << "stdY = " << stdY << "\n";
	if (stdY > EPSILON_BALL_MOVEMENT_STD) {
		return false;
	}

	if (avgY > BALL_IN_DANGER_ZONE_Y) {
		return false;
	}

	if (fabs(avgX) > BALL_IN_DANGER_ZONE_X) {
		return false;
	}

	return true;
}

JumpSide_e Goalkeeper::IsBallComing() {
//	cout<<"In isBallComing\n";

	if (m_listCombinedData->GetNumberOfCombinedDatas() == 0) {
		cout << "\n\nNo list\n\n";
		return DONT_JUMP;
	}

	CombinedData_t* combinedDataIterator = m_listCombinedData->m_head;

	m_currentTime.update();

	UpdateBallBuffer();

	m_ballTrendlineX.ResetTrendline();
	m_ballTrendlineY.ResetTrendline();

	if (m_currentTime - combinedDataIterator->ballLocation.time > 500 * 1000) {
		return DONT_JUMP;
	}
	if ((combinedDataIterator->ballLocation.y
			- combinedDataIterator->myLocation.y > FAR_BALL_DISTANCE)) // || (combinedDataIterator->myLocation.y < 0))
	{
		return DONT_JUMP;
	}
	if (IsBallStaticInRecentFrames() == true) {
		return DONT_JUMP;
	}

	//cout << "Num of balls = " << m_listCombinedData->GetNumberOfCombinedDatas() << endl;
	//cout<<"_list "<< i << ": bx = " << combinedDataIterator->ballLocation.x << " myY = " << combinedDataIterator->myLocation.y << " ,Ydiff = "<<combinedDataIterator->ballLocation.y - combinedDataIterator->myLocation.y << " time since: " << currentTime - combinedDataIterator->ballLocation.time <<  "\n";
	//int i;
	/*cout << "********************************************\n";

	 for(;  combinedDataIterator != m_listCombinedData->m_tail; combinedDataIterator = combinedDataIterator->m_next)
	 {
	 cout<<"_list "<< i << ": b dist = " << combinedDataIterator->ballLocation.distanceFromMe << " myY = " << combinedDataIterator->myLocation.y << " ,Ydiff = "<<combinedDataIterator->ballLocation.y - combinedDataIterator->myLocation.y << " time since: " << currentTime - combinedDataIterator->ballLocation.time <<  "\n";
	 i++;
	 }
	 cout << "********************************************\n";
	 */
	if (m_listCombinedData->GetNumberOfCombinedDatas() <= NUM_OF_BALLS_TO_DECIDE_JUMP) {
		return DONT_JUMP;
	}

	EraseFalseBalls();

	if (m_listCombinedData->GetNumberOfCombinedDatas() <= NUM_OF_BALLS_TO_DECIDE_JUMP) {
		return DONT_JUMP;
	}

	for (combinedDataIterator = m_listCombinedData->m_head;
			combinedDataIterator != NULL; combinedDataIterator =
					combinedDataIterator->m_next) {
		Point2DInt_t newTrendlinePointY;
		newTrendlinePointY.x = (m_currentTime
				- combinedDataIterator->ballLocation.time) / 1000;
		newTrendlinePointY.y = combinedDataIterator->ballLocation.y
				- combinedDataIterator->myLocation.y;

		Point2DInt_t newTrendlinePointX;
		newTrendlinePointX.x = (m_currentTime
				- combinedDataIterator->ballLocation.time) / 1000;
		newTrendlinePointX.y = combinedDataIterator->ballLocation.x
				- combinedDataIterator->myLocation.x;

		//cout << "newTrendlinePoint.y = " << newTrendlinePointY.y << " newTrendlinePoint.x = " << newTrendlinePointY.x << "\n";

		if (newTrendlinePointY.y < 0) {
			cout << "Error - newTrendlinePointY.y < 0, BallY = "
					<< combinedDataIterator->ballLocation.y << "MyY = "
					<< combinedDataIterator->myLocation.y << endl;
		}

		m_ballTrendlineY.AddPoint(&newTrendlinePointY);
		m_ballTrendlineX.AddPoint(&newTrendlinePointX);

		if (m_ballTrendlineY.GetNumOfPoints() > NUM_OF_BALLS_TO_DECIDE_JUMP) {
			m_ballTrendlineY.Initialize();
			m_ballTrendlineX.Initialize();
			break;
		}
	}

	double timeBallReachesGoal = m_ballTrendlineY.PredictPointByY(0);
	//cout << "timeBallReachesGoal = " << timeBallReachesGoal<< "\n";// << "slope = " << m_ballTrendlineY.GetSlope() << " inter = " << m_ballTrendlineY.GetIntecept() << endl;

	if ((timeBallReachesGoal < -1500) || (timeBallReachesGoal > 0)
			|| (m_ballTrendlineY.GetSlope() < 0)) {
		return DONT_JUMP;
	} else {
		double xBallCrossesGoal = m_ballTrendlineX.PredictPoint(
				timeBallReachesGoal);

		if (xBallCrossesGoal < FAR_LEFT_GOAL_THRESHOLD) {
			return DONT_JUMP;
		}

		else if (xBallCrossesGoal < MIDDLE_LEFT_GOAL_THRESHOLD) {
#ifdef DEBUG_GOALKEEPER_JUMP
			cout << " x cross = " << xBallCrossesGoal
					<< " timeBallReachesGoal = " << timeBallReachesGoal
					<< " slope = " << m_ballTrendlineY.GetSlope() << " inter = "
					<< m_ballTrendlineY.GetIntecept() << " m_count = "
					<< m_ballTrendlineY.GetNumOfPoints() << "\n";
			int i;
			for (i = 0, combinedDataIterator = m_listCombinedData->m_head;
					i <= NUM_OF_BALLS_TO_DECIDE_JUMP; combinedDataIterator =
							combinedDataIterator->m_next, i++) {
				cout
						<< "trendY.x = "
						<< m_ballTrendlineY.m_points[i].x
						<< " trendY.y = "
						<< m_ballTrendlineY.m_points[i].y
						<< ", trendX.x = "
						<< m_ballTrendlineX.m_points[i].x
						<< " trendX.y = "
						<< m_ballTrendlineX.m_points[i].y
						<< " time since: "
						<< m_currentTime
								- combinedDataIterator->ballLocation.time
						<< " distance = "
						<< combinedDataIterator->ballLocation.distanceFromMe
						<< "\n";
				//cout<<"Slope = "<<m_ballTrendlineY.GetSlope()<<"\n";
			}
#endif
			return JUMP_LEFT_SIDE;
		} else if (xBallCrossesGoal > FAR_RIGHT_GOAL_THRESHOLD) {
			return DONT_JUMP;
		} else if (xBallCrossesGoal > MIDDLE_RIGHT_GOAL_THRESHOLD) {
#ifdef DEBUG_GOALKEEPER_JUMP
			cout << " x cross = " << xBallCrossesGoal
					<< " timeBallReachesGoal = " << timeBallReachesGoal
					<< " slope = " << m_ballTrendlineY.GetSlope() << " inter = "
					<< m_ballTrendlineY.GetIntecept() << " m_count = "
					<< m_ballTrendlineY.GetNumOfPoints() << "\n";
			;
			int i;
			for (i = 0, combinedDataIterator = m_listCombinedData->m_head;
					i <= NUM_OF_BALLS_TO_DECIDE_JUMP; combinedDataIterator =
							combinedDataIterator->m_next, i++) {
				cout
						<< "trendY.x = "
						<< m_ballTrendlineY.m_points[i].x
						<< " trendY.y = "
						<< m_ballTrendlineY.m_points[i].y
						<< ", trendX.x = "
						<< m_ballTrendlineX.m_points[i].x
						<< " trendX.y = "
						<< m_ballTrendlineX.m_points[i].y
						<< " time since: "
						<< m_currentTime
								- combinedDataIterator->ballLocation.time
						<< " distance = "
						<< combinedDataIterator->ballLocation.distanceFromMe
						<< "\n";
				//cout<<"Slope = "<<m_ballTrendlineY.GetSlope()<<"\n";
			}
#endif

			return JUMP_RIGHT_SIDE;
		} else {
#ifdef DEBUG_GOALKEEPER_JUMP
			cout << " x cross = " << xBallCrossesGoal
					<< " timeBallReachesGoal = " << timeBallReachesGoal
					<< " slope = " << m_ballTrendlineY.GetSlope() << " inter = "
					<< m_ballTrendlineY.GetIntecept() << " m_count = "
					<< m_ballTrendlineY.GetNumOfPoints() << "\n";
			;
			int i;
			for (i = 0, combinedDataIterator = m_listCombinedData->m_head;
					i <= NUM_OF_BALLS_TO_DECIDE_JUMP; combinedDataIterator =
							combinedDataIterator->m_next, i++) {
				cout
						<< "trendY.x = "
						<< m_ballTrendlineY.m_points[i].x
						<< " trendY.y = "
						<< m_ballTrendlineY.m_points[i].y
						<< ", trendX.x = "
						<< m_ballTrendlineX.m_points[i].x
						<< " trendX.y = "
						<< m_ballTrendlineX.m_points[i].y
						<< " time since: "
						<< m_currentTime
								- combinedDataIterator->ballLocation.time
						<< " distance = "
						<< combinedDataIterator->ballLocation.distanceFromMe
						<< "\n";
				;
				//cout<<"Slope = "<<m_ballTrendlineY.GetSlope()<<"\n";
			}
#endif

			return JUMP_MIDDLE_SIDE;
		}
	}

}

void Goalkeeper::UpdateBallBuffer() {
	m_currentTime.update();

	CombinedData_t* combinedIterator = m_listCombinedData->m_tail;
	CombinedData_t* combinedDataToRemove;

	//cout << "Num of balls = " << m_listCombinedData->GetNumberOfBallLocations() << endl;
	while (combinedIterator != m_listCombinedData->m_head) {
		if (m_currentTime
				- combinedIterator->ballLocation.time > TIMEOUT_FOR_BALL_IN_BUFFER) {
			combinedDataToRemove = combinedIterator;
			combinedIterator = combinedIterator->m_prev;
			m_listCombinedData->RemoveCombinedData(combinedDataToRemove);
			//	cout << "Ball Removed. Head = " << m_listCombinedData->m_head << " Tail = " << m_listCombinedData->m_tail << endl;
		} else {
			break;
		}
	}
	//cout << "Num of balls = " << m_listCombinedData->GetNumberOfBallLocations() << endl;
}

bool Goalkeeper::IsBallStaticInRecentFrames() {
	CombinedData_t* combinedDataIterator;
	int i = 0;
	int numOfCombinedData = m_listCombinedData->GetNumberOfCombinedDatas();

	if (numOfCombinedData < NUM_OF_BALLS_TO_DECIDE_RECENT_BALL_STATIC) {
		return false;
	}

	double avgDiffFromBallY = 0;

	for (combinedDataIterator = m_listCombinedData->m_head;
			i < NUM_OF_BALLS_TO_DECIDE_RECENT_BALL_STATIC;
			combinedDataIterator = combinedDataIterator->m_next, i++) {
		avgDiffFromBallY += combinedDataIterator->ballLocation.y
				- combinedDataIterator->myLocation.y;
	}

	avgDiffFromBallY = avgDiffFromBallY
			/ NUM_OF_BALLS_TO_DECIDE_RECENT_BALL_STATIC;

	double varianceDiffFromBallY = 0;
	for (i = 0, combinedDataIterator = m_listCombinedData->m_head;
			i < NUM_OF_BALLS_TO_DECIDE_RECENT_BALL_STATIC;
			combinedDataIterator = combinedDataIterator->m_next, i++) {
		varianceDiffFromBallY +=
				(combinedDataIterator->ballLocation.y
						- combinedDataIterator->myLocation.y - avgDiffFromBallY)
						* (combinedDataIterator->ballLocation.y
								- combinedDataIterator->myLocation.y
								- avgDiffFromBallY);
	}

	double stdY = sqrt(
			varianceDiffFromBallY / NUM_OF_BALLS_TO_DECIDE_RECENT_BALL_STATIC);

//	cout << "stdY = " << stdY << "\n";
	if (stdY > EPSILON_BALL_MOVEMENT_STD) {
		return false;
	}

	return true;
}

StepSide_e Goalkeeper::GetStepSide() {
	//CombinedData_t* combinedDataIterator = m_listCombinedData->m_head;
	//int numOfCombinedData = m_listCombinedData->GetNumberOfCombinedDatas();
	static int counterSinceRobotMovedStep = 0;
	m_currentTime.update();
	double maxAngleForStepAside;

#ifdef GOALKEEPER_STEP__IF_THERE_WAS_GOOD_LOCALIZATION
	double angleBallPoint,angleEdgeRight,distanceRight,targetX;

	m_ballLocation = m_localization->GetBallLocation();
	Point2DInt_s ballPoint;

	ballPoint.x = m_ballLocation.x;
	ballPoint.y = m_ballLocation.y;

	double edgeRight = CalculateEuclideanDistance(&ballPoint,&(FixedPoles[GOAL_RIGHT_BACK]));
	double edgeLeft = CalculateEuclideanDistance(&ballPoint,&(FixedPoles[GOAL_LEFT_BACK]));

	//cos law
	angleBallPoint = RAD2DEG*acos( (SIZE_OF_GOAL_IN_CM*SIZE_OF_GOAL_IN_CM - edgeRight*edgeRight - edgeLeft*edgeLeft) / (-2*edgeLeft*edgeRight));
	angleEdgeRight = RAD2DEG*acos( (edgeLeft*edgeLeft - SIZE_OF_GOAL_IN_CM*SIZE_OF_GOAL_IN_CM - edgeRight*edgeRight) / (-2*SIZE_OF_GOAL_IN_CM*edgeRight));

	//sin law
	distanceRight = (edgeRight*sin(DEG2RAD* angleBallPoint/2.0)) /
	(sin(DEG2RAD*(180 - angleBallPoint/2.0 - angleEdgeRight)));

	if (fabs(m_myLocationPtr->angle) > MAX_ANGLE_FOR_GOALKEEPER_POSITION )
	{
		return DONT_MOVE;
	}

	if (numOfCombinedData == 0)
	{
		return DONT_MOVE;
	}

	if (counterSinceRobotMovedStep != 0)
	{
		counterSinceRobotMovedStep--;
		return DONT_MOVE;
	}

	if(m_currentTime - combinedDataIterator->ballLocation.time > 500*1000)
	{
		return DONT_MOVE;
	}

	m_ballLocation = m_localization->GetBallLocation();
#endif

	m_ballLocation = m_localization->GetBallLocation();

	if (m_currentTime - m_ballLocation.time > 300 * 1000) {
		return DONT_MOVE;
	}

#ifdef STEP_BY_HEAD_ANGLE
	if (fabs(m_myLocationPtr->angle) > 20) {
		return DONT_MOVE;
	}

	if (m_ballLocation.distanceFromMe > FAR_BALL_DISTANCE) {
		maxAngleForStepAside = MAX_ANGLE_FOR_STEP_ASIDE_FAR_DISTANCE;
	} else {
		maxAngleForStepAside = MAX_ANGLE_FOR_STEP_ASIDE_CLOSE_DISTANCE;
	}

#ifdef DEBUG_GOALKEEPER_STEP
	cout << "Ball angle from me = " << m_ballLocation.angleFromMe << "\n";
#endif

	if (m_ballLocation.angleFromMe > maxAngleForStepAside) {
#ifdef DEBUG_GOALKEEPER_STEP
		cout << "****************Step Right\n";
#endif
		counterSinceRobotMovedStep = NUM_OF_BALLS_TO_DECIDE_STEP; // wait for new balls till next step
		return MOVE_RIGHT_STEP;
	} else if (m_ballLocation.angleFromMe < -maxAngleForStepAside) {
#ifdef DEBUG_GOALKEEPER_STEP
		cout << "****************Step Left\n";
#endif
		counterSinceRobotMovedStep = NUM_OF_BALLS_TO_DECIDE_STEP; // wait for new balls till next step
		return MOVE_LEFT_STEP;
	} else {
		return DONT_MOVE;
	}
#endif

#ifdef GOALKEEPER_STEP__IF_THERE_WAS_GOOD_LOCALIZATION
	/* Decide the step side */
	targetX = FixedPoles[GOAL_RIGHT_BACK].x - distanceRight;
#ifdef DEBUG_GOALKEEPER_STEP
	cout << "MyLocationX = " << m_myLocationPtr->x << " ball x = " << m_ballLocation.x << " ball y = " << m_ballLocation.y << " targetX = " << targetX << "\n";
#endif

	if (fabs(targetX - m_myLocationPtr->x) < 20)
	{
		return DONT_MOVE;
	}
	else if (targetX < m_myLocationPtr->x)
	{
		counterSinceRobotMovedStep = NUM_OF_BALLS_TO_DECIDE_STEP;
		return MOVE_LEFT_STEP;
	}
	else
	{
		counterSinceRobotMovedStep = NUM_OF_BALLS_TO_DECIDE_STEP;
		return MOVE_RIGHT_STEP;
	}
#endif
}

void Goalkeeper::DoGoalkeeperStep(StepSide_e stepSide) {
	if (stepSide == MOVE_LEFT_STEP) {
		if (m_myLocationPtr->x > -MAX_X_FOR_GOALKEEPER_STEP) {
			m_localization->setStepAside('l');
			m_motion->StartAction(STEP_LEFT);
		}
	} else if (stepSide == MOVE_RIGHT_STEP) {
		if (m_myLocationPtr->x < MAX_X_FOR_GOALKEEPER_STEP) {
			m_localization->setStepAside('r');
			m_motion->StartAction(STEP_RIGHT);
		}
	}

}

double Goalkeeper::CalculateEuclideanDistance(Point2DInt_s* pointA,
		const Point2DInt_s* pointB) {

	return sqrt(
			(pointA->x - pointB->x) * (pointA->x - pointB->x)
					+ (pointA->y - pointB->y) * (pointA->y - pointB->y));
}

State_t Goalkeeper::DoStateGoBackToGoalCenter() {
	XYLocation_s positionLocation;
	Angle angleStartTurning;

#ifdef DEBUG_GOALKEEPER_STATE_MACHINE
	cout << "[Goalkeeper]::DoStateGoBackToGoalCenter\n";
#endif

	positionLocation.x = GOALKEEPER_POSITION_X;
	positionLocation.y = GOALKEEPER_POSITION_Y;
	m_vision->SetHeadTrackingStatus(GOAL_TRACKING);

	while (!m_goTo->QuickAndDirty(positionLocation.x, positionLocation.y, 0,
			false)) {
		if (!IsStatePlaying()) {
			m_motion->StopWalking();
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}

		usleep(GOALKEEPER_ITERATION_TIME);
	}

	m_vision->SetHeadTrackingStatus(BALL_TRACKING);
	return STATE_POSITION_YOURSELF;
}

bool Goalkeeper::ShouldIGoToBall() {
	//bool shouldIGoToBall;

	BallLocation firstStrikerBallLocation = m_communication->GetFriendData(
			m_firstStrikerId)->ballLocation;
	BallLocation secondStrikerBallLocation = m_communication->GetFriendData(
			m_secondStrikerId)->ballLocation;
	BallLocation myBallLocation;

	// No communication
	if (!IsCommunicationOn()) {
#ifdef DEBUG_GOALKEEPER_SHOULD_I_GO_TO_BALL
		cout
				<< "[Goalkeeper::ShouldIGoToBall]: No communication - I'm going to the ball\n";
#endif
		return true;
	}

	myBallLocation = m_listCombinedData->m_head->ballLocation;

	if (IsBallDataRelevant(&firstStrikerBallLocation)) {
		if (myBallLocation.distanceFromMe
				> firstStrikerBallLocation.distanceFromMe
						- GOALKEEPER_SHOULD_I_GO_TO_BALL_DISTANC_OFFSET) {
#ifdef DEBUG_GOALKEEPER_SHOULD_I_GO_TO_BALL
			cout
					<< "[Goalkeeper::ShouldIGoToBall]: striker 1 will go to the ball\n";
#endif
			return false;
		}
	}

	if (IsBallDataRelevant(&secondStrikerBallLocation)) {
		if (myBallLocation.distanceFromMe
				> secondStrikerBallLocation.distanceFromMe
						- GOALKEEPER_SHOULD_I_GO_TO_BALL_DISTANC_OFFSET) {
#ifdef DEBUG_GOALKEEPER_SHOULD_I_GO_TO_BALL
			cout
					<< "[Goalkeeper::ShouldIGoToBall]: striker 2 will go to the ball\n";
#endif
			return false;
		}
	}

#ifdef DEBUG_GOALKEEPER_SHOULD_I_GO_TO_BALL
	cout << "[Goalkeeper::ShouldIGoToBall]: I'm going to the ball\n";
#endif
	// the goalie will send the ball away
	return true;
}

void Goalkeeper::EraseFalseBalls() {
	CombinedData_t* combinedDataIterator = m_listCombinedData->m_head;
	//int numOfCombinedData = m_listCombinedData->GetNumberOfCombinedDatas();
	CombinedData_t* nextDataIterator = combinedDataIterator->m_next;

	while (nextDataIterator != NULL) {
		if (combinedDataIterator->ballLocation.time
				- nextDataIterator->ballLocation.time < 100 * 1000) {
			if (nextDataIterator->ballLocation.distanceFromMe
					> combinedDataIterator->ballLocation.distanceFromMe + 150) {
				cout << "erasing false ball. ball dist = "
						<< nextDataIterator->ballLocation.distanceFromMe
						<< "\n";
				m_listCombinedData->RemoveCombinedData(nextDataIterator);
			}
		}

		combinedDataIterator = combinedDataIterator->m_next;
		nextDataIterator = combinedDataIterator->m_next;
	}

}
