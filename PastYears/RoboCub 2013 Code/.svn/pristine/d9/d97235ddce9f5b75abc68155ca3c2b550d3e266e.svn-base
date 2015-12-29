/*
 * Pole.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: Alon King
 */


#include "Pole.h"

#define ABS(x)	( (x) > 0 ? (x) : -(x) )
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

Pole::Pole(int col,int row):
			m_dataValid(false),
			m_highestPixelPosition(Camera::HEIGHT),
			m_lowestPixelPosition(0),
			m_averageOfPoleWidth(0),
			m_poleSide(UNKNOWN_POLE_SIDE),
			m_numOfBluePixels(0),
			m_numOfYellowPixels(0),
			m_numOfTotalPixels(0)
{
	m_centerOfMinPointSearch.x = col;
	m_centerOfMinPointSearch.y = row;
	m_sumOfPoleWidths = 0;

	memset(m_WidthArray,0,sizeof(int)*Camera::HEIGHT);

}

Pole::~Pole()
{

}

void Pole::UpdatePoleCharacteristics(int col, int pixelColor)
{

	if(m_lastPixelPositionInCurrentRow < col )	// highest pixel
	{
		m_lastPixelPositionInCurrentRow = col;
	}

	if(m_firstPixelPositionInCurrentRow >  col )	// lowest pixel
	{
		m_firstPixelPositionInCurrentRow = col;
	}

	if(pixelColor == BLUE_AVG_HUE)
	{
		m_numOfBluePixels++;

	}
	else if (pixelColor == YELLOW_AVG_HUE)
	{
		m_numOfYellowPixels++;

	}

}


void Pole::CalculatePoleHeight()
{
	m_poleHeight = m_lowestPixelPosition - m_highestPixelPosition + 1 ;
}

bool Pole::ValidPoleWidthSTDForSpecificWidth()
{
	float 	sigmaSTD = 0;
	int		lowestPixelPositionCol_save = m_lowestPixelPosition;


	while (lowestPixelPositionCol_save >= m_highestPixelPosition)
	{
		if(m_WidthArray[lowestPixelPositionCol_save] != 0)
		{
			sigmaSTD += (m_WidthArray[lowestPixelPositionCol_save] - m_averageOfPoleWidth) *
						(m_WidthArray[lowestPixelPositionCol_save] - m_averageOfPoleWidth);
		}

		lowestPixelPositionCol_save--;
	}

	m_STDOfPoleWidth = (float) sqrt(sigmaSTD/m_poleHeight);


	if (m_averageOfPoleWidth < POLE_WIDTH_SMALL_AREA && m_STDOfPoleWidth < MAX_STD_FOR_POLE_WIDTH_SMALL_AREA)
	{
		return true;
	}

	if (m_averageOfPoleWidth < POLE_WIDTH_MEDIUM_AREA && m_averageOfPoleWidth >= POLE_WIDTH_SMALL_AREA
		&& m_STDOfPoleWidth < MAX_STD_FOR_POLE_WIDTH_MEDIUM_AREA)
	{
		return true;
	}

	if (m_averageOfPoleWidth < POLE_WIDTH_BIG_AREA && m_averageOfPoleWidth >= POLE_WIDTH_MEDIUM_AREA
		&&	m_STDOfPoleWidth < MAX_STD_FOR_POLE_WIDTH_BIG_AREA)
	{
		return true;
	}

	return false;


	//m_STDOfPoleWidth = m_averageOfPoleWidth/m_STDOfPoleWidth;
}


bool Pole::CheckTypeOfPole(unsigned char*  image)
{
	int blueAndYellowPixels = m_numOfBluePixels + m_numOfYellowPixels;
	bool isItValidPole = false;

	double bluePixelsPercentage = (double)m_numOfBluePixels/blueAndYellowPixels;
	double yellowPixelsPercentage = (double)m_numOfYellowPixels/blueAndYellowPixels;


	if (bluePixelsPercentage > GOAL_POST_COLORED_PIXELS_THRESHOLD)
	{
		m_poleColor = BLUE_GOAL;
		isItValidPole = true;
	}
	else if (yellowPixelsPercentage > GOAL_POST_COLORED_PIXELS_THRESHOLD)
	{
		m_poleColor = YELLOW_GOAL;
		isItValidPole = true;
	}
	else
	{
		isItValidPole = CheckTypeOfLocalizationPole(image);
	}

	return isItValidPole;
}


void Pole::FixHighestLocalizationPointForPole(unsigned char* image)
{
	unsigned char* imagePtr;			// the image pointer
	int middleRow, middleCol;				// loop variables
	unsigned char hue,sat,val;					// hue of segmented HSV frame

	bool endofShape = false;
	Point2DInt_t newTrendlinePoint;
	int			unValidRow = 0;

	middleRow = m_poleTrendline.m_points[m_poleTrendline.GetNumOfPoints()-1].x;

	while(endofShape == false)
	{
		middleCol = m_poleTrendline.PredictPoint(middleRow);

	//	cout << "middleRow = " << middleRow << " middleCol = " << middleCol << "\n";
		for (int col = middleCol - NUM_OF_PIXELS_FROM_MIDDLE_POLE_FOR_POLE_HEIGHT; col <= middleCol + NUM_OF_PIXELS_FROM_MIDDLE_POLE_FOR_POLE_HEIGHT; col ++)
		{
			if(middleRow < 0  || middleRow >= Camera::HEIGHT || col < 0 || col >= Camera::WIDTH)
			{
				endofShape = true;
				break;
			}

			imagePtr = (unsigned char*) (image + middleRow * Camera::WIDTH * 3 + 3*col);

			hue = *(imagePtr);
			sat = *(imagePtr+1);
			val = *(imagePtr+2);

			if ((hue == BLUE_AVG_HUE || hue == YELLOW_AVG_HUE)  &&  sat == MAX_SAT && val == MAX_VAL)
			{
				break;
			}
			else
			{
				if (col == middleCol + NUM_OF_PIXELS_FROM_MIDDLE_POLE_FOR_POLE_HEIGHT)
				{
					if(unValidRow == NUM_OF_ROWS_WITHOUT_VALID_COLOR)
					{
						endofShape = true;
						break;
					}
					else
					{
						unValidRow++;
					}
				}
			}
		}

		newTrendlinePoint.x = middleRow;
		newTrendlinePoint.y = middleCol;

		m_poleTrendline.AddPoint(&newTrendlinePoint);

		middleRow--;

		if (m_poleTrendline.GetNumOfPoints() == Camera::HEIGHT - 1)
		{
			break;
		}

	}

	m_highestPixelPosition = MIN(middleRow+NUM_OF_ROWS_WITHOUT_VALID_COLOR+1,Camera::HEIGHT-1);

}
bool Pole::CheckTypeOfLocalizationPole(unsigned char*  image)
{

	int squareTopLeftPointRow, 	squareTopLeftPointCol;			// coordinates of the top-left point of the bounding square
	int squareBottomRightPointRow, squareBottomRightPointCol;	// coordinates of the bottom-right point of bounding square

	unsigned char* imagePtr;			// the image pointer
	int row, col;				// loop variables
	unsigned char hue,sat,val;					// hue of segmented HSV frame

	Point2DInt_t squareMiddlePoint;

	int numOfTotalPixels, numOfBluePixels, numOfYellowPixels;

	float bluePixelsPercentage[3];
	float yellowPixelsPercentage[3];


	for (int i = 0; i < 3; i++)
	{
		numOfTotalPixels = 0;
		numOfBluePixels = 0;
		numOfYellowPixels = 0;

		if(i == 0)
		{
		//	printf("Check Type of LOCALIZATION pole - 0\n");
			squareMiddlePoint.y = (int)m_localizationBottomPoint.Y;
			squareMiddlePoint.x = m_poleTrendline.m_points[3].y; // this is not a bug ,look at comments in Trendline class
		}
		else if(i == 1)
		{
			//s	printf("Check Type of LOCALIZATION pole - 2\n");
			squareMiddlePoint.y = (int)m_localizationMiddlePoint.Y;
			squareMiddlePoint.x = (int)m_localizationMiddlePoint.X;

		}
		else
		{
			//	printf("Check Type of LOCALIZATION pole - 1\n");
			squareMiddlePoint.y = (int)m_localizationTopPoint.Y;
			squareMiddlePoint.x = m_poleTrendline.m_points[m_poleTrendline.m_count - 3].y;
		}

		squareTopLeftPointRow = MAX(squareMiddlePoint.y - LOCALIZATION_POLE_IDENTIFICATION_SQUARE_SIZE,0);
		squareTopLeftPointCol = MAX(squareMiddlePoint.x - LOCALIZATION_POLE_IDENTIFICATION_SQUARE_SIZE,0);
		// determining the bottom right point of the square
		squareBottomRightPointRow = MIN(squareMiddlePoint.y + LOCALIZATION_POLE_IDENTIFICATION_SQUARE_SIZE,Camera::HEIGHT-1);
		squareBottomRightPointCol = MIN(squareMiddlePoint.x + LOCALIZATION_POLE_IDENTIFICATION_SQUARE_SIZE,Camera::WIDTH-1);

		for (row = squareTopLeftPointRow; row <= squareBottomRightPointRow ; row++)
		{
			for (col = squareTopLeftPointCol; col < squareBottomRightPointCol; col++)
			{
				imagePtr = (unsigned char*) (image + row * Camera::WIDTH * 3 + 3*col);
				hue = *(imagePtr);
				sat = *(imagePtr+1);
				val = *(imagePtr+2);


				if ( hue == BLUE_AVG_HUE && sat == MAX_SAT && val == MAX_VAL)
				{
					numOfBluePixels++;
				}
				else if ( hue == YELLOW_AVG_HUE && sat == MAX_SAT && val == MAX_VAL)
				{
					numOfYellowPixels++;
				}

				numOfTotalPixels++;

			}
		}

		bluePixelsPercentage[i] = (float)numOfBluePixels/numOfTotalPixels;
		yellowPixelsPercentage[i] = (float)numOfYellowPixels/numOfTotalPixels;

	}

	if (bluePixelsPercentage[0] > MIN_THRESHOLD_COLOR_PERCENTAGE_FOR_LOCALIZATION_POLE &&
		yellowPixelsPercentage[1] > MIN_THRESHOLD_COLOR_PERCENTAGE_FOR_LOCALIZATION_POLE &&
		bluePixelsPercentage[2] > MIN_THRESHOLD_COLOR_PERCENTAGE_FOR_LOCALIZATION_POLE)
	{
		m_poleColor = BYB_POLE;

	//	cout << "BYB bottom = " << bluePixelsPercentage[0] << " middle = " << yellowPixelsPercentage[1] << " top = " << bluePixelsPercentage[2] << "\n";
		return true;
	}
	else if (yellowPixelsPercentage[0] > MIN_THRESHOLD_COLOR_PERCENTAGE_FOR_LOCALIZATION_POLE &&
			bluePixelsPercentage[1] > MIN_THRESHOLD_COLOR_PERCENTAGE_FOR_LOCALIZATION_POLE &&
			yellowPixelsPercentage[2] > MIN_THRESHOLD_COLOR_PERCENTAGE_FOR_LOCALIZATION_POLE)
	{
		m_poleColor = YBY_POLE;

	//	cout << "YBY bottom = " << yellowPixelsPercentage[0] << " middle = " << bluePixelsPercentage[1] << " top = " << yellowPixelsPercentage[2] << "\n";
		return true;
	}
	else
	{
	//	cout << "No Pole bottom = " << yellowPixelsPercentage[0] << " middle = " << bluePixelsPercentage[1] << " top = " << yellowPixelsPercentage[2] << "\n";

		return false;
	}

}

void Pole::InitializePoleForNewRow()
{
	m_lastPixelPositionInCurrentRow = 0;
	m_firstPixelPositionInCurrentRow = Camera::WIDTH - 1;
}

bool Pole::UpdatePoleForLastRow(int row,Point2DInt_t *currentCenter)
{
	m_highestPixelPosition = row;
	CalculatePoleHeight();

	int currentWidth = m_lastPixelPositionInCurrentRow - m_firstPixelPositionInCurrentRow +1;


//	cout <<"Points = "<<m_poleTrendline.GetNumOfPoints()<<" , Predict X= "<< m_poleTrendline.PredictPoint(currentCenter->y)
//	<<"  Center X = "<<currentCenter->x<<"	CurrentWidth = "<<currentWidth<<
//	", avgWidth =  "<<m_averageOfPoleWidth<<endl;



	if (m_poleHeight > POLE_MIN_HEIGHT_FOR_AVG_AND_SLOPE_CHECK)
	{

		if (currentWidth > m_averageOfPoleWidth + POLE_WIDTH_OFFSET_FOR_AVG_CHECK)
		{
			return true;

		}
		else if( ABS(m_poleTrendline.PredictPoint(currentCenter->y)-currentCenter->x) > POLE_MAX_DISTANCE_FROM_TRENDLINE)
		{
			return true;

		}


	}

	m_sumOfPoleWidths += currentWidth;

	m_averageOfPoleWidth = (float) m_sumOfPoleWidths/m_poleHeight;
	m_WidthArray[row]    = currentWidth;

	m_poleTrendline.UpdateTrendline(currentCenter);

	return false;
}



bool Pole::CheckColoredPixelPercentage()
{

	double bluePixelsPercentage = (double)m_numOfBluePixels/m_numOfTotalPixels;
	double yellowPixelsPercentage = (double)m_numOfYellowPixels/m_numOfTotalPixels;


	if (m_poleColor == BLUE_GOAL)
	{
		//cout << "BLUE GOAL. bluePixelsPercentage = " << bluePixelsPercentage << "\n";
		if (bluePixelsPercentage > MIN_COLORED_PIXELS_RATIO_GOAL_THRESHOLD)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (m_poleColor == YELLOW_GOAL)
	{
		//cout << "YELLOW GOAL. yellowPixelsPercentage = " << yellowPixelsPercentage << "\n";

		if (yellowPixelsPercentage > MIN_COLORED_PIXELS_RATIO_GOAL_THRESHOLD)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//cout << "LOCALIZATION GOAL. blue + yellow PixelsPercentage = " << bluePixelsPercentage + yellowPixelsPercentage << "\n";

		if (bluePixelsPercentage + yellowPixelsPercentage > MIN_COLORED_PIXELS_RATIO_LOCALIZATION_POLE_THRESHOLD)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

