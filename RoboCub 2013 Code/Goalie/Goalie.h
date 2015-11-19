/*
 * Communication.h
 *
 *  Created on: 5 1 2012
 *      Author: Amir&Sarit
 */

#ifndef GOALIE_H_
#define GOALIE_H_
#define A 4
#define REHAVA_LEFT -150
#define REHAVA_RIGHT 150
#define REHAVA_FRONT 100
#define GOALIE_POSITION_X (0)
#define GOALIE_POSITION_Y (10)

#include "IncludesFile.h"
#include "General/MyThreadClass.h"
#include "Structs/XYLocation.h"
#include "GoTo.h"
#include "Communication.h"
#include "Localization.h"

class Goalie : public MyThreadClass
{
public:
	static Goalie*	m_uniqueInstance;
	Localization* m_localization;
	WalkingAndActions* m_motion;
	Communication* m_communication;
	GoTo* m_goTo;
	void InternalThreadFunc();
	void GoalieMain();
	static Goalie* GetInstance();
	virtual ~Goalie();

private:

	Goalie();

	void DoStateGameControllerReady();
//	State_t DoStateInitialize();
//	State_t DoStateLookForTheBall();
//	State_t DoStatePositionYourself();
//	State_t DoStateSendTheBallAway();
//	State_t DoStateJumpToSave();

	list<BallLocation> _mylist;
	list<BallLocation>::iterator _it;
	list<Location> _myPos;
	list<Location>::iterator _itMy;
	bool _two_pic_row;
	float _new_X;
	float _new_Y;
	int print_n;
	float _OLD_X;
	Location _my_pos;
	bool ballmoved();
	void JumpToSave();
	void MoveToNewRange();
	void KickTheBall();
	double GetAttackerDistanceFromTheBall();
	void DirectionOfTheKick();
	bool IncomeKickToGoal();
	bool CloserThenAttacker();
	bool CheckDistanceFromBall();

};

#endif /* GOALIE_H_ */

