/*
* File :   BallCandidatesList.cpp
* Authors : Guy
*
* Created on December 30, 2011
*
* Description: This class holds the Balls List.
*				Will use it in ObjectDetector to Add & Remove Ball candidates.
*/

#include "BallCandidatesList.h"

#define ABS(x)	( (x) > 0 ? (x) : -(x) )
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

/*****************************************************************************************
 * Function Name: (constructor)
 * Description: This constructor initializes the Ball Candidates List.
 *****************************************************************************************/
BallCandidatesList::BallCandidatesList()
{
	m_head = NULL;
	m_tail = NULL;
	m_numberOfElements = 0;
}

/*****************************************************************************************
 * Function Name: (destructor)
 * Description: This destructor deletes all the Ball candidate list
 *****************************************************************************************/
BallCandidatesList::~BallCandidatesList()
{
	RemoveAllBallCandidates();
}


/*****************************************************************************************
 * Function Name: GetNumberOfBallCandidates
 * Description: This Function returns the number of ball candidates.
 * Return Value: returns the number of ball candidates.
 *****************************************************************************************/
int BallCandidatesList::GetNumberOfBallCandidates()
{
	return m_numberOfElements;
}

/*****************************************************************************************
 * Function Name: AddBallCandidate
 * Description: This Function adds a new ball candidate to the ball candidates list.
 * Return Value: None.
 *****************************************************************************************/
void BallCandidatesList::AddBallCandidate(Ball* newBallCandidate)
{
	if (m_head == NULL)
	{
		m_head = newBallCandidate;
		newBallCandidate->m_prev = NULL;
	}
	else
	{
		m_tail->m_next = newBallCandidate;
		newBallCandidate->m_prev = m_tail;
	}

	m_tail = newBallCandidate;
	newBallCandidate->m_next = NULL;

	m_numberOfElements++;
}

/*****************************************************************************************
 * Function Name: RemoveBallCandidate
 * Description: This Function removes a ball candidate from the ball candidates list.
 * Return Value: None.
 *****************************************************************************************/
void BallCandidatesList::RemoveBallCandidate(Ball* ballCandidateToRemove)
{
	Ball* nextBallCandidate = ballCandidateToRemove->m_next;
	Ball* prevBallCandidate = ballCandidateToRemove->m_prev;

	if (prevBallCandidate == NULL)	// ball candidate is head
	{
		m_head = nextBallCandidate;
	}
	else
	{
		prevBallCandidate->m_next = nextBallCandidate;
	}
	if (nextBallCandidate == NULL)	// ball candidate is tail
	{
		m_tail = prevBallCandidate;
	}
	else
	{
		nextBallCandidate->m_prev = prevBallCandidate;
	}


	ballCandidateToRemove->m_next = NULL;
	ballCandidateToRemove->m_prev = NULL;
	delete ballCandidateToRemove;
	m_numberOfElements--;
}


/*****************************************************************************************
 * Function Name: RemoveAllBallCandidates
 * Description: This Function removes all the ball candidates from the list.
 * Return Value: None.
 *****************************************************************************************/
void BallCandidatesList::RemoveAllBallCandidates()
{
	Ball* ballToRemove;
	Ball* iterator = m_head;
	int m_numberOfElementsToRemove = m_numberOfElements;

	for (int i = 0; i < m_numberOfElementsToRemove; i++)
	{
		ballToRemove = iterator;
		iterator = iterator->m_next;
		RemoveBallCandidate(ballToRemove);
	}
}

/*****************************************************************************************
 * Function Name: InitializeListForNewRow
 * Description: This Function initializes all the ball candidates for the new row.
 * It initializes the parameters m_valid, m_firstOrangePixelAppearanceInCurrentRow and
 * m_numberOfOrangePixelsInCurrentRow.
 * Return Value: None.
 *****************************************************************************************/
void BallCandidatesList::InitializeListForNewRow()
{
	Ball* iterator = m_head;

	for (int i = 0; i < m_numberOfElements; i++)
	{
		iterator->m_numberOfOrangePixelsInCurrentRow = 0;				// initialize the number of orange pixel in the current row
		iterator->m_firstOrangePixelAppearanceInCurrentRow = false;		// initialize the first orange pixel flag in the current row
		iterator->m_valid = false;										// initialize valid flag

		iterator = iterator->m_next;
	}

}

/*****************************************************************************************
 * Function Name: FindMatchingBallCandidate
 * Description: This Function returns the first close shape to a pixel.
 *				It finds the first shape that is in under the maximum distance defined.
 * Return Value: Ball* - the matching ball candidate.
 *****************************************************************************************/
Ball* BallCandidatesList::FindNearbyBallCandidate(int row, int col)
{
	Ball* iterator = m_head;

	for (int i = 0; i < m_numberOfElements; i++)
	{
		// Checks if the pixel is under some distance from the shape
		if ( (col > iterator->m_firstOrangePixelPositionInPreviousRow - MIN_DISTANCE_FROM_SHAPE_X) &&
			 (col < iterator->m_lastOrangePixelPositionInPreviousRow + MIN_DISTANCE_FROM_SHAPE_X) &&
			 ( ABS(row - iterator->m_ballLastRow) < MIN_DISTANCE_FROM_SHAPE_Y) )
		{
			// found the shape
			break;
		}

		iterator = iterator->m_next;
	}

	// no shape was within the distance from the pixel
	if (iterator == NULL)
	{
		iterator = new Ball(row);			// allocate a new ball candidate
		AddBallCandidate(iterator);			// adds the ball to the list

		//	cout << "DBG new shape, total - ballCandidates_num_shaps = "<<ballCandidates_num_shaps << endl;
	}

	return iterator;
}

/*****************************************************************************************
 * Function Name: UpdateValidBallCandidatesForCurrentRow
 * Description: This function updates the ball shapes that were changed in the current row.
 * 				It updates their tests and m_lastOrangePixelPositionInLastRow, m_numberOfOrangePixelsInLastRow,
 * 				m_firstOrangePixelPositionInLastRow.
 * Return Value: None.
 *****************************************************************************************/
void BallCandidatesList::UpdateValidBallCandidatesForCurrentRow(int row)
{
	Ball* iterator = m_head;					// iterator, goes over all the list

	for (int i = 0; i < m_numberOfElements; i++)
	{
		if (iterator->m_valid == true)			// updates only if the shape was changed in the last row.
		{
			// update ball's characteristics
			int numberOfTotalPixelsInCurrentRow  = iterator->m_lastOrangePixelPositionInCurrentRow -
													iterator->m_firstOrangePixelPositionInCurrentRow + 1;
			if (numberOfTotalPixelsInCurrentRow >= iterator->m_maxNumOfPixelsInARow)
			{
				iterator->m_maxNumOfPixelsInARow = numberOfTotalPixelsInCurrentRow;
				iterator->m_rowOfMaxNumOfPixels = row;
			}

			if (iterator->m_numberOfOrangePixelsInCurrentRow > iterator->m_maxNumOfOrangePixelsInARow)	// if the number of pixels in the row is the max by now
			{
				iterator->m_maxNumOfOrangePixelsInARow = iterator->m_numberOfOrangePixelsInCurrentRow;	// sets the new maximum
				iterator->m_radius =  iterator->m_maxNumOfOrangePixelsInARow/2.0;							// sets the new radius
				iterator->m_center.Y  = row;														// sets the new center's Y coordinate
				iterator->m_center.X  = (iterator->m_firstOrangePixelPositionInCurrentRow + iterator->m_lastOrangePixelPositionInCurrentRow)/2; // sets the new center's X coordinate
			}

			// Updating FIR
			if (iterator->m_numberOfOrangePixelsInCurrentRow >= iterator->m_numberOfOrangePixelsInPreviousRow) // num of pixels greater from the last row
			{
				if (iterator->m_numberOfUpsFIR != 3 )	// increase the number of ups of the FIR
				{
					iterator->m_numberOfUpsFIR++;
				}

				if (iterator->m_numberOfUpsFIR >= 2)	// sets the currentlyInRise flag
				{
					iterator->m_currentlyInRise = true;
				}

				// Checking rule of sequence of number of pixels in row
				if(iterator->m_currentlyInRise == true)
				{
					iterator->m_correctBallShapeTests++;	// currentlyInRise && increased number of pixels => correctTest++
				}
				else
				{
					iterator->m_incorrectBallShapeTests++;	// not in rise && increased number of pixels => incorrectTest++
				}
			}
			else	// m_numberOfOrangePixelsInCurrentRow < m_numberOfOrangePixelsInLastRow
			{
				if (iterator->m_numberOfUpsFIR != 0 )	// decrease the number of ups in the FIR
				{
					iterator->m_numberOfUpsFIR--;
				}

				if (iterator->m_numberOfUpsFIR <= 1)	// sets currently in raise flag to decreasing
				{
					iterator->m_currentlyInRise = false;
				}

				// Checking rule of sequence of number of pixels in row
				if(iterator->m_currentlyInRise == false)
				{
					iterator->m_correctBallShapeTests++;	// currently in decrease && decreased number of pixels => correctTest++
				}
				else
				{
					iterator->m_incorrectBallShapeTests++;	// currently in increase && decreased number of pixels => incorrectTest++
				}
			}

			//checking right order of first orange pixel in current and last row
			if(iterator->m_firstOrangePixelPositionInCurrentRow <= iterator->m_firstOrangePixelPositionInPreviousRow)
			{
				if(iterator->m_currentlyInRise == true)
				{
					iterator->m_correctBallShapeTests++;	// if in rise && first pixel now is more lefty than previous one => correctTest++
				}
				else
				{
					iterator->m_incorrectBallShapeTests++;	// if in rise && first pixel now is more righty than previous one => incorrectTest++
				}
			}
			else	// m_firstOrangePixelPositionInCurrentRow > m_firstOrangePixelPositionInLastRow
			{
				 if (iterator->m_currentlyInRise == false)
				 {
					 iterator->m_correctBallShapeTests++;	// if in decrease && first pixel now is more righty than previous one => correctTest++
				 }
				 else
				 {
					 iterator->m_incorrectBallShapeTests++; // if in decrease && first pixel now is more lefty than previous one => incorrectTest++
				 }
			}

			// checking the last pixel of the shape in the row
			if(iterator->m_lastOrangePixelPositionInCurrentRow >= iterator->m_lastOrangePixelPositionInPreviousRow)
			{
				if(iterator->m_currentlyInRise == true)
				{
					iterator->m_correctBallShapeTests++;	// if in rise && last pixel now is more righty than previous one => correctTest++
				}
				else
				{
					iterator->m_incorrectBallShapeTests++; // if in rise && last pixel now is more lefty than previous one => incorrectTest++
				}

			}
			else // m_lastOrangePixelPositionInCurrentRow < m_lastOrangePixelPositionInLastRow
			{
				if (iterator->m_currentlyInRise == false)
				{
					iterator->m_correctBallShapeTests++;	// if in decrease && last pixel now is more lefty than previous one => correctTest++
				}
				else
				{
					iterator->m_incorrectBallShapeTests++;	// if in decrease && last pixel now is more righty than previous one => correctTest++
				}
			}

			iterator->m_firstOrangePixelPositionInPreviousRow  = iterator->m_firstOrangePixelPositionInCurrentRow;	// update the first orange pixel position
			iterator->m_numberOfOrangePixelsInPreviousRow = iterator->m_numberOfOrangePixelsInCurrentRow;			// update the number of orange pixels in the row
			iterator->m_lastOrangePixelPositionInPreviousRow = iterator->m_lastOrangePixelPositionInCurrentRow;		// update the last orange pixel position
			iterator->m_ballLastRow = row;			// update the last row of the shape

			iterator->m_numOfPixelsInBall += iterator->m_lastOrangePixelPositionInCurrentRow - iterator->m_firstOrangePixelPositionInCurrentRow + 1;
		}

		iterator = iterator->m_next;
	}
}


/*****************************************************************************************
 * Function Name: CalculateGreenPixelsRatio
 * Description: This function computes the green pixels ratio for every ball candidate
 *				in the list.
 *				The orange pixels are excluded from the ratio.
 *				The green pixels ratio goes like this:
 *					greenPixels/NonOrangePixels in the bounding square (bounding the circle).
 * Return Value: None.
 *****************************************************************************************/
void BallCandidatesList::CalculateGreenPixelsRatio(IplImage* segmentedHSVFrame,Ball* iterator)
{
	int nonOrangePixels;	// the non orange pixels in the square
	int greenPixels;		// the green pixels in the square

	int squareTopLeftPointRow, squareTopLeftPointCol;			// coordinates of the top-left point of the bounding square
	int squareBottomRightPointRow, squareBottomRightPointCol;	// coordinates of the bottom-right point of bounding square

	int greenExtraBoundary;		// the extra boundary of the square, apart from the ball's radius

	uchar* imagePtr;			// the image pointer
	int row, col;				// loop variables
	uchar hue,sat,val;					// hue of segmented HSV frame


	nonOrangePixels = 0;	// initialize non orange pixels counter for current ball candidate
	greenPixels = 0;		// initialize green pixels counter for current ball candidate

	greenExtraBoundary = MIN(iterator->m_radius/2, MIN_GREEN_EXTRA_BOUNDARY); // determine the square boundary

	// determining the top left point of the square
	squareTopLeftPointRow = iterator->m_center.Y - iterator->m_radius - greenExtraBoundary;
	squareTopLeftPointCol = iterator->m_center.X - iterator->m_radius - greenExtraBoundary;

	// determining the bottom right point of the square
	squareBottomRightPointRow = iterator->m_center.Y + iterator->m_radius + greenExtraBoundary;
	squareBottomRightPointCol = iterator->m_center.X + iterator->m_radius + greenExtraBoundary;

	for(row = squareTopLeftPointRow; row <= squareBottomRightPointRow ; row++ )
	{
		for(col = squareTopLeftPointCol; col <= squareBottomRightPointCol ; col++ )
		{
			// Check boundaries
			if (row >= 0 && row < segmentedHSVFrame->height && col>=0 && col < segmentedHSVFrame->width)
			{
				imagePtr = (uchar*) (segmentedHSVFrame->imageData + row * segmentedHSVFrame->widthStep + 3*col);
				hue = *(imagePtr);		// H component
				sat = *(imagePtr+1);
				val = *(imagePtr+2);

				if(hue == GREEN_AVG_HUE && sat == MAX_SAT && val == MAX_VAL)
				{
					greenPixels++;
				}
				if(hue != ORANGE_AVG_HUE && sat != MAX_SAT && val != MAX_VAL)
				{
					nonOrangePixels++;
				}
			}
		}
	}

	// Calculate green pixels ratio
	iterator->m_greenRatio = (float)greenPixels/nonOrangePixels;



}

/*****************************************************************************************
 * Function Name: FindBestMatchingBall
 * Description:  This function finds the best matching ball from the balls list.
 * Return Value: Ball* - The best matching ball.
 *****************************************************************************************/
Ball* BallCandidatesList::FindBestMatchingBall(bool ballFoundFlag,IplImage* image)
{
	float minRadius;				// minimum radius to pass
	float maxGreenRatio;			// need to be as big as possible
	float minCircleTestsRatio;		// need to be as big as possible
	float maxRadiusRatio;			// need to be as close as possible to 0

	Ball* iterator = m_head;		// iterator, goes over the ball candidates list
	Ball* bestMatchedBall = NULL;	// pointer of best matching ball candidate

	UpdateThresholds(ballFoundFlag, minCircleTestsRatio,maxGreenRatio,maxRadiusRatio,minRadius);

	for (int i = 0; i < m_numberOfElements; i++)
	{
		// Ratios Calculations
		iterator->m_circleTestsRatio = ((float)iterator->m_correctBallShapeTests)/((float)iterator->m_incorrectBallShapeTests) ;
		iterator->m_radiusRatio = ((float)iterator->m_ballLastRow - (float)iterator->m_ballFirstRow +1.0)/((float)iterator->m_maxNumOfPixelsInARow) ;
		iterator->m_radiusRatio = ABS(1.0 - iterator->m_radiusRatio);

		iterator->m_diameter = MAX(( iterator->m_ballLastRow - iterator->m_ballFirstRow + 1), iterator->m_maxNumOfPixelsInARow);
		iterator->m_radius = iterator->m_diameter/2.0;
		if (iterator->m_diameter == iterator->m_maxNumOfPixelsInARow)
		{
			iterator->m_center.Y = iterator->m_rowOfMaxNumOfPixels;
		}
		else
		{
			iterator->m_center.Y = iterator->m_ballFirstRow + (int)(iterator->m_radius) - 1;
		}

		/* Preferring the best ball candidate.
			Needs to pass the next calculations:
				Passing minimum radius ratio threshold
				Passing minimum ball shape tests threshold
				Passing the minimum radius threshold
				Passing the maximum radius threshold
				Being the ball shape with the best green percentage
		*/
		if( iterator->m_radiusRatio < maxRadiusRatio &&
			iterator->m_circleTestsRatio >= minCircleTestsRatio && iterator->m_radius < MAX_RADIUS &&
			iterator->m_radius >= minRadius && iterator->m_numOfPixelsInBall > MIN_NUM_OF_PIXELS_BALL_NOT_DETECTED)
		{
			//Max_Ratio_rules = Ratio_rules;
			CalculateGreenPixelsRatio(image,iterator);

			if(iterator->m_greenRatio > maxGreenRatio)
			{
				maxGreenRatio = iterator->m_greenRatio;	// searching for the ball candidate with the greatest green pixels percentage
				bestMatchedBall = iterator;

				/*
				cout << "**********************************" << endl;
				cout << "Ball number = " << i << endl;
				cout << "Radius = " << iterator->m_radius << ", x = " << iterator->m_center.X << " y = " << iterator->m_center.Y << endl;
				cout << "rules_ratio = " << iterator->m_circleTestsRatio << endl;
				cout << "green_ratio - " << iterator->m_greenRatio << endl;
				cout << "radius_ratio - " << iterator->m_radiusRatio << endl;
				cout << "Num of pixels for ball = " << iterator->m_numOfPixelsInBall << endl;
				*/
			}
		}

		iterator = iterator->m_next;
	}

	return bestMatchedBall;
}


/*****************************************************************************************
 * Function Name: FindBestMatchingBall
 * Description:  This function updates the thresholds.
 * 					If a ball was found, the thresholds are loosen
 * 					If a ball wasn't found, the thresholds are tighter
 * Return Value: None.
 *****************************************************************************************/
void BallCandidatesList::UpdateThresholds(bool ballFoundFlag, float& minCircleTestsRatio,float& maxGreenRatio,float& maxRadiusRatio,float& minRadius)
{
	if(ballFoundFlag == true)	// loosen thresholds
	{
		minCircleTestsRatio	= MIN_CIRCLE_TESTS_RATIO_BALL_DETECTED;
		maxGreenRatio   	= MIN_GREEN_RATIO_BALL_DETECTED;
		maxRadiusRatio		= MAX_RADIUS_RATIO_BALL_DETECTED;
		minRadius 			= MIN_RADIUS_BALL_DETECTED;
	}
	else						// tighter thresholds
	{
		minCircleTestsRatio	= MIN_CIRCLE_TESTS_RATIO_BALL_NOT_DETECTED;
		maxGreenRatio		= MIN_GREEN_RATIO_BALL_NOT_DETECTED;
		maxRadiusRatio 		= MAX_RADIUS_RATIO_BALL_NOT_DETECTED;
		minRadius			= MIN_RADIUS_BALL_NOT_DETECTED;
	}
}


void BallCandidatesList::RemoveFalseBallsByNow(int row)
{
	float minRadius;				// minimum radius to pass
	float maxGreenRatio;			// need to be as big as possible
	float minCircleTestsRatio;		// need to be as big as possible
	float maxRadiusRatio;			// need to be as close as possible to 0

	Ball* iterator = m_head;		// iterator, goes over the ball candidates list
	Ball* ballToRemove;

	UpdateThresholds(false, minCircleTestsRatio,maxGreenRatio,maxRadiusRatio,minRadius);

	int i = 0;

	while (iterator != NULL)
	{
		if ((row - iterator->m_ballLastRow > 3) && (iterator->m_ballLastRow != -1))
		{
			iterator->m_circleTestsRatio = ((float)iterator->m_correctBallShapeTests)/((float)iterator->m_incorrectBallShapeTests) ;
			iterator->m_radiusRatio = ((float)iterator->m_ballLastRow - (float)iterator->m_ballFirstRow +1.0)/ ((float)iterator->m_maxNumOfOrangePixelsInARow) ;
			iterator->m_radiusRatio = ABS(1.0 - iterator->m_radiusRatio);

			if (iterator->m_radius < minRadius || iterator->m_radius > MAX_RADIUS || iterator->m_circleTestsRatio < minCircleTestsRatio || iterator->m_radiusRatio >= maxRadiusRatio)
			{
				//cout << "Removing Ball Candidate. Number of Ball Candidates before removal = " << m_numberOfElements << " Removing Ball ID = " << i+1 << endl;
				//cout << "Row = " << row << endl;

				//cout << "Radius = "<< iterator->m_radius << endl;
				//cout << "Radius Ratio = " << iterator->m_radiusRatio << endl;
				//cout << "Correct/Incorrect Ratio = "<< iterator->m_circleTestsRatio << endl;

				ballToRemove = iterator;
				iterator = iterator->m_next;
				RemoveBallCandidate(ballToRemove);
				continue;
			}
		}
		i++;
		iterator = iterator->m_next;
	}
}
