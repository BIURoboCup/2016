/*
 * Vision.cpp
 *
 *  Created on: 6 October 2011
 *      Author: Guy
 */

#include "Vision.h"

#define PI 3.14159265
#define SIGN(x) ( (x) >= 0 ? (1) : (-1) )

//#define DEBUG_SET_HEAD_TRACKING_STATUS
//#define DEBUG_IS_CLEAR_TO_KICK
#define WITH_HEAD_AND_EYE_LIGHTS

Vision* Vision::m_uniqueInstance = 0;
struct tm Vision::m_programStartTimestamp;

Vision* Vision::GetInstance() {
	if (m_uniqueInstance == 0) {
		m_uniqueInstance = new Vision();
	}
	return m_uniqueInstance;
}

Vision::Vision() {
	m_debugMode = false;
	m_recordVideo = false;
	cout << "Vision::Vision()" << endl;
	m_camera = MyCamera::GetInstance();
	m_numOfFeeds = 0;

	m_timestampTakenBallLost = false;
	m_firstBallDetectedFromExitHeadScan = false;

	CvSize frameSize = m_camera->GetImageProperties();

	m_rgbOriginalFrame = cvCreateImage(frameSize, IPL_DEPTH_8U, 3);
	m_rgbSegmentedFrame = cvCreateImage(frameSize, IPL_DEPTH_8U, 3);
	m_hsvOriginalFrame = cvCreateImage(frameSize, IPL_DEPTH_8U, 3);
	m_hsvSegmentedFrame = cvCreateImage(frameSize, IPL_DEPTH_8U, 3);

	SetHeadTrackingStatus(BALL_TRACKING);

}

void Vision::ProgramStartInitializations(int visionFlag) {
	if (OPEN_VISION_DEBUG_WINDOWS == (visionFlag & OPEN_VISION_DEBUG_WINDOWS)) {
		m_debugMode = true;
	}

	if (SAVE_CAPTURED_VIDEO_STREAM
			== (visionFlag & SAVE_CAPTURED_VIDEO_STREAM)) {
		//cout << "Capture Video\n";
		std::stringstream fileName;

		fileName << "RoBIU-VideoRecord." << m_programStartTimestamp.tm_mday
				<< "-" << m_programStartTimestamp.tm_mon + 1 << "-"
				<< m_programStartTimestamp.tm_year - 100 << "."
				<< m_programStartTimestamp.tm_hour << "-"
				<< m_programStartTimestamp.tm_min << "-"
				<< m_programStartTimestamp.tm_sec << ".avi"; //.date.time

		m_videoWriter = cvCreateVideoWriter(fileName.str().c_str(),
				CV_FOURCC('M', 'J', 'P', 'G'), 7,
				m_camera->GetImageProperties());

		m_recordVideo = true;
	}

}

void Vision::SetProgramStartTimestamp(struct tm* programStartTimestamp) {
	m_programStartTimestamp = (*programStartTimestamp);
}

void Vision::Initialize() {
	m_head.Initialize();
	m_cm730 = WalkingAndActions::GetInstance()->GetCM730();
	cout << "CM730 address = " << m_cm730 << "\n";

	//write blue color to head's led
#ifdef WITH_HEAD_AND_EYE_LIGHTS
	m_cm730->WriteWord(CM730::P_LED_HEAD_L, CM730::MakeColor(255, 0, 0), 0);
#else
	m_cm730->WriteWord(CM730::P_LED_HEAD_L, CM730::MakeColor(0, 0, 0), 0);
	m_cm730->WriteWord(CM730::P_LED_EYE_L, CM730::MakeColor(0, 0, 0), 0);
#endif

	m_localization = Localization::GetInstance();
	cout << "Loading Vision ini settings\n";

	memset(m_lastPanPositionsBall, 0,
			sizeof(double) * HEAD_POSITION_FOR_BALL_HISTORY);
	memset(m_lastTiltPositionsBall, 0,
			sizeof(double) * HEAD_POSITION_FOR_BALL_HISTORY);

	m_head.HeadScan(0, 25, 1);

	try {
		minIni* iniFile = new minIni(INI_FILE_PATH);

#ifdef USE_V4L2
		m_camera->LoadIniSettings(iniFile);
#endif
		m_myImageProcessor.LoadIniSettings(iniFile);

#ifdef USE_V4L2
		m_camera->PrintCameraSettings();
#endif

		if (m_debugMode == true) {
			m_streamerImage = new Image(Camera::WIDTH, Camera::HEIGHT,
					Image::RGB_PIXEL_SIZE);
			m_wwwStreamer = new mjpg_streamer(Camera::WIDTH, Camera::HEIGHT);
			httpd::ini = iniFile;
		}
	} catch (...) {
		cout << "error loading vision ini file\n";
		m_myImageProcessor.LoadDefaultColorSettings();
	}

}

void Vision::InternalThreadFunc() {
	cout << "Vision Internal Thread. tid = " << (long int) syscall(224) << endl;

	Initialize();
	m_previousBallWasDetected.update();
	m_currentTime.update();

	myTimeval time1;
	time1.update();
#ifdef USE_OPENCV
	if (m_debugMode == true)
	{
		//	AddVideoFeed(m_rgbOriginalFrame, "RGB Original");
	}
#endif

	while (exitProgram == false) {
		m_currentTime.update();

		//for localization.
		m_currPicPan = m_head.GetPanAngle();
		m_currPicTilt = m_head.GetTiltAngle();

#ifdef USE_V4L2
		m_camera->GetFrameUsingV4L2(m_rgbOriginalFrame);
#endif

#ifdef USE_OPENCV
		m_camera->GetFrameUsingOpenCV(m_rgbOriginalFrame);
#endif

#ifdef USE_V4L2
		cvCvtColor(m_rgbOriginalFrame, m_hsvOriginalFrame, CV_RGB2HSV);
#endif

#ifdef USE_OPENCV
		cvCvtColor(m_rgbOriginalFrame, m_hsvOriginalFrame,CV_BGR2HSV);
#endif

		cvCopy(m_hsvOriginalFrame, m_hsvSegmentedFrame);

		m_myImageProcessor.ImageHSVSegmentation(m_hsvSegmentedFrame,
				m_rgbOriginalFrame);
		m_myObjectDetector.DetectGreenLimits(m_hsvSegmentedFrame);

		ScanForFieldPoles();
		DetectBall();
		HeadTracking();

#ifdef WITH_HEAD_AND_EYE_LIGHTS
		SetEyesColor();
#endif

		UpdateAllVideos();

		//usleep(10*1000);
		m_localization->process();
	}

	cout << "Vision Thread exited\n";
}

void Vision::SetEyesColor() {
	m_polesDetectionStatus = m_myObjectDetector.GetPolesDetectionStatus();

	if (exitProgram == true) {
		return;
	}

	if (m_ballDetected == false && m_polesDetectionStatus == NO_POLES) {
		m_cm730->WriteWord(CM730::P_LED_EYE_L, CM730::MakeColor(255, 255, 255),
				0);
	} else if (m_ballDetected == true && m_polesDetectionStatus == NO_POLES) {
		m_cm730->WriteWord(CM730::P_LED_EYE_L, CM730::MakeColor(0, 255, 0), 0);
	} else if (m_ballDetected == false && m_polesDetectionStatus != NO_POLES) {
		m_cm730->WriteWord(CM730::P_LED_EYE_L, CM730::MakeColor(255, 0, 255),
				0);
	} else // see both
	{
		m_cm730->WriteWord(CM730::P_LED_EYE_L, CM730::MakeColor(255, 255, 0),
				0);
	}

}

void Vision::HeadTracking() {
	m_currentTime.update();
	static myTimeval firstTimeBallDetectedFromStartOfBallDetectionTime;
	static bool firstTimeBallDetectedFromStartOfBallDetection = true;

	if (exitProgram == true) {
		return;
	}

	if (m_headTrackingStatus == BALL_TRACKING) {
		HeadScanForBall();
	} else if (m_headTrackingStatus == GOAL_TRACKING) {
		HeadScanForGoal();
	} else if (m_headTrackingStatus == BALL_AND_GOAL_TRACKING) {
		if (m_currentTime - m_previousBallWasDetected > TIMEOUT_GOAL_TRACKING) {
			HeadScanForBall();
			//		cout<<"BALL_AND_GOAL_TRACKING - A, now BALL \n";
			firstTimeBallDetectedFromStartOfBallDetection = true;
		} else {
			if (firstTimeBallDetectedFromStartOfBallDetection == true) {
				firstTimeBallDetectedFromStartOfBallDetection = false;
				firstTimeBallDetectedFromStartOfBallDetectionTime.update();
			}

			if (m_currentTime
					- firstTimeBallDetectedFromStartOfBallDetectionTime <= TIMEOUT_BALL_TRACKING) {
				HeadScanForBall();
				//			cout<<"BALL_AND_GOAL_TRACKING - B, now BALL \n";
			} else {
				HeadScanForGoal();
				//			cout<<"BALL_AND_GOAL_TRACKING, now GOAL \n";
			}
		}

	} else if (m_headTrackingStatus == KICKING_BALL_TRACKING) {

		HeadScanForBall();
	}

}

void Vision::DetectBall() {
	Ball lastBall = m_ball;
	double lastDistance =
			lastBall.CalculateDistanceFromFrameCenter_ByBallDiameter();
	double currentDistance;

	m_ballDetected = m_myObjectDetector.DetectAverageBall(m_hsvSegmentedFrame,
			m_ball);

	if (m_ballDetected == true) {
		//cout << "time since last = " << TimeStamp() -  lastBall.m_timeBallDetected << " time since current = " << TimeStamp() - m_ball.m_timeBallDetected << "\n";

		if (m_ball.m_timeBallDetected
				- lastBall.m_timeBallDetected < BALL_MAX_USEC_TO_CLEAR_NOISE_BALLS) {
			currentDistance =
					m_ball.CalculateDistanceFromFrameCenter_ByBallDiameter();

			if (currentDistance
					> lastDistance + BALL_MIN_DISTANCE_CM_TO_CLEAR_NOISE_BALLS) {
				//cout<<"currentDistance = "<<currentDistance<< " lastDistance = "<<lastDistance;
				//cout<<",        diffTimeUsec = "<<m_ball.m_timeBallDetected - lastBall.m_timeBallDetected<<"\n";

				m_ballDetected = false;
			}
		}
	}

	if (m_ballDetected == true) {
		m_localization->setBall(m_ball);

		CheckSaveOfLastBallPosition();

		m_timestampTakenBallLost = false;

		//double dist = m_ball.CalculateDistanceFromFrameCenter_ByBallDiameter();
		//cout << "ball num of pixels = " << m_ball.m_numOfPixelsInBall << " diameter = " << m_ball.m_diameter << " dist = " << dist << "\n";
		cvCircle(m_hsvSegmentedFrame,
				cvPoint(m_ball.m_center.X, m_ball.m_center.Y), m_ball.m_radius,
				cvScalar(0, 255, 255), 3);
		m_previousBallWasDetected.update();
	} else {
		m_localization->setBall(false);
	}

}

void Vision::CheckSaveOfLastBallPosition() {
	int i;

	double tempMeanPan = 0, tempMeanTilt = 0;
	double distancePan, distanceTilt;

	for (i = 0; i < HEAD_POSITION_FOR_BALL_HISTORY - 1; i++) {
		m_lastPanPositionsBall[i] = m_lastPanPositionsBall[i + 1];
		m_lastTiltPositionsBall[i] = m_lastTiltPositionsBall[i + 1];

		tempMeanPan += m_lastPanPositionsBall[i + 1];
		tempMeanTilt += m_lastTiltPositionsBall[i + 1];
	}

	m_lastPanPositionsBall[HEAD_POSITION_FOR_BALL_HISTORY - 1] =
			m_head.GetPanAngle();
	m_lastTiltPositionsBall[HEAD_POSITION_FOR_BALL_HISTORY - 1] =
			m_head.GetTiltAngle();

	tempMeanPan += m_lastPanPositionsBall[HEAD_POSITION_FOR_BALL_HISTORY - 1];
	tempMeanTilt += m_lastTiltPositionsBall[HEAD_POSITION_FOR_BALL_HISTORY - 1];

	tempMeanPan = tempMeanPan / HEAD_POSITION_FOR_BALL_HISTORY;
	tempMeanTilt = tempMeanTilt / HEAD_POSITION_FOR_BALL_HISTORY;

	//cout<<"tempMeanPan = "<<tempMeanPan<<" tempMeanTilt = "<<tempMeanTilt<<endl;
	for (i = 0; i < HEAD_POSITION_FOR_BALL_HISTORY; i++) {
		//	cout << i <<"P = " << m_lastPanPositionsBall[i] << " "<< i << "T = " << m_lastTiltPositionsBall[i] << endl;

		distancePan = ABS(m_lastPanPositionsBall[i] - tempMeanPan); // calculating the distance of the ball from the mean point in x-axis
		distanceTilt = ABS(m_lastTiltPositionsBall[i] - tempMeanTilt); // calculating the distance of the ball from the mean point in y-axis

		if (distancePan > HEAD_MAX_ANGLE_FROM_AVERAGE_POINT_PAN
				|| distanceTilt > HEAD_MAX_ANGLE_FROM_AVERAGE_POINT_TILT) {
			return;
		}
	}

	m_meanPanLastBallPosition = tempMeanPan;
	m_meanTiltLastBallPosition = tempMeanTilt;

//	cout<<"MeanP = "<<m_meanPanLastBallPosition<<" MeanT = "<<m_meanTiltLastBallPosition<<endl;
}

void Vision::ScanForFieldPoles() {
	m_myObjectDetector.TheNewDetectFieldPoles(m_hsvSegmentedFrame, m_firstPole,
			m_secondPole, m_localizationPole);

//	cout << "Number of Poles Detected = " << numOfPoles << "\n";

	if (m_firstPole.m_dataValid == true && m_secondPole.m_dataValid == false) {
		Localization::GetInstance()->setPoles(m_firstPole);
		Localization::GetInstance()->setPoles(true);
		cvLine(
				m_hsvSegmentedFrame,
				cvPoint(m_firstPole.m_localizationBottomPoint.X,
						m_firstPole.m_localizationBottomPoint.Y),
				cvPoint(m_firstPole.m_localizationTopPoint.X,
						m_firstPole.m_localizationTopPoint.Y),
				cvScalar(0, 255, 255), 3);
	} else if (m_firstPole.m_dataValid == true
			&& m_secondPole.m_dataValid == true) {

		Localization::GetInstance()->setPoles(m_firstPole, m_secondPole);
		Localization::GetInstance()->setPoles(true);

		cvLine(
				m_hsvSegmentedFrame,
				cvPoint(m_firstPole.m_localizationBottomPoint.X,
						m_firstPole.m_localizationBottomPoint.Y),
				cvPoint(m_firstPole.m_localizationTopPoint.X,
						m_firstPole.m_localizationTopPoint.Y),
				cvScalar(0, 255, 255), 3);
		cvLine(
				m_hsvSegmentedFrame,
				cvPoint(m_secondPole.m_localizationBottomPoint.X,
						m_secondPole.m_localizationBottomPoint.Y),
				cvPoint(m_secondPole.m_localizationTopPoint.X,
						m_secondPole.m_localizationTopPoint.Y),
				cvScalar(0, 255, 255), 3);

	}

	if (m_localizationPole.m_dataValid == true) {
		Localization::GetInstance()->setPoles(m_localizationPole);
		cvLine(
				m_hsvSegmentedFrame,
				cvPoint(m_localizationPole.m_localizationBottomPoint.X,
						m_localizationPole.m_localizationBottomPoint.Y),
				cvPoint(m_localizationPole.m_localizationTopPoint.X,
						m_localizationPole.m_localizationTopPoint.Y),
				cvScalar(160, 255, 255), 3);

	}

}

void Vision::HeadScanForBall() {
	if (m_ballDetected == true) {
		if (m_firstBallDetectedFromExitHeadScan == true) {
			//	m_head.MakeUpCM730Delay();
			m_head.HeadScan();

			m_firstBallDetectedFromExitHeadScan = false;
		} else {
			UpdateHeadPosition(m_ball);
		}
	} else {
		if (m_currentTime
				- m_previousBallWasDetected > TIMEOUT_BALL_NOT_DETECTED) {
			m_firstBallDetectedFromExitHeadScan = true;

			if (m_timestampTakenBallLost == false) {
				m_head.HeadScan(m_meanPanLastBallPosition,
						m_meanTiltLastBallPosition, 1);
				m_timestampTakenBallLost = true;
			} else {
				m_head.HeadScan();
			}
		}
	}
}
void Vision::HeadScanForGoal() {
	m_polesDetectionStatus = m_myObjectDetector.GetPolesDetectionStatus();

	Location myLocation = m_localization->GetMyLocation();
	double headPanByMyAngleAttack, headPanByMyAngleDefend;

	Point2D offsetGoal;
	static int headMoveFromLocalizationFrames = 0;
	static int headRotation = 1;
	static bool headGoLeft = true;
	static bool headGoRight = false;

	if (m_polesDetectionStatus != NO_POLES) {
		headMoveFromLocalizationFrames = 0;
		headRotation = 1;
		m_iSeeGoalFrames++;
	}

	//cout << "[Vision]::HeadScanForGoal\n";

	switch (m_polesDetectionStatus) {

	case NO_POLES:

		headPanByMyAngleDefend = GetAngleToLookAtGoal(0, 0);
		headPanByMyAngleAttack = GetAngleToLookAtGoal(0, 600);

		if (myLocation.y < 70 && fabs(myLocation.x) < 100) {
			headPanByMyAngleDefend = 180;
		}
		//		headPanByMyAngleAttack = -MIN(70,ABS(myAngle))*SIGN(myAngle);
		//		headPanByMyAngleDefend = -MIN(70,180 - ABS(myAngle))*SIGN(myAngle);

		//cout << "headGoLeft =  " << headGoLeft << "headGoRight = " << headGoRight << "\n";

		//	cout << "headPanByMyAngleDefend = " << headPanByMyAngleDefend << " headPanByMyAngleAttack = " << headPanByMyAngleAttack << "\n";

		if (ABS(headPanByMyAngleAttack) <= ABS(headPanByMyAngleDefend)) {
			if (headMoveFromLocalizationFrames
					< NUM_OF_FRAMES_FROM_LOCALIZATION_HEAD_MOVEMENT) {
				//cout << "[Vision::]MoveByAngle. my angle = "<< headPanByMyAngleAttack << " tilt = " << m_head.GetTiltAngle() <<  "\n";
				m_head.MoveByAngle(headPanByMyAngleAttack, 50);
				//		m_head.MoveByAngle(0,50);
				headMoveFromLocalizationFrames++;
				//headRotation = 1;
				//			cout << "NO_POLES - Localization - Attack" << " Pan = " << headPanByMyAngleAttack<< endl;
			} else {
				if (m_head.GetPanAngle() < HEAD_PAN_LEFT_LIMIT
						&& headGoLeft == true) {
					headGoLeft = false;
					headGoRight = true;
//						cout<<"headGoRight\n";
				} else if (m_head.GetPanAngle() > HEAD_PAN_RIGHT_LIMIT
						&& headGoRight == true) {
					headGoLeft = true;
					headGoRight = false;
					//				cout<<"headGoLeft\n";
				}
				//				cout << "headGoLeft = " << headGoLeft;
				//				cout << " headGoRight = " << headGoRight << "\n";
				if (headGoLeft == true) {
					headRotation = 1;
				} else {
					headRotation = -1;
				}

				//			cout << "Attack: headGoLeft =  " << headGoLeft << "headGoRight = " << headGoRight << "\n";
				m_head.MoveByAngleOffset(headRotation * -HEAD_OFFSET_STEP,
						-HEAD_OFFSET_STEP);
				//			cout << "NO_POLES" << endl;
			}

		} else {
			if (headMoveFromLocalizationFrames
					< NUM_OF_FRAMES_FROM_LOCALIZATION_HEAD_MOVEMENT) {
				//			cout << "[Vision::]MoveByAngle(0,50). my angle = "<< headPanByMyAngleDefend << " tilt = " << m_head.GetTiltAngle() << "\n";
				m_head.MoveByAngle(headPanByMyAngleDefend, 50);
				//m_head.MoveByAngle(0,50);
				headMoveFromLocalizationFrames++;

				//headRotation = 1;
				//				cout << "NO_POLES - Localization - Defend" << " Pan = " << headPanByMyAngleDefend<< endl;
			} else {
				if (m_head.GetPanAngle() < HEAD_PAN_LEFT_LIMIT
						&& headGoLeft == true) {
					headGoLeft = false;
					headGoRight = true;
					//			cout<<"headGoRight\n";

				} else if (m_head.GetPanAngle() > HEAD_PAN_RIGHT_LIMIT
						&& headGoRight == true) {
					headGoLeft = true;
					headGoRight = false;
					//			cout<<"headGoLeft\n";
				}

				if (headGoRight == true) {
					headRotation = 1;
				} else {
					headRotation = -1;
				}

				//			cout << "Defense: headGoLeft =  " << headGoLeft << "headGoRight = " << headGoRight << "\n";
				m_head.MoveByAngleOffset(headRotation * HEAD_OFFSET_STEP,
						-HEAD_OFFSET_STEP);
				//			cout << "NO_POLES" << endl;
			}

		}

		//		usleep(MIN_HEAD_RESPONSE_TIME);
		break;

	case ONE_POLE_FACE_UP:
		//cout << "ONE_POLE_FACE_UP" << endl;
		if (m_head.GetPanAngle() < HEAD_PAN_LEFT_LIMIT && headGoLeft == true) {
			headGoLeft = false;
			headGoRight = true;
		} else if (m_head.GetPanAngle() > HEAD_PAN_RIGHT_LIMIT
				&& headGoRight == true) {
			headGoLeft = true;
			headGoRight = false;
		}

		if (headGoRight == true) {
			headRotation = 1;
		} else {
			headRotation = -1;
		}
		m_head.MoveByAngleOffset(headRotation * HEAD_OFFSET_STEP,
				-HEAD_OFFSET_STEP);
		//		usleep(MIN_HEAD_RESPONSE_TIME);
		break;

	case ONE_POLE_LEFT_SIDE:
		//		cout << "ONE_POLE_LEFT_SIDE" << endl;

		if (m_firstPole.m_localizationMiddlePoint.X
				> GOAL_MAX_COL_THRESHOLD_TO_CHANGE_HEAD_POSITION) {
			m_head.MoveByAngleOffset(HEAD_OFFSET_STEP, 0);
			//			usleep(MIN_HEAD_RESPONSE_TIME);
		}

		break;

	case ONE_POLE_RIGHT_SIDE:
		//		cout << "ONE_POLE_RIGHT_SIDE" << endl;

		if (m_firstPole.m_localizationMiddlePoint.X
				< Camera::WIDTH - GOAL_MAX_COL_THRESHOLD_TO_CHANGE_HEAD_POSITION) {
			m_head.MoveByAngleOffset(-HEAD_OFFSET_STEP, 0);
			//			usleep(MIN_HEAD_RESPONSE_TIME);
		}

		break;
	case ONE_POLE_UNKNOWN_SIDE:
		//	cout << "ONE_POLE_UNKNOWN_SIDE" << endl;
//			if (m_firstPole.m_localizationBottomPoint.X <= Camera::WIDTH/2)
//			{
//				m_head.MoveByAngleOffset(-HEAD_OFFSET_STEP,0);
//			}
//			else
//			{
//				m_head.MoveByAngleOffset(HEAD_OFFSET_STEP,0);
//			}
		//		usleep(MIN_HEAD_RESPONSE_TIME);
		if (m_head.GetPanAngle() < HEAD_PAN_LEFT_LIMIT && headGoLeft == true) {
			headGoLeft = false;
			headGoRight = true;
		} else if (m_head.GetPanAngle() > HEAD_PAN_RIGHT_LIMIT
				&& headGoRight == true) {
			headGoLeft = true;
			headGoRight = false;
		}

		if (headGoRight == true) {
			headRotation = 1;
		} else {
			headRotation = -1;
		}
		m_head.MoveByAngleOffset(headRotation * HEAD_OFFSET_STEP,
				-HEAD_OFFSET_STEP);

		break;
	case TWO_POLES_FACE_UP_LEFT:
		//		cout << "TWO_POLES_FACE_UP_LEFT" << endl;

		if (m_firstPole.m_localizationTopPoint.Y > POLE_HEIGHT_LIMIT) {
			m_head.MoveByAngleOffset(-2, -HEAD_OFFSET_STEP);
			//			usleep(MIN_HEAD_RESPONSE_TIME);
		}

		break;

	case TWO_POLES_FACE_UP_RIGHT:
		//		cout << "TWO_POLES_FACE_UP_RIGHT" << endl;

		if (m_secondPole.m_localizationTopPoint.Y > POLE_HEIGHT_LIMIT) {
			m_head.MoveByAngleOffset(2, -HEAD_OFFSET_STEP);
			//			usleep(MIN_HEAD_RESPONSE_TIME);
		}
		break;

	case TWO_POLES_FACE_UP:
		//		cout << "TWO_POLES_FACE_UP" << endl;
		m_head.MoveByAngleOffset(0, -HEAD_OFFSET_STEP);
		//		usleep(MIN_HEAD_RESPONSE_TIME);

		break;

	case TWO_POLES_NO_FACE_UP:

		//	cout << "TWO_POLES_NO_FACE_UP" << endl;

		offsetGoal.X = (m_firstPole.m_localizationMiddlePoint.X
				+ m_secondPole.m_localizationMiddlePoint.X) / 2
				- Camera::WIDTH / 2;

		if (MAX(m_firstPole.m_poleHeight,m_secondPole.m_poleHeight)
				< Camera::HEIGHT - GOAL_MAX_TOP_ROW_TO_CHANGE_HEAD_POSITION) {
			offsetGoal.Y =
					MIN(m_firstPole.m_localizationTopPoint.Y,m_secondPole.m_localizationTopPoint.Y)
							- GOAL_MAX_TOP_ROW_TO_CHANGE_HEAD_POSITION;

			offsetGoal *= -1;
			offsetGoal.X *= (Camera::VIEW_H_ANGLE / ((double) Camera::WIDTH));
			offsetGoal.Y *= (Camera::VIEW_V_ANGLE / ((double) Camera::HEIGHT));

			m_head.m_headInstance->MoveTracking(offsetGoal);
			//			usleep(MIN_HEAD_RESPONSE_TIME);
		}

		break;
	default:

		break;

	}
}

bool Vision::DoISeeTheGoal() {
	int numOfFramesGoalWasSeen = m_iSeeGoalFrames;

	if (numOfFramesGoalWasSeen > 8) {
		m_iSeeGoalFrames = 0;
		return true;
	}
	return false;
}

void Vision::ResetSeeGoalFrames() {
	m_iSeeGoalFrames = 0;
}

bool Vision::IsClearToKickDistanceFromBall() {
	double tilt = MotionStatus::m_CurrentJoints.GetAngle(
			JointData::ID_HEAD_TILT);
	double tilt_min = Head::GetInstance()->GetBottomLimitAngle();

	if (m_head.IsHeadOffsetToKick() == false) {
#ifdef DEBUG_IS_CLEAR_TO_KICK
		cout << "[Vision]::IsClearToKickDistanceFromBall: IsHeadOffsetToKick returned false, offsetY = " << m_head.GetHeadYOffset() << "\n";
#endif
		return false;
	}

	if (tilt > (tilt_min + MX28::RATIO_VALUE2ANGLE + 4)) {
#ifdef DEBUG_IS_CLEAR_TO_KICK
		cout << "[Vision]::IsClearToKickDistanceFromBall: returned false, tilt = " << tilt << "\n";
#endif
		return false;
	}

#ifdef DEBUG_IS_CLEAR_TO_KICK
	cout << "[Vision]::IsClearToKick: returned true, tilt = " << tilt << " IsHeadOffsetToKick = "<< m_head.GetHeadYOffset() <<"\n";
#endif

	return true;
}

bool Vision::IsClearToKick() {
	double pan = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	double tilt = MotionStatus::m_CurrentJoints.GetAngle(
			JointData::ID_HEAD_TILT);
	double tilt_min = Head::GetInstance()->GetBottomLimitAngle();
	double kickAngle = 30;

	if (m_ballDetected == false) {
#ifdef DEBUG_IS_CLEAR_TO_KICK
		cout << "[Vision]::IsClearToKick: ball not detected\n";
#endif
		return false;
	}

	if (m_head.IsHeadOffsetToKick() == false) {
#ifdef DEBUG_IS_CLEAR_TO_KICK
		cout << "[Vision]::IsClearToKick: IsHeadOffsetToKick returned false. Yoffset = " << m_head.GetHeadYOffset() << "\n";
#endif
		return false;
	}

	if (fabs(pan) > kickAngle
			|| (tilt > (tilt_min + MX28::RATIO_VALUE2ANGLE + 5))) {
#ifdef DEBUG_IS_CLEAR_TO_KICK
		cout << "[Vision]::IsClearToKick: returned false, pan = "<< pan << " tilt = " << tilt << "\n";
#endif
		return false;
	}

#ifdef DEBUG_IS_CLEAR_TO_KICK
	cout << "[Vision]::IsClearToKick: returned true \n";
#endif
	return true;
}

double Vision::GetHeadYOffset() {
	return m_head.GetHeadYOffset();
}

Angle Vision::GetAngleToLookAtGoal(int x, int y) {
	Location myLocation = m_localization->GetMyLocation();

	return (Angle(RAD2DEG * atan2(x - myLocation.x, y - myLocation.y))
			- myLocation.angle);
}

void Vision::UpdateAllVideos() {
	if (m_debugMode == true || m_recordVideo == true) {
		if (m_debugMode == true) {
#ifdef USE_V4L2
			cvCvtColor(m_hsvSegmentedFrame, m_rgbSegmentedFrame, CV_HSV2RGB);
#endif

#ifdef USE_OPENCV
			cvCvtColor(m_hsvSegmentedFrame, m_rgbSegmentedFrame ,CV_HSV2BGR);
#endif

			UpdateVideoStream();
		}

		if (m_recordVideo == true) {
			cvCvtColor(m_hsvSegmentedFrame, m_rgbSegmentedFrame, CV_HSV2BGR);
			cvWriteFrame(m_videoWriter, m_rgbSegmentedFrame);
		}
	}

	cvWaitKey(MIN_MILLISECS_FOR_CVWAIT); // wait for 'ESC'
}

void Vision::UpdateHeadPosition(Ball &ball) {
	m_myObjectDetector.SetHeadMoveInLastFrame(true);
	m_head.MoveTracking(ball.m_center, m_ballDetected);

}

void Vision::SetHeadTrackingStatus(HeadTrackingStatus_e newHeadTrackingStatus) {
#ifdef DEBUG_SET_HEAD_TRACKING_STATUS
	cout << "Set head tracking status to: " << newHeadTrackingStatus << "\n";
#endif
	m_head.UpdateHeadScanForBallParameters(newHeadTrackingStatus);

	m_headTrackingStatus = newHeadTrackingStatus;
}

HeadTrackingStatus_e Vision::GetHeadTrackingStatus() {
	return m_headTrackingStatus;
}

/*
 * Description: Adds a new video stream to the videos' streaming list.
 * Puts the new video in the end of the queue.
 * Parameters: bool ifDebugging - to know if to actually add a new window
 * Return Value: None.
 */
void Vision::AddVideoFeed(IplImage* newFrame, const char* windowName) {
	VideoFeed* newVideo = new VideoFeed(m_numOfFeeds, newFrame, windowName); // allocate the new video feed
	VideoFeed* iterator = m_videoFeedsHead; // iterator runs until end of queue

	cout << "DBG Vision Camera adding Video Feed" << endl;

	if (m_numOfFeeds == 0) // no other videos in queue.
			{
		m_videoFeedsHead = newVideo; // this will be the head of the queue
	} else {
		while (iterator->m_next != NULL) {
			iterator = iterator->m_next; // goes to end of the queue
		}

		iterator->m_next = newVideo; // connect the new video to the queue
	}

	newVideo->m_next = NULL; // set the tail to NULL
	m_numOfFeeds++; // increase queue number of videos
	cout << "Number of Video Feeds = " << m_numOfFeeds << "\n";
}

/*
 * Description: Adds a new video stream to the videos' streaming list.
 * Puts the new video in the end of the queue.
 * Parameters: bool ifDebugging - to know if to actually add a new window
 * Return Value: None.
 */
void Vision::RemoveAllVideoFeeds() {
	cout << "Removing All Video Feeds. Number of Videos = " << m_numOfFeeds
			<< "\n";

	VideoFeed* iterator = m_videoFeedsHead; // iterator runs until end of queue
	VideoFeed* feedToRemove;

	for (int i = 0; i < m_numOfFeeds; i++) {
		feedToRemove = iterator;
		iterator = iterator->m_next; // goes to end of the queue
		if (feedToRemove != NULL) {
			delete feedToRemove;
		}
	}

	m_videoFeedsHead = NULL;
	m_numOfFeeds = 0;
}

/*
 * Description: The function updates all the streaming videos with their matching frames
 * Parameters: IplImage** newFramesList - Contains the list of frames, each one updates its matching streaming video by order.
 * Return Value: bool - a flag that says whether or not to terminate the process.
 */
void Vision::UpdateVideoStream() {
	VideoFeed* iterator = m_videoFeedsHead; // goes over the streaming videos' queue

	for (int i = 0; i < m_numOfFeeds; i++) // goes over the queue
			{
		iterator->UpdateFrame(); // updates frame for the current video
		iterator = iterator->m_next;
	}

#ifdef USE_OPENCV
	cvCvtColor(m_rgbSegmentedFrame, m_rgbSegmentedFrame ,CV_BGR2RGB);
#endif
	m_streamerImage->m_ImageData =
			(unsigned char*) (m_rgbSegmentedFrame->imageData);
	m_wwwStreamer->send_image(m_streamerImage);

}

Vision::~Vision() {
	cout << "Vision d'tor" << endl;
	RemoveAllVideoFeeds();
	if (m_recordVideo == true) {
		cvReleaseVideoWriter(&m_videoWriter);
	}
	cvReleaseImage(&m_rgbOriginalFrame);
	cvReleaseImage(&m_rgbSegmentedFrame);
	cvReleaseImage(&m_hsvOriginalFrame);
	cvReleaseImage(&m_hsvSegmentedFrame);
}
