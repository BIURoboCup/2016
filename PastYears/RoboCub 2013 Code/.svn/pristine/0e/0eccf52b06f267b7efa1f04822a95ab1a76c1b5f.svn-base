#ifndef _OBJECT_DETECTOR_H_
#define _OBJECT_DETECTOR_H_

#include "opencv2/opencv.hpp"
#include "IncludesFile.h"

#include "ImageProcess.h"

#include "ColorsValues.h"
#include "VisionEnums.h"
#include "BallCandidatesList.h"
#include "PoleCandidatesList.h"

#include "FirTrendline.h"

#define PI 3.14159265
#define SIGN(x) ( (x) >= 0 ? (1) : (-1) )

#define GET_OUT_OF_SHAPE_CONSECUTIVE_PIXELS 4

#define NUM_OF_ITERATIONS_SEARCHING_THE_BALL 2

#define MAX_DISTANCE_FROM_AVERAGE_POINT 10

#define MIN_BOUNDARIES_RECTANGLE 8
#define MAX_BOUNDARIES_RECTANGLE 90

#define EXTRA_BOUNDARY_FOR_ORANGE_HUE_BALL_SECOND_SCAN 3

//////////////////////////////////POLES/////////////////////////////////////////

// green field limit thresholds
#define MIN_GREEN_SEQUENCE_TO_FIELD_LIMITS 3
#define MIN_RATIO_TO_ZERO_FIELD_LIMITS 0.75

#define PREDICTED_GREEN_LIMITS_ERROR 	20

////////////////////////////////
/*Step 1 pole detection*/
#define SIZE_OF_SEARCH_LINE_FOR_POTENTIAL_COLUMN	20
#define MIN_RATIO_COLOR_PIXELS_FOR_POTENTIAL_COLUMN	0.33

/*Step 2 pole detection*/
#define GET_OUT_OF_POTENTIAL_SPOT_COLUMN_SEQUENCES 7

/*Step 3 pole detection*/
#define SQUARE_SEARCH_SIZE_OFFSET_Y	18
#define SQUARE_SEARCH_SIZE_OFFSET_X	9
#define NUM_OF_ROWS_TO_AVG_POLE_STARTING_POINT 4

/*Step 4 pole detection*/
#define MAX_NUM_OF_NOT_COLORED_PIXELS_SEQUENCE 4
#define MAX_NUM_OF_ROWS_WITHOUT_COLORED_PIXELS 4

/*Step 5 pole detection*/
#define MIN_POLE_WIDTH  1.75
#define MAX_POLE_WIDTH  36
#define MIN_POLE_HEIGHT	25
#define MIN_WIDTH_HEIGHT_RATIO_FOR_POLE 2.5

#define PRECENTAGE_TO_REMOVE_POINTS_FROM_TRENDLINE 0.2

/*Step 7 pole detection*/
#define POLE_HEIGHT_LIMIT 5

#define MIN_ANGLE_FOR_POLE_IDENTIFICATION_SIDE_ROTATION 0.25
#define OFFSET_SIZE_FOR_POLE_IDENTIFICATION_SIDE_ROTATION 9

#define POLE_SIDE_SQUARE_OFFSET 9
#define POLE_SIDE_IDENTIFIACTION_SQUARE_SIZE_HEIGHT	15
#define POLE_SIDE_IDENTIFIACTION_SQUARE_SIZE_WIDHT	45
#define POLE_SIDE_MIN_SQUARE_RATIO 0.03

/////////////////////////////////////////////////////////

class ObjectDetector {
private:

	static bool m_debugMode;
	static bool m_ballDetected;

	Ball m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL]; // static variables, holding the last 3 balls detected
	BallCandidatesList m_ballCandidatesList;

	PoleCandidatesList m_poleCandidatesList;
	HeadScanForGoal_e m_polesDetectionStatus;

	bool m_headMoveInLastFrame;

	int m_greenLimitPerColumn[Camera::WIDTH];

	bool m_potientialColumns[Camera::WIDTH];

public:

	ObjectDetector();

	bool DetectBall(IplImage* image, CvRect& boundriesRectangle, Ball &ball);
	bool DetectAverageBall(IplImage* image, Ball &ball);
	void SetNewBoundingRectangle(CvRect& boundariesRectangle, Ball& currentBall,
			Ball& previousBall, int xAxisLimit, int yAxisLimit);

	void SetHeadMoveInLastFrame(bool headMove);
	void DetectFaceUp(Pole& pole);

	void DetectGreenLimits(IplImage* hsvImage);
	void FixGreenLimitByFIR();

	bool IsItOrangePixel(uint8_t* imagePtr, bool BallWasDetectedAlready);
	//////////////////////////
	void TheNewDetectFieldPoles(IplImage* image, Pole& firstPole,
			Pole& secondPole, Pole& thirdPole);
	void InitializeFieldPolesDetection(Pole& firstPole, Pole& secondPole,
			Pole& localizationPole);
	void DetectPotentialColumns(IplImage* image);
	void MergeColumnsToSpots(IplImage* image);
	void FindStartOfPoles(IplImage* image);
	void DetectPoles(IplImage* image);
	void CheckPoleRules(IplImage* image);
	void FixHighestLocalizationPoint(IplImage* image);
	void CalculateSignificantPolePoints(IplImage* image);
	void IdentifyPoles(IplImage* image, Pole& firstPole, Pole& secondPole,
			Pole& localizationPole);
	void IdentifySideOfGoalPoles(IplImage* image, Pole& firstPole,
			Pole& secondPole);

	HeadScanForGoal_e GetPolesDetectionStatus();

	~ObjectDetector();

};

#endif /* _OBJECT_DETECTOR_H_*/

