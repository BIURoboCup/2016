/*
 * HeadMotion.h
 *
 *  Created on: Jan 5, 2012
 *      Author: Alon King
 */

#ifndef HEADMOTION_H_
#define HEADMOTION_H_

#include "IncludesFile.h"
#include "VisionEnums.h"
#include "Head.h"
#include "Camera.h"
#include "WalkingAndActions.h"

using namespace Robot;

//#define MIN_HEAD_RESPONSE_TIME 		0//12500

#define NOT_KICKING_NUM_OF_REFINEMENT_POINTS 	48//48
#define KICKING_NUM_OF_REFINEMENT_POINTS 		24//48
#define NOT_KICKING_MODE_PAN_RANGE 		70.0
#define NOT_KICKING_MODE_TILT_UP_RANGE 	55.0
#define NOT_KICKING_MODE_TILT_DOWN_RANGE 8.0

#define KICKING_MODE_PAN_RANGE 			40.0
#define KICKING_MODE_TILT_UP_RANGE 		0.0
#define KICKING_MODE_TILT_DOWN_RANGE 	25.0

class HeadMotion {
public:
	Head* m_headInstance;

private:

	CM730* m_cm730;
	double m_destinationPanAngle;
	double m_destinationTiltAngle;

	double m_headScanPanRange;
	double m_headScanTiltDownRange;
	double m_headScanTiltUpRange;
	int m_numberOfRefinementPoints;

	int m_noBallCount;
	static const int m_noBallMaxCount = 5;

	int m_currentHeadIteration;
	int m_rotation;

	double m_offsetY;

public:

	HeadMotion();
	void Initialize();
	void MoveToHome();
	double GetPanAngle();
	double GetTiltAngle();

	void MoveToPoint(Point2D &centerPoint);

	void XYToPanTilt(Point2D &point, double& pan, double& tilt);

	void MoveByAngleOffset(double AlphaOffsetPan, double AlphaOffsetTilt);
	void MoveByAngle(double AlphaOffsetPan, double AlphaOffsetTilt);
	void MoveTracking();
	void MoveTracking(Point2D& ballCenter, bool ballFound);
	bool IsHeadOffsetToKick();
	double GetHeadYOffset();

	void UpdateHeadScanForBallParameters(
			HeadTrackingStatus_e newHeadTrackingStatus);

	void HeadScan(double panAngleForLastBallPosition,
			double tiltAngleForLastBallPosition, int rotation);
	void HeadScan();
	void ScanIteration();

	void MakeUpCM730Delay();

	~HeadMotion();
};

#endif /* HEADMOTION_H_ */
