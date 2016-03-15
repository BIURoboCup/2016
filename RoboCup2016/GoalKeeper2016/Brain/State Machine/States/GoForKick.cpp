#include "GoForKick.h"

GoForKick::GoForKick()
{

}

GoForKick::~GoForKick()
{

}

void GoForKick:: Play()
{
	if (1) //ball1.center.y>360
	{
		std::string g = "State == GoForKick y=";
		stringstream p;
		p << (int) ball1.center.y;
		g += p.str();
		writeToLog(logFile, g);

		if (ball1.x_ball_spot > 3 && ball1.x_ball_spot < 7) {
			GoTo(0, StateList[State].stateString, ball1.x_ball_spot); //forward
			writeToLog(logFile, "GoForKick forward");
		} else if (ball1.x_ball_spot < 4) {
			GoTo(6, StateList[State].stateString, ball1.x_ball_spot);  //right
			writeToLog(logFile, "GoForKick right");
		} else if (ball1.x_ball_spot > 6) {
			GoTo(-6, StateList[State].stateString, ball1.x_ball_spot);  //left
			writeToLog(logFile, "GoForKick left");
		}

		sleep(4);

		GoTo(0, StateList[State].stateString, ball1.x_ball_spot);

		//				if (ball1.center.y<420)
		//					sleep(15);
		//				sleep(11)
		sleep(20);

		GoStop();
		writeToLog(logFile, "Kick");

		Action::GetInstance()->Start(4);
		while (Action::GetInstance()->IsRunning())
			usleep(8 * 1000);
	}

	State = GoToBall;

}





