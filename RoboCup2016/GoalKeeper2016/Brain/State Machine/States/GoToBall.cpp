#include "GoToBall.h"

GoToBall::GoToBall()
{

}

GoToBall::~GoToBall()
{

}

void GoToBall:: Play()
{
	writeToLog(logFile, "State == GoToBall");

	int dir = ball1.x_ball_spot;

	if (ball1.found == false) {
		++counter_ball_not_found;
		writeToLog(logFile, "counter_ball_not_found value is",
				convertIntToString(counter_ball_not_found));
		if (counter_ball_not_found == 3) {
			State = LookForBall;
		}
	} else if (HeadPosition == LookingStraight) {
		counter_ball_not_found = 0;
		if (ball1.x_ball_spot > 3 && ball1.x_ball_spot < 7) {
			GoTo(0, StateList[State].stateString, ball1.x_ball_spot); //forward
		} else if (ball1.x_ball_spot < 4) {
			GoTo(dir, StateList[State].stateString, ball1.x_ball_spot);  //left
		} else if (ball1.x_ball_spot > 6) {
			GoTo(-(dir % 5), StateList[State].stateString, ball1.x_ball_spot); //Right
		}
		writeToLog(logFile, "GoToBall - LookingStraight");
	} else if (HeadPosition == LookingStraightDown) {
		counter_ball_not_found = 0;
		if (ball1.center.y < 360) {
			if (ball1.x_ball_spot > 3 && ball1.x_ball_spot < 7) {
				GoTo(0, StateList[State].stateString, ball1.x_ball_spot); //forward
			} else if (ball1.x_ball_spot < 4) {
				GoTo(dir, StateList[State].stateString, ball1.x_ball_spot); //left
			} else if (ball1.x_ball_spot > 6) {

				GoTo(-(dir % 5), StateList[State].stateString,
						ball1.x_ball_spot);  //right
			}
			writeToLog(logFile, "GoToBall - LookingStraightDown >360");
		} else {
			std::string g = "Next State: GoForKick y=";
			stringstream p;
			p << (int) ball1.center.y;
			g += p.str();
			writeToLog(logFile, g);
			State = GoForKick;
		}
	} else if (HeadPosition == LookingRight) {
		counter_ball_not_found = 0;
		if (ball1.x_ball_spot < 6) {
			GoTo(-(dir + 1), StateList[State].stateString, ball1.x_ball_spot);
		} else if (ball1.x_ball_spot > 5) {
			GoTo(-dir, StateList[State].stateString, ball1.x_ball_spot);
		}

		SetTilt(0, -15);
		HeadPosition = LookingStraight;
		writeToLog(logFile, "HeadPosition = LookingStraight");
		sleep(1);
	} else if (HeadPosition == LookingLeft) {
		counter_ball_not_found = 0;
		if (ball1.x_ball_spot < 6) {
			GoTo(8 - dir, StateList[State].stateString, ball1.x_ball_spot);
		} else if (ball1.x_ball_spot > 5) {
			GoTo(5 - (dir % 5), StateList[State].stateString,
					ball1.x_ball_spot);
		}

		SetTilt(0, -15);
		HeadPosition = LookingStraight;
		writeToLog(logFile, "HeadPosition = LookingStraight");
		sleep(1);
	}
	//*/
	sleep(1);

}




