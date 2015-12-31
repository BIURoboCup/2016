#include "DetectedGate.h"

DetectedGate::DetectedGate(RotatedRect post1, RotatedRect post2)
{
	DetectedPost = E_DetectedPost_BOTH;

	LeftPost = (post1.center.x < post2.center.x) ? post1 : post2;
	RightPost = (post1.center.x > post2.center.x) ? post1 : post2;
	Center.x = (RightPost.center.x + LeftPost.center.x) / 2;
	Center.y = (RightPost.center.y + LeftPost.center.y) / 2;
}

DetectedGate::DetectedGate(RotatedRect post, E_DetectedPost detectedPost)
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

DetectedGate::DetectedGate()
{
	DetectedPost = E_DetectedPost_NONE;
}

DetectedGate::~DetectedGate()
{
}

bool DetectedGate::IsDetected()
{
	return DetectedPost != E_DetectedPost_NONE;
}

void DetectedGate::Draw(Mat& image)
{
	if (DetectedPost == E_DetectedPost_BOTH)
	{
		DrawRectangle(image, LeftPost);
		DrawRectangle(image, RightPost);
		DrawPoint(image, Center);
	}
	if (DetectedPost == E_DetectedPost_LEFT)
	{
		DrawRectangle(image, LeftPost);
	}
	if (DetectedPost == E_DetectedPost_RIGHT)
	{
		DrawRectangle(image, RightPost);
	}
}
