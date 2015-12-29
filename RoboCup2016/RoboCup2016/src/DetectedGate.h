#include "Vision.h"

#pragma once

enum E_DetectedPost
{
	E_DetectedPost_LEFT = 0,
	E_DetectedPost_RIGHT = 1,
	E_DetectedPost_BOTH = 2,
	E_DetectedPost_NONE = 3
};

class DetectedGate
{
public:
	Point2i Center;
	RotatedRect LeftPost;
	RotatedRect RightPost;
	E_DetectedPost DetectedPost;	

	void InitializeFromTwoPosts(RotatedRect leftPost, RotatedRect rightPost);
	void InitializeFromOnePost(RotatedRect onePost, E_DetectedPost detectedPost);
	void SetNotDetected();

	DetectedGate();
	~DetectedGate();
};



