#include "MotionIncludes.h"


#define MOTION_FILE_PATH    "/home/robot/Desktop/Walk/Data/motion_4096.bin"
#define INI_FILE_PATH    "/home/robot/Robot1/Data/config.ini"

#pragma once

class HeadTilt
{
public:
	HeadTilt(float tilt, float pan) : Tilt(tilt), Pan(pan) {}

	// Tilt is up-down. Pan is right-left.
	float Tilt;
	float Pan;
};

class Motion
{
public:
	// Singletone
	static Motion* GetInstance();

	~Motion();

	void WalkStraight(int cmToWalk);
	
	void SetHeadTilt(HeadTilt headTilt);
	HeadTilt GetHeadTilt();

	// Returns true as long as any motion is being done
	bool IsActionRunning();
	void WaitForActionFinish();
	void FreeAllEngines();
	void StartEngines();

private:
	Motion();
	static Motion* m_instance;
};


