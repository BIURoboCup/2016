#ifndef DETECTEDOBJECT_H_
#define DETECTEDOBJECT_H_

#include "VisionUtils.h"

// Abstract class


class DetectedObject
{
public:
	Point2i Center;
	float Distance;

	virtual bool IsDetected() = 0;
	virtual void Draw(Mat& image) = 0;

	DetectedObject();
	virtual ~DetectedObject();
};

#endif /* DETECTEDOBJECT_H_ */
