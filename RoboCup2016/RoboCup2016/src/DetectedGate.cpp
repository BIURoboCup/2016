#include "DetectedGate.h"


DetectedGate::DetectedGate()
{
}

DetectedGate::~DetectedGate()
{
}

void DetectedGate::InitializeFromTwoPosts(RotatedRect post1, RotatedRect post2)
{
	DetectedPost = E_DetectedPost_BOTH;

	LeftPost = (post1.center.x < post2.center.x) ? post1 : post2;
	RightPost = (post1.center.x > post2.center.x) ? post1 : post2;
	Center.x = (RightPost.center.x + LeftPost.center.x) / 2;
	Center.y = (RightPost.center.y + LeftPost.center.y) / 2;	
}

void DetectedGate::InitializeFromOnePost(RotatedRect post, E_DetectedPost detectedPost)
{
	DetectedPost = detectedPost;

	if (DetectedPost == E_DetectedPost_LEFT)
	{
		LeftPost = post;
	}
	else
	{
		RightPost = post;
	}
}

void DetectedGate::SetNotDetected()
{
	DetectedPost = E_DetectedPost_NONE;
}
