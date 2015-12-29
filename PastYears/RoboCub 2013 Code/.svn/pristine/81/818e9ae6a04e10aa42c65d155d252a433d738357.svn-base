#ifndef BALL_H_
#define BALL_H_

#define PIXEL2DEGREE 56.6/320

#define MAX_TOTAL_VERTICAL_ANGLE_TO_SET_DISTANCE_FROM_BALL_BY_DIAMETER 13

#include "IncludesFile.h"
#include "WalkingAndActions.h"
using namespace Robot;

extern const double pixel2angleHorizon ;
extern const double pixel2angleVertical;


#define BALL_REAL_DIAMETER_CMS 6
#define MAX_RADIUS 				40
#define PIXEL2ANGLE 			(54.0/320.0)

#define MIN_DISTANCE_CM_THRESHOLD_TO_RETURN_DISTANCE_BY_DIAMETER 68

class Ball
{
	friend class BallCandidatesList;
	friend class ObjectDetector;
	friend class Vision;
	friend class BallHandler;

private:

	float 	m_radius;				// the radius of the ball
	float   m_diameter;				// the diameter of the ball
	Point2D	m_center;				// the center Point (X,Y) of the ball on the frame
	myTimeval m_timeBallDetected;		// timestamp the ball was detected


	float	m_PicHorizonAngle;
	float	m_picVerticalAngle;
	float	m_headHorizonAngle;
	float	m_headVerticalAngle;

	bool    m_valid;				// boolean, checks if the ball characteristics were changed in the last row

	int 	m_ballFirstRow;			// the first ball of the ball
	int 	m_ballLastRow;			// the last rows of the ball
	int 	m_maxNumOfPixelsInARow;			// maximum number of pixels in a row of the ball
	int 	m_maxNumOfOrangePixelsInARow;			// maximum number of pixels in a row of the ball
	int		m_rowOfMaxNumOfPixels;

	// Ball shape tests members

	bool 	m_firstOrangePixelAppearanceInCurrentRow;	// boolean that saves if there was an orange pixel of the ball in the current row
	int		m_firstOrangePixelPositionInCurrentRow;		// first orange pixel position in the current row
	int		m_firstOrangePixelPositionInPreviousRow;	// first orange pixel position in the previous row

	int		m_lastOrangePixelPositionInCurrentRow;		// last orange pixel position in the current row
	int		m_lastOrangePixelPositionInPreviousRow;		// last orange pixel position in the previous row

	int		m_numberOfOrangePixelsInCurrentRow;			// the number of orange pixels in the current row
	int 	m_numberOfOrangePixelsInPreviousRow;		// the number of orange pixels in the previous row

	int 	m_correctBallShapeTests;					// correct tests for the ball shape
	int 	m_incorrectBallShapeTests;					// incorrect test for the ball shape

	int		m_numberOfUpsFIR;							// number of ups - rows that the number of pixels rises (maximum 3)
	bool	m_currentlyInRise;							// boolean that signifies if currently in rise (number of ups >=2) or down (number of ups <=1)

	float	m_greenRatio;								// the ball's green pixels ratio
	float	m_radiusRatio;								// the ball's radius ratio (need to be as close as possible to 0)
	float 	m_circleTestsRatio;								// the ball's tests ratio (need to be as big as possible)

	int 	m_numOfPixelsInBall;

	Ball*	m_next;										// the next ball candidate in the list
	Ball*	m_prev;										// the prev ball candidate in the list

	static  double PixelsToCMsArray[81];

public:

	// Constructor
	Ball(int ballFirstRow = -1);

	void UpdateBallCharacteristics(int row, int col);
	double CalculateDistanceFromFrameCenter_ByBallDiameter();
	void setAngles();

};



#endif /* BALL_H_ */
