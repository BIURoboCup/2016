#include "MotionIncludes.h"

#pragma once

struct S_HeadTilt
{
	// Tilt is up-down. Pan is right-left.
	int Tilt;
	int Pan;
};

class Motion
{
public:
	Motion();
	~Motion();

	void WalkStraight(int cmToWalk);
	
	void SetHeadTilt(S_HeadTilt headTilt);
	S_HeadTilt GetHeadTilt();

	// Returns true as long as any motion is being done
	bool IsRunning();
	void WaitForActionFinish();
	void FreeAllEngines();

private:
	LinuxCM730* m_linuxCM730;
	CM730* m_cm730;
};


