#ifndef STRIKER_H
#define STRIKER_H

#include "AI/AI.h"
#include "Vision.h"

//#define SLOWING_DOWN_DISTANCE (18)

// X positioning
#define POSITION_IN_LEFT_SIDE    (-50)
#define POSITION_IN_RIGHT_SIDE   (50)

/*

___________|_________|____________
|				|				|
|       1       |       2       |
|				|				|
|				|				|
|       3       |       4       |
|				|				|
|				|				|
|       5       |       6       |
|				|				|
|				|				|
|       7       |       8       |
|_______________|_______________|
           |          |

*/

//Y posistioning
#define POSITION_Y_LOW_FIELD      (80)
#define POSITION_Y_MID_LOW_FIELD  (150)
#define POSITION_Y_MID_HIGH_FIELD (300)
#define POSITION_Y_HIGH_FIELD     (450)
#define BALL_LOCATION_IS_FAR (200)
#define MAX_VALID_TIME_BALL_LOCATION_FROM_FRIEND (MAX_VALID_TIME_BALL_LOCATION)
#define MAX_VALID_DISTANCE_BALL_LOCATION_FROM_FRIEND (250)
#define MAX_TIME_GO_TOWARDS_BALL (3 * 1000 * 1000)
#define DIFF_DISTANCE_FROM_BALL_X (100)
#define DIFF_DISTANCE_FROM_BALL_Y (100)
#define DANGER_ZONE_DISTANCE_Y (180)
#define X_HALF_FIELD_COORDINATE (0)
#define Y_HALF_FIELD_COORDINATE (300)
#define Y_MAX_DANGER_ZONE_DONT_DRIBBLE (200)
#define Y_LOW_FIELD_COORDINATE  (150)
#define Y_MID_LOW_FIELD_COORDINATE   (300)
#define Y_MID_HIGH_FIELD_COORDINATE  (450)
#define OPPONENT_GOAL_X (0)
#define OPPONENT_GOAL_Y (600)
#define X_BAR_RIGHT_DEFENCE (80)
#define X_BAR_LEFT_DEFENCE (-80)
#define KICKABLE_DISTANCE_FROM_OPPONENT_GOAL (300)
#define VALID_ANGLE_FOR_DRIBBLE (30)
#define DISTANCE_FROM_BALL_THRESHOLD (5)
#define DRIBBLE_X_SPEED (18)
#define LOOK_FOR_BALL_TURNNING_ANGLE_SPEED (MAX_TURN_LEFT_SPEED)

#define MAX_TIME_TO_WAIT_KICKOFF 									(10) // after being untouched for 10 seconds the non kicking team can go to the ball
#define MIN_NUM_OF_BALLS_FOR_STATIC_BALL_DECISION_STATE_INITIALIZE 	(6)
#define MAX_DIFF_TO_START_PLAY_STATE_INITIALIZE						(10)

#define LEFT_TURN (true)

#define DEFENSIVE_POSITION_STRIKER1_X (0)
#define DEFENSIVE_POSITION_STRIKER1_Y (220)
#define DEFENSIVE_POSITION_STRIKER2_X (50)
#define DEFENSIVE_POSITION_STRIKER2_Y (140)

#define OFFENSIVE_POSITION_STRIKER1_X (0)
#define OFFENSIVE_POSITION_STRIKER1_Y (260)
#define OFFENSIVE_POSITION_STRIKER2_X (60)
#define OFFENSIVE_POSITION_STRIKER2_Y (200)

#define LOOK_FOR_BALL_POSITION1_STRIKER1_X (100)
#define LOOK_FOR_BALL_POSITION1_STRIKER1_Y (450)
#define LOOK_FOR_BALL_POSITION2_STRIKER1_X (100)
#define LOOK_FOR_BALL_POSITION2_STRIKER1_Y (150)

#define LOOK_FOR_BALL_POSITION1_STRIKER2_X (-100)
#define LOOK_FOR_BALL_POSITION1_STRIKER2_Y (150)
#define LOOK_FOR_BALL_POSITION2_STRIKER2_X (-100)
#define LOOK_FOR_BALL_POSITION2_STRIKER2_Y (450)


//typedef State_t Striker::state_func_t( instance_data_t *data );

extern int myTeamNumber;

class Striker : public AI
{
	// members
private:
	static Striker*	m_uniqueInstance;
	
	int m_goalieId;
	int m_otherStrikerId;
	bool m_didIGoLastTime;
	bool m_isLastCommunicationOn;
	void GoToBall();

	State_t RunState( State_t cur_state );
	State_t virtual DoStateGameControllerReady();

	bool ShouldIGoToBall();
	XYLocation_t CalculateBallLocationFromFriends();

	State_t DoStateInitialize();
	State_t DoStateLookForTheBall();
	State_t DoStateShouldIGoToTheBall();
	State_t DoStateGoToTheBall();
	State_t DoStatePositionYourself();
	State_t DoStateIsItPossibleToKick();
	State_t DoStateKick();
	State_t DoStateDribble();
	State_t DoStatePassAway();



public:
	// methods
	Striker();
	virtual ~Striker();
	static Striker*	GetInstance();

	bool IsIBlocked();
	cm CalculateDistance(cm fromXLocation, cm fromYLocation, cm toXLocation, cm toYLocation);
	cm CalculateDistanceFromMe(cm xLocation, cm yLocation);
	cm CalculateDistanceFromOtherStriker(cm xLocation, cm yLocation);

};

#endif
