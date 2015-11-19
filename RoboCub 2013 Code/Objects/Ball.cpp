
#include "Ball.h"
#include "IncludesFile.h"
#include "Vision.h"
#define PI 3.14159265

//#define MIN(a,b) ((a) < (b) ? (a) : (b))

double Ball::PixelsToCMsArray[] =
/* new table
{ 	-1,		// 0 pixels
	400,	400,	400,	400,	400,	400,	400,	360,	330,	300, //1->10
	250,	230,	210,	180,	165,	150,	135,	120,	115,	110, //11->20
	105,	100,	95,		90,		85,		80,		75,		70,		68,		65, //21->30
	63,		62,		60,		58,		57,		55,		53,		50,		49,		47,	//31
	45,		43,		42,		41,		40,		39,		39,		38,		37,		36,	//41
	35,		33,		31,		30,		29,		28,		27,		26,		25,		24, //51
	24,		23,		22,		21,		20,		18,		17,		16,		16,		16, //61->70
	15,		14,		12,		11,		10,		7,		7,		7,		7,		7
};
*/ //old table
{ 	-1,		// 0 pixels
	400,	380,	360,	340,	300,	262,	230,	200,	187,	167, //1->10
	157,	147,	130,	123,	116,	110,	102,	97,		92,		87, //11->20
	83,		80,		76,		74,		70,		68,		64,		62,		59,		56, //21->30
	54,		52,		50,		46,		44,		43,		42,		40,		39,		37,	//31
	36,		35,		34,		33,		31,		30,		29,		28,		26,		25,	//41
	25,		23,		22,		21,		20,		19,		18,		18,		17,		17, //51
	15,		14,		13,		12,		11,		10,		8,		7,		7,		7, //61->70
	7,		7,		7,		7,		7,		7,		7,		7,		7,		7 //71->80
};

/*****************************************************************************************
 * Function Name: Constructor
 * Description: This Constructor initializes some arguments of the Ball
 * Return Value: None.
 *****************************************************************************************/
Ball::Ball(int ballFirstRow):
				m_valid(false),
				m_ballFirstRow(ballFirstRow),
				m_ballLastRow(-1),
				m_maxNumOfPixelsInARow(0),
				m_maxNumOfOrangePixelsInARow(0),
				m_firstOrangePixelPositionInPreviousRow(-1),
				m_lastOrangePixelPositionInPreviousRow(-1),
				m_numberOfOrangePixelsInCurrentRow(0),
				m_numberOfOrangePixelsInPreviousRow(-1),
				m_correctBallShapeTests(0),
				m_incorrectBallShapeTests(0),
				m_numberOfUpsFIR(3),
				m_currentlyInRise(true),
				m_numOfPixelsInBall(0)
{

}

/*****************************************************************************************
 * Function Name: UpdateBallCharacteristics
 * Description: This Function updates some arguments of the ball
 * Return Value: row and col of the orange pixel.
 *****************************************************************************************/
void Ball::UpdateBallCharacteristics(int row, int col)
{
	m_valid = true;												// sets the ball candidate to valid
	m_numberOfOrangePixelsInCurrentRow++;						// increases the number of pixels in the row

	if (m_firstOrangePixelAppearanceInCurrentRow == false)		// if the ball's candidate already has a first pixel in the current row
	{
		m_firstOrangePixelPositionInCurrentRow = col;			// sets the first orange pixel in the row
		m_firstOrangePixelAppearanceInCurrentRow = true;		// sets the first orange pixel in the row flag to true
	}

	m_lastOrangePixelPositionInCurrentRow = col;				// updates the last pixel in the row
}



double Ball::CalculateDistanceFromFrameCenter_ByBallDiameter()
{
	int placeInArray = MIN(m_diameter, 80); //I found that the cosinus is not helpful here

	placeInArray = MAX(placeInArray,0);

	double distance = PixelsToCMsArray[placeInArray];

	//interpolation to distance ball table
	if (distance >= MIN_DISTANCE_CM_THRESHOLD_TO_RETURN_DISTANCE_BY_DIAMETER)
	{
		double diameterByNumOfPixels = sqrt((m_numOfPixelsInBall/PI))*2;

		int farDistance 	= PixelsToCMsArray[int(floor(diameterByNumOfPixels))];
		int closerDistance  = PixelsToCMsArray[int(ceil(diameterByNumOfPixels))];

		distance = farDistance - (farDistance - closerDistance)*(diameterByNumOfPixels - floor(diameterByNumOfPixels));
	}
	return distance;
}

void Ball::setAngles()
{
	m_PicHorizonAngle= (m_center.X-160)*pixel2angleHorizon;
	m_picVerticalAngle=(120-m_center.Y)*pixel2angleVertical;

	m_headHorizonAngle  = Vision::GetInstance()->getCurrPicPan();
	m_headVerticalAngle = Vision::GetInstance()->getCurrPicTilt();
}

