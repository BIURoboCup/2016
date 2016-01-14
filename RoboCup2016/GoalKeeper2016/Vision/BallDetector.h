#include "DetectedBall.h"

#ifndef BALLDETECTOR_H_
#define BALLDETECTOR_H_

class BallDetector {
public:
	BallDetector();
	~BallDetector();

	DetectedObject* FindBall(Mat& inputImage);

	static inline DetectedObject* GetDefault() {return new DetectedBall();}
};

#endif /* BALLDETECTOR_H_ */
