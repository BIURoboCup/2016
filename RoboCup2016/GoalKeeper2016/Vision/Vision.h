#include "findGate.h"

#pragma once

class Vision
{
public:
	Vision();
	~Vision();

	DetectedObject* SafeGetDetectedBall();
	DetectedObject* SafeGetDetectedGate();

private:
	void RunVisionThread();

	DetectedObject* FindBall(Mat& image);
	DetectedObject* FindGate(Mat& image);
};

