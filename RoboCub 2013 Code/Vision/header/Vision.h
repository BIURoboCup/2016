/*
 *	Vision.h
 *
 *  Created on: 6 Oct 2011
 *  Author: Guy
 */

#ifndef VISION_H_
#define VISION_H_

#include "Localization.h"
#include "General/MyThreadClass.h"
#include "WalkingAndActions.h"

#include "Linux/build/streamer/mjpg_streamer.h"
#include "MyCamera.h"
#include "VideoFeed.h"
#include "ImageProcess.h"
#include "ObjectDetector.h"
#include "HeadMotion.h"

#include "CM730.h"

#define INI_FILE_PATH       "/darwin/Data/CameraConfig.ini"

#define MIN_MILLISECS_FOR_CVWAIT 2

#define HEAD_OFFSET_STEP 	      		(4)
#define TIMEOUT_BALL_NOT_DETECTED 		1500000
#define TIMEOUT_GOAL_TRACKING			2000000
#define TIMEOUT_BALL_TRACKING	 		300000

#define GOAL_MAX_TOP_ROW_TO_CHANGE_HEAD_POSITION 			70
#define GOAL_MAX_COL_THRESHOLD_TO_CHANGE_HEAD_POSITION		35

#define HEAD_MAX_ANGLE_FROM_AVERAGE_POINT_PAN 	3
#define HEAD_MAX_ANGLE_FROM_AVERAGE_POINT_TILT  3
#define NUM_OF_FRAMES_FROM_LOCALIZATION_HEAD_MOVEMENT 1

#define HEAD_POSITION_FOR_BALL_HISTORY 2
#define HEAD_PAN_RIGHT_LIMIT  67
#define HEAD_PAN_LEFT_LIMIT (-HEAD_PAN_RIGHT_LIMIT)

#define BALL_MAX_USEC_TO_CLEAR_NOISE_BALLS 			(100 * 1000)
#define BALL_MIN_DISTANCE_CM_TO_CLEAR_NOISE_BALLS 	150

using namespace Robot;

class Vision: public MyThreadClass {

private:

	static Vision* m_uniqueInstance;
	bool m_debugMode;

	MyCamera* m_camera;
	VideoFeed* m_videoFeedsHead; // pointer to start of streaming videos queue
	int m_numOfFeeds; // number of streaming videos in the list
	Image* m_streamerImage;
	mjpg_streamer* m_wwwStreamer;
	CvVideoWriter* m_videoWriter;
	static struct tm m_programStartTimestamp;
	bool m_recordVideo;

	IplImage* m_rgbOriginalFrame;
	IplImage* m_rgbSegmentedFrame;
	IplImage* m_hsvOriginalFrame;
	IplImage* m_hsvSegmentedFrame;

	ObjectDetector m_myObjectDetector;
	ImageProcess m_myImageProcessor;

public:
	HeadMotion m_head;
private:
	CM730* m_cm730;

	bool m_ballDetected;
	HeadScanForGoal_e m_polesDetectionStatus;
	int m_iSeeGoalFrames;
	HeadTrackingStatus_e m_headTrackingStatus;

	Pole m_firstPole;
	Pole m_secondPole;
	Pole m_localizationPole;

	Ball m_ball;

	double m_lastPanPositionsBall[HEAD_POSITION_FOR_BALL_HISTORY];
	double m_lastTiltPositionsBall[HEAD_POSITION_FOR_BALL_HISTORY];

	double m_meanPanLastBallPosition;
	double m_meanTiltLastBallPosition;

	bool m_timestampTakenBallLost;
	bool m_firstBallDetectedFromExitHeadScan;

	myTimeval m_previousBallWasDetected;
	myTimeval m_currentTime;

	Localization* m_localization;

	double m_currPicPan; //for localization.
	double m_currPicTilt; //for localization.

	// Private constructor for singleton
	Vision();

public:

	static Vision* GetInstance();

	void ProgramStartInitializations(int visionFlag);
	static void SetProgramStartTimestamp(struct tm* programStartTimestamp);
	void Initialize();

	void InternalThreadFunc();
	void UpdateHeadPosition(Ball &ball);

	void HeadTracking();

	void DetectBall();

	void ScanForFieldPoles();
	void HeadScanForGoal();
	bool DoISeeTheGoal();
	void ResetSeeGoalFrames();
	bool IsClearToKick();
	bool IsClearToKickDistanceFromBall();
	double GetHeadYOffset();

	void HeadScanForBall();

	void SetEyesColor();

	void CheckSaveOfLastBallPosition();
	Angle GetAngleToLookAtGoal(int x, int y);
	// Video Stream attributes
	void UpdateAllVideos();
	void AddVideoFeed(IplImage* newFrame, const char* windowName); // adds a new video stream to the videos' list
	void RemoveAllVideoFeeds();
	void UpdateVideoStream(); // updates the video stream

	void SetHeadTrackingStatus(HeadTrackingStatus_e newHeadTrackingStatus);
	HeadTrackingStatus_e GetHeadTrackingStatus();

	double getCurrPicPan() {
		return m_currPicPan;
	} //for localization.
	double getCurrPicTilt() {
		return m_currPicTilt;
	} //for localization.

	virtual ~Vision();
};

#endif /* VISION_H_ */
