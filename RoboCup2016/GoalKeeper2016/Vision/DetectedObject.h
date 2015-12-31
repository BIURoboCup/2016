#include "VisionUtils.h"

// Abstract class

class DetectedObject
{
public:
	Point2i Center;
	float Distance;

	virtual bool IsDetected() = 0;
	virtual void Draw(Mat& image) = 0;
};