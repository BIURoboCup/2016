#include "Init.h"

Init::Init()
{

}

Init::~Init()
{

}

void Init:: Play()
{
	writeToLog(logFile, "State == Init");
	Action::GetInstance()->Start(1);
	while (Action::GetInstance()->IsRunning())
		usleep(8 * 1000);
	SetTilt(0, 0);
	HeadPosition = LookingStraight;
	writeToLog(logFile, "HeadPosition = LookingStraight");
	State = LookForBall;
}






