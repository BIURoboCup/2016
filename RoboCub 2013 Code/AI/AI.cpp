#include "AI.h"

///#define DEBUG_AI_STATE_MACHINE
#define DEBUG_AI_KICK
#define DEBUG_ADJUST_YOURSELF_AND_KICK

AI::AI() {
	cout << "AI::AI()\n";
	m_vision = Vision::GetInstance();
	m_localization = Localization::GetInstance();
	m_communication = Communication::GetInstance();
	m_motion = WalkingAndActions::GetInstance();
	m_goTo = GoTo::GetInstance();
	cout << "AI::AI() completed.\n";
}

AI::~AI() {
}

bool AI::IsStateInitial() {
	return (m_communication->GetMyState() == STATE_INITIAL);
}

bool AI::IsStateReady() {
	return (m_communication->GetMyState() == STATE_READY);
}

bool AI::IsStateSet() {
	return (m_communication->GetMyState() == STATE_SET);
}

bool AI::IsStatePlaying() {
	return (m_communication->GetMyState() == STATE_PLAYING);
}

bool AI::IsStateFinished() {
	return (m_communication->GetMyState() == STATE_FINISHED);
}

bool AI::IsCommunicationOn() {
	return m_communication->GetCommunicationFlag();
}

State_t AI::DoStateGameControllerInitial() {
	m_goTo->Stop();
	m_motion->StopWalking();
	m_motion->StartAction(SIT_DOWN);
	m_motion->StartAction(STAND_UP);
	m_vision->SetHeadTrackingStatus(GOAL_TRACKING);

	while (exitProgram == false) {
		if (!IsStateInitial()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}
		usleep(100 * 1000);
	}
	return STATE_HANDLE_GAME_CONTROLLER_STATES;
}

State_t AI::DoStateGameControllerSet() {
	m_goTo->Stop();
	m_motion->StopWalking();
	m_motion->StartAction(STAND_UP);
	m_vision->SetHeadTrackingStatus(BALL_TRACKING);

	while (exitProgram == false) {
		if (!IsStateSet()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}
		usleep(100 * 1000);
	}
	return STATE_HANDLE_GAME_CONTROLLER_STATES;
}

State_t AI::DoStateGameControllerPlaying() {
	return STATE_INITIALIZE;
}

State_t AI::DoStateGameControllerFinished() {
	m_goTo->Stop();
	m_motion->StopWalking();
	m_motion->StartAction(SIT_DOWN);
	while (exitProgram == false) {
		if (!IsStateFinished()) {
			return STATE_HANDLE_GAME_CONTROLLER_STATES;
		}
		usleep(100 * 1000);
	}
	return STATE_HANDLE_GAME_CONTROLLER_STATES;

}

State_t AI::DoStateHandleGameControllerStates() {

	State_t nextState = STATE_UNKNOWN;

#ifdef DEBUG_AI_STATE_MACHINE
	cout << "[AI]::DoStateHandleGameControllerStates\n";
#endif

	switch (m_communication->GetMyState()) {
	case STATE_INITIAL:
		nextState = STATE_GAME_CONTROLLER_INITIAL;
		break;
	case STATE_READY:
		nextState = STATE_GAME_CONTROLLER_READY;
		break;
	case STATE_SET:
		nextState = STATE_GAME_CONTROLLER_SET;
		break;
	case STATE_PLAYING:
		nextState = STATE_GAME_CONTROLLER_PLAYING;
		break;
	case STATE_FINISHED:
		nextState = STATE_GAME_CONTROLLER_FINISHED;
		break;
	default:
		cout
				<< "[DoStateHandleGameControllerStates]: WARNING: Unknown State number "
				<< m_communication->GetMyState() << "!\n";
		throw "[DoStateHandleGameControllerStates]: WARNING: Unknown State number !\n";
		break;
	}

	return nextState;
}

string AI::PrintStateGameController(int state) {
	switch (state) {
	case STATE_INITIAL:
		return "DoStateGameControllerInitial";
	case STATE_READY:
		return "DoStateGameControllerReady";
	case STATE_SET:
		return "DoStateGameControllerSet";
	case STATE_PLAYING:
		return "DoStateGameControllerPlaying";
	case STATE_FINISHED:
		return "DoStateGameControllerFinished";
	default:
		return "Unknown Game Controller state";
	}
}

void AI::InternalThreadFunc() {
	cout << "AI Internal Thread. tid = " << (long int) syscall(224) << "\n";
	//State_t nextState = STATE_INITIALIZE; // TODO: change this to handle game controller states
	State_t nextState = STATE_HANDLE_GAME_CONTROLLER_STATES;
	try {
		while (exitProgram == false) {
			nextState = RunState(nextState);
			//cout << PrintStateGameController(m_communication->GetMyState()) << "\n";
		}
	} catch (char const* str) {
		cout << "[AI]: Exception raised: " << str << '\n';
	}

	m_goTo->Stop();

	cout << "AI Thread exited\n";
}
string AI::PrintState(State_t state) {
	switch (state) {
	case STATE_INITIALIZE:
		return "DoStateInitialize";
	case STATE_LOOK_FOR_THE_BALL:
		return "DoStateLookForTheBall";
	case STATE_GO_TO_THE_BALL:
		return "DoStateGoToTheBall";
	case STATE_SHOULD_I_GO_TO_THE_BALL:
		return "DoStateShouldIGoToTheBall";
	case STATE_IS_IT_POSSIBLE_TO_KICK:
		return "DoStateIsItPossibleToKick";
	case STATE_KICK:
		return "DoStateKick";
	case STATE_DRIBBLE:
		return "DoStateDribble";
	case STATE_PASS_AWAY:
		return "DoStatePassAway";
	case STATE_POSITION_YOURSELF:
		return "DoStatePositionYourself";
	case STATE_JUMP_TO_SAVE:
		return "DoStateJumpToSave";
	case STATE_SEND_THE_BALL_AWAY:
		return "DoStateSendTheBallAway";
	case STATE_TURN_TO_MIDDLE:
		return "DoStateTurnToMiddle";
	case STATE_HANDLE_GAME_CONTROLLER_STATES:
		return "DoStateHandleGameControllerStates";
	case STATE_GAME_CONTROLLER_INITIAL:
		return "DoStateGameControllerInitial";
	case STATE_GAME_CONTROLLER_SET:
		return "DoStateGameControllerSet";
	case STATE_GAME_CONTROLLER_READY:
		return "DoStateGameControllerReady";
	case STATE_GAME_CONTROLLER_PLAYING:
		return "DoStateGameControllerPlaying";
	case STATE_GAME_CONTROLLER_FINISHED:
		return "DoStateGameControllerFinished";
	default:
		return "Unknown state";
	}
}

void AI::AdjustForAKickAndKick() {
	//Assumption: The robot is close to the ball.
	BallLocation ballLocation = m_localization->GetBallLocation();

#ifdef DEBUG_ADJUST_YOURSELF_AND_KICK
	cout << "[AI::Kick]: ball Angle = " << ballLocation.angleFromMe << "\n";
#endif

	if (IsBallDataRelevant(&ballLocation)) {
		if (ballLocation.angleFromMe > 27) {
#ifdef DEBUG_ADJUST_YOURSELF_AND_KICK
			cout << "[AI::Kick]: Hard Right\n";
#endif
			m_motion->StartAction(STEP_RIGHT);
		} else if (ballLocation.angleFromMe < -20) {
#ifdef DEBUG_ADJUST_YOURSELF_AND_KICK
			cout << "[AI::Kick]: Hard Left\n";
#endif
			m_motion->StartAction(STEP_LEFT);
		} else if (ballLocation.angleFromMe > -14
				&& ballLocation.angleFromMe < 0) {
#ifdef DEBUG_ADJUST_YOURSELF_AND_KICK
			cout << "[AI::Kick]: Soft Left\n";
#endif
			m_motion->StartAction(STEP_LEFT);
		} else if (ballLocation.angleFromMe < 18
				&& ballLocation.angleFromMe >= 0) {
#ifdef DEBUG_ADJUST_YOURSELF_AND_KICK
			cout << "[AI::Kick]: Soft Right\n";
#endif
			m_motion->StartAction(STEP_RIGHT);
		}

		usleep(500 * 1000);
		ballLocation = m_localization->GetBallLocation();
#ifdef DEBUG_ADJUST_YOURSELF_AND_KICK		
		cout << "[AI]::kick ball angle = " << ballLocation.angleFromMe << "\n";
#endif
		if (ballLocation.angleFromMe < 0) {
			m_motion->StartAction(KICK_LEFT);
		} else {
			m_motion->StartAction(KICK_RIGHT);
		}
	}
#ifdef DEBUG_WALKING_AND_ACTIONS_KICK_ON
	cout << "[WalkingAndActions::Kick]: Distance from ball = " << ballLocation->distanceFromMe << "\n";
#endif
}
/*
 void AI::Kick()
 {
 m_vision->SetHeadTrackingStatus(KICKING_BALL_TRACKING);

 BallLocation ballLocation;

 if(!IsStatePlaying() || !IsBallDataRelevant(&ballLocation))
 {


 return;
 }
 else
 {
 #ifdef DEBUG_AI_KICK
 cout << "[AI::Kick]: kicking!. distance = " << ballLocation.distanceFromMe << " angle = " << ballLocation.angleFromMe << "\n";
 #endif
 ballLocation = m_localization->GetBallLocation();
 if (ballLocation.angleFromMe > 0)
 {
 m_motion->StartAction(KICK_RIGHT);
 }
 else
 {
 m_motion->StartAction(KICK_LEFT);
 }
 return;
 }

 #ifdef DEBUG_AI_KICK
 cout << "[AI::Kick]: didn't kick. distance = " << ballLocation.distanceFromMe << " angle = " << ballLocation.angleFromMe << "\n";
 #endif
 }
 */
void AI::AdjustYourselfTowardTheBallForDribble() {
	//Assumption: The robot is close to the ball.
	BallLocation ballLocation = m_localization->GetBallLocation();

	for (int i = 0; i < 3; i++) {
		while ((IsStatePlaying() && m_vision->IsClearToKickDistanceFromBall())
				&& (IsBallDataRelevant(&ballLocation))
				&& (fabs(ballLocation.angleFromMe) > MAX_ANGLE_FOR_KICK
						|| fabs(ballLocation.angleFromMe) < MIN_ANGLE_FOR_KICK)) {
			if (ballLocation.angleFromMe > MAX_ANGLE_FOR_KICK) {
#ifdef DEBUG_ADJUST_YOURSELF
				cout << "[WalkingAndActions::Kick]: Hard Right\n";
#endif
				//m_motion->StartWalkingAsync(-2.5,-20,0);
				m_motion->StartAction(STEP_RIGHT);
				return;
			} else if (ballLocation.angleFromMe < -MAX_ANGLE_FOR_KICK) {
#ifdef DEBUG_ADJUST_YOURSELF
				cout << "[WalkingAndActions::Kick]: Hard Left\n";
#endif
				//m_motion->StartWalkingAsync(-2.5,20,0);
				m_motion->StartAction(STEP_LEFT);
				return;
			} else {
				return;
			}
			ballLocation = m_localization->GetBallLocation();
			usleep(1000 * 50);
		}

		ballLocation = m_localization->GetBallLocation();
		usleep(1000 * 50);
	}
#ifdef DEBUG_WALKING_AND_ACTIONS_KICK_ON
	cout << "[WalkingAndActions::Kick]: Distance from ball = " << ballLocation->distanceFromMe << "\n";
#endif

	m_motion->StopWalking();
}

