#ifndef DETECTEDBALL_H_
#define DETECTEDBALL_H_

#include "DetectedObject.h"

class DetectedBall : public DetectedObject{
public:
	float Radius;

	DetectedBall();
	DetectedBall(Point2f center, float radius, bool isFound);
	~DetectedBall();

	virtual bool IsDetected();
	virtual void Draw(Mat& image);

private:
	bool m_isFound;
};

#endif /* DETECTEDBALL_H_ */
