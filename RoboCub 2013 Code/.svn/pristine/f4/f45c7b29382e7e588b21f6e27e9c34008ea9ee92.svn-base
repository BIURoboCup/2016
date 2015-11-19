/*
* File :   BallCandidatesList.h
* Authors : Guy
*
* Created on December 30, 2011
*
* Description: This class holds the Balls List.
*				Will use it in ObjectDetector to Add & Remove Ball candidates.
*/

#ifndef _BALL_CANDIDATES_LIST_
#define _BALL_CANDIDATES_LIST_

#include "Ball.h"
#include "opencv2/opencv.hpp"

#include "Vision/header/ColorsValues.h"

#define MIN_DISTANCE_FROM_SHAPE_X 	8
#define MIN_DISTANCE_FROM_SHAPE_Y	3

#define MIN_GREEN_EXTRA_BOUNDARY 5


// THERSHOLDS
#define MIN_CIRCLE_TESTS_RATIO_BALL_DETECTED	0.7
#define MIN_GREEN_RATIO_BALL_DETECTED  			0.1
#define MAX_RADIUS_RATIO_BALL_DETECTED 			5
#define	MIN_RADIUS_BALL_DETECTED				0.5

#define MIN_CIRCLE_TESTS_RATIO_BALL_NOT_DETECTED	1
#define MIN_GREEN_RATIO_BALL_NOT_DETECTED			0.1
#define MAX_RADIUS_RATIO_BALL_NOT_DETECTED			0.5
#define	MIN_RADIUS_BALL_NOT_DETECTED				1
#define	MIN_NUM_OF_PIXELS_BALL_NOT_DETECTED			8


class BallCandidatesList
{
	Ball* 	m_head;					// head of the Ball's list
	Ball* 	m_tail;					// tail of the Ball's list
	int		m_numberOfElements;		// number of elements in list


public:

	// Constructor
	BallCandidatesList();

	void AddBallCandidate(Ball* newBallCandidate);
	void RemoveBallCandidate(Ball* ballCandidateToRemove);
	void RemoveAllBallCandidates();

	int   GetNumberOfBallCandidates();
	void  InitializeListForNewRow();
	Ball* FindNearbyBallCandidate(int row, int col);
	void  UpdateValidBallCandidatesForCurrentRow(int row);
	void  CalculateGreenPixelsRatio(IplImage* segmentedHSVFrame,Ball* iterator);
	Ball* FindBestMatchingBall(bool Ball_found,IplImage* image);
	void  UpdateThresholds(bool ballFoundFlag, float& minTestsRatio,float& maxGreenRatio,float& maxRadiusRatio,float& minRadius);
	void  RemoveFalseBallsByNow(int row);

	// Destructor
	~BallCandidatesList();
};


#endif /* _BALL_CANDIDATES_LIST_ */
