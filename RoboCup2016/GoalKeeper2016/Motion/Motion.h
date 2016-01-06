#include "MotionIncludes.h"

#pragma once

struct S_HeadTilt
{
	// Tilt is up-down. Pan is right-left.
	int TiltAngle;
	int PanAngle;
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
};


