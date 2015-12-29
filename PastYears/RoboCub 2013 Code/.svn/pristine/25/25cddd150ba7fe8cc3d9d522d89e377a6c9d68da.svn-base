#ifndef AI_H
#define AI_H

#include "IncludesFile.h"
#include "General/MyThreadClass.h"
#include "Structs/XYLocation.h"
#include "GoTo.h"
#include "Communication.h"
#include "Localization.h"
#include "Vision.h"

#define MAX_ANGLE_FOR_KICK 28
#define MIN_ANGLE_FOR_KICK 18

namespace Robot {
typedef enum State_e {
	STATE_HANDLE_GAME_CONTROLLER_STATES,
	STATE_INITIALIZE,
	STATE_LOOK_FOR_THE_BALL,
	STATE_SHOULD_I_GO_TO_THE_BALL,
	STATE_POSITION_YOURSELF,
	STATE_GO_TO_THE_BALL,
	STATE_IS_IT_POSSIBLE_TO_KICK,
	STATE_KICK,
	STATE_DRIBBLE,
	STATE_PASS_AWAY,
	STATE_JUMP_TO_SAVE,
	STATE_SEND_THE_BALL_AWAY,
	STATE_TURN_TO_MIDDLE,
	STATE_GO_BACK_TO_GOAL_CENTER,
	STATE_GAME_CONTROLLER_INITIAL,
	STATE_GAME_CONTROLLER_READY,
	STATE_GAME_CONTROLLER_SET,
	STATE_GAME_CONTROLLER_PLAYING,
	STATE_GAME_CONTROLLER_FINISHED,
	STATE_PENALTY_MODE,
	STATE_UNKNOWN,
	NUM_STATES
} State_t;
typedef struct instance_data instance_data_t;
}

class AI: public MyThreadClass {
	// members
protected:
	Vision *m_vision;
	Localization *m_localization;
	Communication *m_communication;
	WalkingAndActions *m_motion;
	GoTo *m_goTo;
	XYLocation_s m_ballLocationFromFriends;

	bool IsStateInitial();
	bool IsStateReady();
	bool IsStateSet();
	bool IsStatePlaying();
	bool IsStateFinished();
	bool IsCommunicationOn();

	State_t DoStateHandleGameControllerStates();

	State_t virtual DoStateGameControllerInitial();
	State_t virtual DoStateGameControllerSet();
	State_t virtual DoStateGameControllerReady() = 0;
	State_t virtual DoStateGameControllerPlaying();
	State_t virtual DoStateGameControllerFinished();

	State_t virtual RunState(State_t cur_state) = 0;
	string virtual PrintState(State_t state);
	string PrintStateGameController(int state);

public:

	AI();
	virtual ~AI();
	void virtual InternalThreadFunc();
	XYLocation_t GetBallLocation();
	XYLocation_t GetMyLocation();
	void AdjustForAKickAndKick();
	void AdjustYourselfTowardTheBallForDribble();

	void Kick();

};

#endif
