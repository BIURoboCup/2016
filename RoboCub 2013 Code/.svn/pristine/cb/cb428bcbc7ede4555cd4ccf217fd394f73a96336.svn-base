#ifndef GOALKEEPER_H_
#define GOALKEEPER_H_

#include "AI/AI.h"
#include "GoalkeeperEnums.h"
#include "AI/ListCombinedData.h"
#include "Objects/Trendline.h"
#include "GoalkeeperDefines.h"

#define  SIZE_OF_GOAL_IN_CM 160

class Goalkeeper: public AI {
public:

	static Goalkeeper* GetInstance();
	virtual ~Goalkeeper();

private:
	static Goalkeeper* m_uniqueInstance;

	Goalkeeper();
	BallLocation m_ballLocation;
	Location* m_myLocationPtr;
	ListCombinedData* m_listCombinedData;

	Trendline m_ballTrendlineY;
	Trendline m_ballTrendlineX;
	JumpSide_e m_jumpSide;

	StepSide_e m_stepSide;
	int m_turnCount;

	myTimeval m_currentTime;

	int m_firstStrikerId;
	int m_secondStrikerId;

	State_t virtual RunState(State_t cur_state);
	State_t virtual DoStateGameControllerReady();

	State_t DoStateInitialize();
	State_t DoStatePositionYourself();
	State_t DoStateSendTheBallAway();
	State_t DoStateJumpToSave();
	State_t DoStateTurnToMiddle();
	State_t DoStateGoBackToGoalCenter();
	State_t DoStatePenaltyMode();

	double CalculateEuclideanDistance(Point2DInt_s* pointA,
			const Point2DInt_s* pointB);
	bool IsBallInTheDangerZone();
	bool IsBallStaticInRecentFrames();
	JumpSide_e IsBallComing();

	void UpdateBallBuffer();
	void EraseFalseBalls();

	StepSide_e GetStepSide();
	void DoGoalkeeperStep(StepSide_e stepSide);
	bool ShouldIGoToBall();
};

#endif /* GOALKEEPER_H_ */

