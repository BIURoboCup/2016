/*
 * HeadMotion.cpp
 *
 *  Created on: Jan 5, 2012
 *      Author: Alon King
 */

#include "HeadMotion.h"

#define USLEEP_FOR_ONE_ANGLE__MOVING_HEAD			10

#define MAX(a,b)	( (a) > (b) ? (a) : (b) )
#define MIN(a,b)	( (a) < (b) ? (a) : (b) )
#define ABS(a)		( (a) >= (0) ? (a) : (-a) )

HeadMotion::HeadMotion() {
}

HeadMotion::~HeadMotion() {

}

void HeadMotion::Initialize() {
	m_headInstance = Head::GetInstance();

	m_headInstance->m_Joint.SetEnableHeadOnly(true, true);

	m_headInstance->m_Joint.SetPGain(JointData::ID_HEAD_PAN, 8);
	m_headInstance->m_Joint.SetPGain(JointData::ID_HEAD_TILT, 8);

	m_headInstance->MoveToHome();

	m_cm730 = WalkingAndActions::GetInstance()->GetCM730();
}

void HeadMotion::MoveToHome() {
	m_headInstance->MoveToHome();
}

double HeadMotion::GetPanAngle() {

	int x_ang = 0;

	if (m_cm730->ReadWord(JointData::ID_HEAD_PAN, MX28::P_PRESENT_POSITION_L,
			&x_ang, 0) == CM730::SUCCESS)
		return x_ang * -140.0 / 1519 + 189.17; //PAN LIMITS -> 70,-70
	else
		return -WalkingAndActions::GetInstance()->GetHeadAngleValue(X_ANGLE);

}

double HeadMotion::GetTiltAngle() {
	int y_ang = 0;

	if (m_cm730->ReadWord(JointData::ID_HEAD_TILT, MX28::P_PRESENT_POSITION_L,
			&y_ang, 0) == CM730::SUCCESS)
		return y_ang * 80.0 / 801.0 - 205.87; //TILT LIMITS 55,-25
	else
		return WalkingAndActions::GetInstance()->GetHeadAngleValue(Y_ANGLE);
}

void HeadMotion::MoveToPoint(Point2D& centerPoint) {
	m_headInstance->m_Joint.SetEnableHeadOnly(true, true);

	XYToPanTilt(centerPoint, m_destinationPanAngle, m_destinationTiltAngle);
	m_headInstance->MoveByAngle(m_destinationPanAngle, m_destinationTiltAngle);
}

void HeadMotion::MoveByAngleOffset(double AlphaOffsetPan,
		double AlphaOffsetTilt) {
	m_headInstance->m_Joint.SetEnableHeadOnly(true, true);

	if (ABS(AlphaOffsetTilt) / 2 <= 60 && ABS(AlphaOffsetPan) / 2 <= 75) {
		m_headInstance->MoveByAngleOffset(AlphaOffsetPan * (-1),
				AlphaOffsetTilt * (-1));

//	cout<<"offset MAX(ChangeAngleTilt,ChangeAnglePan) = "<<MAX(ABS(AlphaOffsetPan),ABS(AlphaOffsetTilt))<<"\n";

		usleep(
				(int) (USLEEP_FOR_ONE_ANGLE__MOVING_HEAD
						* MAX(ABS(AlphaOffsetPan),ABS(AlphaOffsetTilt))));
	} else {
		m_headInstance->MoveByAngleOffset(AlphaOffsetPan * (-1),
				AlphaOffsetTilt * (-1));

		//cout<<"angle  = "<<MAX(ChangeAngleTilt,ChangeAnglePan)<<"\n";

		usleep((int) USLEEP_FOR_ONE_ANGLE__MOVING_HEAD * 70);
	}

}

void HeadMotion::MoveByAngle(double AlphaPan, double AlphaTilt) {
	m_headInstance->m_Joint.SetEnableHeadOnly(true, true);

	double ChangeAngleTilt = ABS((GetTiltAngle() - AlphaTilt));
	double ChangeAnglePan = ABS((GetPanAngle() - AlphaPan));

	if (ChangeAngleTilt / 2 <= 60 && ChangeAnglePan / 2 <= 75) {
		m_headInstance->MoveByAngle(AlphaPan, AlphaTilt);

		//cout<<"angle  = "<<MAX(ChangeAngleTilt,ChangeAnglePan)<<"\n";

		usleep(
				(int) (USLEEP_FOR_ONE_ANGLE__MOVING_HEAD
						* MAX(ChangeAngleTilt,ChangeAnglePan)));
	} else {
		m_headInstance->MoveByAngle(AlphaPan, AlphaTilt);

		//cout<<"angle  = "<<MAX(ChangeAngleTilt,ChangeAnglePan)<<"\n";

		usleep((int) USLEEP_FOR_ONE_ANGLE__MOVING_HEAD * 70);
	}

}

void HeadMotion::MoveTracking(Point2D& ballCenter, bool ballFound) {
	if (ballFound == false) {
		if (m_noBallCount < m_noBallMaxCount) {
			m_headInstance->MoveTracking(); // move by the same angle you've moved the last time
			m_noBallCount++;
		} else {
			m_headInstance->InitTracking(); // too many losses, initialize head tracking
		}
	} else {
		m_noBallCount = 0;
		Point2D frameCenter = Point2D(160, 120);
		Point2D offset = ballCenter - frameCenter;
		offset *= -1;
		offset.X *= (Camera::VIEW_H_ANGLE / ((double) 320));
		offset.Y *= (Camera::VIEW_V_ANGLE / ((double) 240));
		m_offsetY = offset.Y;
		//	cout << "Offset Pan by Pixels = " << offset.X << " Offset Y by Pixels = " << offset.Y << endl;
		m_headInstance->MoveTracking(offset);
	}
}

void HeadMotion::MoveTracking() {
	m_headInstance->MoveTracking();
}

bool HeadMotion::IsHeadOffsetToKick() {
	int kickTopAngle = -3;

	if (m_offsetY < kickTopAngle) {
		return true;
	}
	return false;
}

double HeadMotion::GetHeadYOffset() {
	return m_offsetY;
}

void HeadMotion::XYToPanTilt(Point2D &point, double& pan, double& tilt) {
	m_headInstance->m_Joint.SetEnableHeadOnly(true, true);

	pan = (319 - point.X) / 319.0 * 140.0 - 70.0;
	tilt = (239 - point.Y) / 239.0 * 65.0 - 32.5;
}

void HeadMotion::UpdateHeadScanForBallParameters(
		HeadTrackingStatus_e newHeadTrackingStatus) {
	if (newHeadTrackingStatus != KICKING_BALL_TRACKING) {
		m_headScanPanRange = NOT_KICKING_MODE_PAN_RANGE;
		m_headScanTiltDownRange = NOT_KICKING_MODE_TILT_DOWN_RANGE;
		m_headScanTiltUpRange = NOT_KICKING_MODE_TILT_UP_RANGE;
		m_numberOfRefinementPoints = NOT_KICKING_NUM_OF_REFINEMENT_POINTS;
	} else {
		m_headScanPanRange = KICKING_MODE_PAN_RANGE;
		m_headScanTiltDownRange = KICKING_MODE_TILT_DOWN_RANGE;
		m_headScanTiltUpRange = KICKING_MODE_TILT_UP_RANGE;
		m_numberOfRefinementPoints = KICKING_NUM_OF_REFINEMENT_POINTS;
	}
}

void HeadMotion::HeadScan(double panAngleForLastBallPosition,
		double tiltAngleForLastBallPosition, int rotation) {
	int offset;

	if (tiltAngleForLastBallPosition > 0 && panAngleForLastBallPosition > 0) {
		offset = (panAngleForLastBallPosition / m_headScanPanRange)
				* (m_numberOfRefinementPoints / 4);
	} else if (tiltAngleForLastBallPosition < 0
			&& panAngleForLastBallPosition > 0) {
		offset = (-panAngleForLastBallPosition / m_headScanPanRange)
				* (m_numberOfRefinementPoints / 4)
				+ (m_numberOfRefinementPoints) / 2;
	} else if (tiltAngleForLastBallPosition < 0
			&& panAngleForLastBallPosition < 0) {
		offset = (-panAngleForLastBallPosition / m_headScanPanRange)
				* (m_numberOfRefinementPoints / 4)
				+ (m_numberOfRefinementPoints) / 2;
	} else {
		offset = (panAngleForLastBallPosition / m_headScanPanRange)
				* (m_numberOfRefinementPoints / 4);
	}

	m_currentHeadIteration = (offset + m_numberOfRefinementPoints)
			% m_numberOfRefinementPoints;
	m_currentHeadIteration -= 2;
	m_currentHeadIteration = (m_currentHeadIteration)
			% m_numberOfRefinementPoints;
	m_rotation = rotation;
	ScanIteration();
	m_currentHeadIteration = (m_currentHeadIteration + m_rotation
			+ m_numberOfRefinementPoints) % m_numberOfRefinementPoints;

//	usleep(MIN_HEAD_RESPONSE_TIME*10);
}

void HeadMotion::HeadScan() {
	ScanIteration();
	m_currentHeadIteration = (m_currentHeadIteration + m_rotation)
			% m_numberOfRefinementPoints;
}

void HeadMotion::ScanIteration() {
	double pan;
	double tilt;

	if (m_currentHeadIteration >= 0
			&& m_currentHeadIteration <= m_numberOfRefinementPoints / 4) {
		pan = m_headScanPanRange / ((double) m_numberOfRefinementPoints / 4)
				* m_currentHeadIteration;
		tilt = -m_headScanPanRange / ((double) m_numberOfRefinementPoints / 4)
				* m_currentHeadIteration + m_headScanPanRange;
	} else if (m_currentHeadIteration >= m_numberOfRefinementPoints / 4 + 1
			&& m_currentHeadIteration <= m_numberOfRefinementPoints / 2) {
		pan = -m_headScanPanRange / ((double) m_numberOfRefinementPoints / 4)
				* (m_currentHeadIteration - m_numberOfRefinementPoints / 4)
				+ m_headScanPanRange;
		tilt = m_headScanTiltDownRange
				/ ((double) m_numberOfRefinementPoints / 4)
				* (m_currentHeadIteration - m_numberOfRefinementPoints / 4)
				- m_headScanTiltDownRange;
	} else if (m_currentHeadIteration >= m_numberOfRefinementPoints / 2 + 1
			&& m_currentHeadIteration <= (m_numberOfRefinementPoints * 3) / 4) {
		pan = -m_headScanPanRange / ((double) m_numberOfRefinementPoints / 4)
				* (m_currentHeadIteration - m_numberOfRefinementPoints / 2);
		tilt = m_headScanTiltDownRange
				/ ((double) m_numberOfRefinementPoints / 4)
				* (m_currentHeadIteration - m_numberOfRefinementPoints / 2)
				- m_headScanTiltDownRange;
	} else if (m_currentHeadIteration
			>= (m_numberOfRefinementPoints * 3) / 4 + 1
			&& m_currentHeadIteration <= m_numberOfRefinementPoints - 1) {
		pan =
				m_headScanPanRange / ((double) m_numberOfRefinementPoints / 4)
						* (m_currentHeadIteration
								- (m_numberOfRefinementPoints * 3) / 4)
						- m_headScanPanRange;
		tilt =
				m_headScanPanRange / ((double) m_numberOfRefinementPoints / 4)
						* (m_currentHeadIteration
								- (m_numberOfRefinementPoints * 3) / 4);
	}

//	m_headInstance->MoveByAngle(pan, tilt);
	MoveByAngle(pan, tilt);

//	usleep(MIN_HEAD_RESPONSE_TIME);
}

void HeadMotion::MakeUpCM730Delay() {
	//cout << "before m_currentHeadIteration = " << m_currentHeadIteration << "\n";

	int offsetDelay = m_currentHeadIteration
			- (int) (0.11 * ((double) m_numberOfRefinementPoints)) * m_rotation;
	m_currentHeadIteration = (offsetDelay + m_numberOfRefinementPoints)
			% m_numberOfRefinementPoints;
	//cout << "after m_currentHeadIteration = " << m_currentHeadIteration << "\n";
}
