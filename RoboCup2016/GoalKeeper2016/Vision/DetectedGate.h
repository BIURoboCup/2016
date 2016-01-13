#include "DetectedObject.h"

#pragma once

enum E_DetectedPost
{
	E_DetectedPost_LEFT = 0,
	E_DetectedPost_RIGHT = 1,
	E_DetectedPost_BOTH = 2,
	E_DetectedPost_NONE = 3
};

class DetectedGate : public DetectedObject
{
public:
	RotatedRect LeftPost;
	RotatedRect RightPost;
	E_DetectedPost DetectedPost;

	virtual bool IsDetected();
	virtual void Draw(Mat& image);

	DetectedGate();
	~DetectedGate();

	DetectedGate(RotatedRect post1, RotatedRect post2);
	DetectedGate(RotatedRect post, E_DetectedPost detectedPost);
};



