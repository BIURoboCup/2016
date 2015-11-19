/*
 * Pole.h
 *
 *  Created on: Jan 19, 2012
 *      Author: Guy
 */

#ifndef POLE_H_
#define POLE_H_

#include "IncludesFile.h"

#include "Point.h"
#include "Trendline.h"
#include "VisionEnums.h"
#include "Camera.h"


using namespace Robot;

extern const double pixel2angleHorizon ;
extern const double pixel2angleVertical;


/*Step 4 pole detection*/
#define POLE_MIN_HEIGHT_FOR_AVG_AND_SLOPE_CHECK   15
#define POLE_WIDTH_OFFSET_FOR_AVG_CHECK 12
#define POLE_MAX_DISTANCE_FROM_TRENDLINE 8

/*Step 5 pole detection*/
#define POLE_WIDTH_BIG_AREA	29
#define POLE_WIDTH_MEDIUM_AREA	23
#define POLE_WIDTH_SMALL_AREA	16

#define MAX_STD_FOR_POLE_WIDTH_BIG_AREA 4
#define MAX_STD_FOR_POLE_WIDTH_MEDIUM_AREA 3.5
#define MAX_STD_FOR_POLE_WIDTH_SMALL_AREA 3

//fix highest point
#define NUM_OF_PIXELS_FROM_MIDDLE_POLE_FOR_POLE_HEIGHT 	2
#define NUM_OF_ROWS_WITHOUT_VALID_COLOR 				2

/*Step 6 pole detection*/
#define	GOAL_POST_COLORED_PIXELS_THRESHOLD	0.9
#define MIN_COLORED_PIXELS_RATIO_GOAL_THRESHOLD 0.66
#define MIN_COLORED_PIXELS_RATIO_LOCALIZATION_POLE_THRESHOLD 0.7
#define LOCALIZATION_POLE_IDENTIFICATION_SQUARE_SIZE 2
#define MIN_THRESHOLD_COLOR_PERCENTAGE_FOR_LOCALIZATION_POLE 0.2

class Pole
{
	friend class PoleCandidatesList;
	friend class ObjectDetector;
	friend class Vision;
	friend class PoleHandler;

private:
	bool 	m_dataValid;

	int     m_highestPixelPosition;					// the highest pixel of the pole
	int     m_lowestPixelPosition;					// the lowest pixel of the pole
	int		m_poleHeight;							// the height of the pole

	int 	m_WidthArray[Camera::HEIGHT];
	int 	m_sumOfPoleWidths;
	float   m_averageOfPoleWidth;
	float   m_STDOfPoleWidth;

	Point2DInt_t 	m_centerOfMinPointSearch;
	Point2DInt_t 	m_poleStartingPoint;

	Trendline		m_poleTrendline;

	Point2D			m_localizationTopPoint;
	Point2D			m_localizationMiddlePoint;
	Point2D			m_localizationBottomPoint;
	double			m_localizationEuclideanHeight;

	int  		m_lastPixelPositionInCurrentRow;
	int 		m_firstPixelPositionInCurrentRow;

	PoleType_e	 	m_poleColor;
	GoalPoleSide_e 	m_poleSide;

	int		m_numOfBluePixels;
	int		m_numOfYellowPixels;
	int		m_numOfTotalPixels;

	Pole*	m_next;										// the next ball candidate in the list
	Pole*	m_prev;										// the prev ball candidate in the list

public:

	Pole(int col = -1,int row = -1);
	~Pole();

	void UpdatePoleCharacteristics(int col, int pixelColor);

	void CalculatePoleHeight();
	bool ValidPoleWidthSTDForSpecificWidth();

	void InitializePoleForNewRow();
	bool UpdatePoleForLastRow(int row,Point2DInt_t *currentCenter);

	bool CheckTypeOfPole(unsigned char* image);
	void FixHighestLocalizationPointForPole(unsigned char* image);
	bool CheckColoredPixelPercentage();
	bool CheckTypeOfLocalizationPole(unsigned char*  image);

	double getXAngle() {return (m_localizationMiddlePoint.X-160)*pixel2angleHorizon;}
};






#endif /* POLE_H_ */
