
#include "ObjectDetector.h"

#define ABS(x)	( (x) > 0 ? (x) : -(x) )
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

bool ObjectDetector::m_debugMode = false;
bool ObjectDetector::m_ballDetected = false;

/*****************************************************************************************
 * Function Name: constructor
 * Description: Builds a new instance of ObjectDetector
 * Return Value: None.
 *****************************************************************************************/
ObjectDetector::ObjectDetector() {
	m_headMoveInLastFrame = false;

}

/*****************************************************************************************
 * Function Name: Destructor
 * Description: Destroys an instance of Object Detector.
 * Return Value: None.
 *****************************************************************************************/
ObjectDetector::~ObjectDetector() {

}
/*****************************************************************************************
 * Function Name: SetHeadMoveInLastFrame
 * Description:
 * Return Value: NONE
 *****************************************************************************************/
void ObjectDetector::SetHeadMoveInLastFrame(bool headMove) {
	m_headMoveInLastFrame = headMove;
}

/*****************************************************************************************
 * Function Name: Detect Ball
 * Description: This Function gets a segmented image and boundaries (as a rectangle)
 * 				and a ball, searches for an orange ball under a green background in the image
 *				and if it finds one, it puts the ball in the ball parameter
 * Return Value: boolean - true or false if found a ball within the boundaries
 *****************************************************************************************/
bool ObjectDetector::DetectBall(IplImage* image, CvRect& boundariesRectangle,
		Ball& ball) {
	int row, col; // iterators
	bool currentlyInShape = false; // flag indicating if we are currently in a shape
	int notOrangeConsecutivePixels = 0; // counts consecutive not orange pixels

	Ball* ballCandidate = NULL; // pointer of current ball shape working on
	Ball* bestMatchedBall; // pointer for the best matching ball

	// hue component
	uchar* imagePtr; // pointer of the image

	bool ballWasDetectedAlready = false;

	if (boundariesRectangle.height != image->height
			|| boundariesRectangle.width != image->width) {
		ballWasDetectedAlready = true;
	}

	for (row = boundariesRectangle.y;
			row < boundariesRectangle.y + boundariesRectangle.height; row++) {
		currentlyInShape = false; // new row, currently not in shape

		m_ballCandidatesList.InitializeListForNewRow(); // initialize the list for the new row

		for (col = boundariesRectangle.x;
				col < boundariesRectangle.x + boundariesRectangle.width;
				col++) {

			if (row < m_greenLimitPerColumn[col]) {
				continue;
			}

			imagePtr = (uchar*) (image->imageData + row * image->widthStep
					+ 3 * col);

			if (IsItOrangePixel(imagePtr, ballWasDetectedAlready) == true) // orange pixel
					{
				notOrangeConsecutivePixels = 0;
				if (currentlyInShape == false) // not in shape mode
						{
					ballCandidate =
							m_ballCandidatesList.FindNearbyBallCandidate(row,
									col); // find a nearby ball candidate
					currentlyInShape = true; // entered to shape
				}

				ballCandidate->UpdateBallCharacteristics(row, col); // update this ball characteristics
			} else //not orange pixel
			{

				notOrangeConsecutivePixels++; // increase consecutive not orange pixels

				if ((currentlyInShape == true)
						&& (notOrangeConsecutivePixels
								>= GET_OUT_OF_SHAPE_CONSECUTIVE_PIXELS)) // exiting shape mode
					currentlyInShape = false;
			}
		}

		m_ballCandidatesList.UpdateValidBallCandidatesForCurrentRow(row); // update the valid ball candidates for the finished row

		if (ballWasDetectedAlready == false) {
			m_ballCandidatesList.RemoveFalseBallsByNow(row);
		}
	}

	if (m_debugMode == true) {
		//cout<<"DBG Object Detector: Start Checking for Best Match of Ball"<<endl;
	}

//	m_ballCandidatesList.CalculateGreenPixelsRatio(image);					// calculate the green pixels ratio for all the ball candidates

	bestMatchedBall = m_ballCandidatesList.FindBestMatchingBall(m_ballDetected,
			image); // find the best matching ball

	//cout << "Number of Ball Candidates = " << m_ballCandidatesList.GetNumberOfBallCandidates() << endl;
	if (bestMatchedBall != NULL) // if found ball
	{
		ball = (*bestMatchedBall); // copy the balls content to the ball result
		m_ballCandidatesList.RemoveAllBallCandidates(); // deletes the list
		return true;
	} else {
		//	cout << "Nothing Detected" << endl;
		m_ballCandidatesList.RemoveAllBallCandidates(); // deletes the list
		return false;
	}
}

bool ObjectDetector::IsItOrangePixel(uint8_t* imagePtr,
		bool BallWasDetectedAlready) {
	uint8_t hue = *(imagePtr); // H component
	uint8_t sat = *(imagePtr + 1);
	uint8_t val = *(imagePtr + 2);

	if (BallWasDetectedAlready == false) {
		if (hue == ORANGE_AVG_HUE && sat == MAX_SAT && val == MAX_VAL) {
			return true;
		} else {
			return false;
		}
	} else {
		if (hue
				<= m_orangeMaxHue
						+ EXTRA_BOUNDARY_FOR_ORANGE_HUE_BALL_SECOND_SCAN
				&& hue
						>= MAX((int)m_orangeMinHue - EXTRA_BOUNDARY_FOR_ORANGE_HUE_BALL_SECOND_SCAN,0)
				&& (sat > MAX(m_orangeMinSat - 30,0)/* || sat > m_whiteMaxSat || m_whiteMaxSat >  m_orangeMinSat)*/
						&& sat != MIN_SAT) && val > MAX(m_orangeMinVal - 30,0)) {
			*(imagePtr) = ORANGE_AVG_HUE;
			*(imagePtr + 1) = MAX_SAT;
			*(imagePtr + 2) = MAX_VAL;

			return true;
		} else {
			return false;
		}

	}
}

/*****************************************************************************************
 * Function Name: Detect Average Ball
 * Description: This Function gets a segmented image and searches for the ball in it.
 * 				It only counts a ball if it found four frames with the center of the ball
 * 				within some distance from the mean center point - to reduce noises.
 * 				If it finds a ball, it searches for the ball in the frame in a bounding rectangle
 *				where it bounds the ball found in the last frame.
 *				It puts the ball it has found in ball parameter.
 * Return Value: boolean - true or false if found a ball within the boundaries
 *****************************************************************************************/
bool ObjectDetector::DetectAverageBall(IplImage* image, Ball& ball) {
	static CvRect allFrame = cvRect(0, 0, image->width, image->height); // rectangle of all the frame
	static CvRect boundariesRectangle = cvRect(0, 0, image->width,
			image->height); // rectangle of boundaries to check
	static int consecutiveBallFrames = 0; // number of consecutive frames the ball was detected

	static Point2D meanPoint(0, 0); // the mean point of the ballPlace array

	int i; // loop variable
	int distanceX, distanceY; // distance in x-axis and in y-axis

	if (consecutiveBallFrames == 0) // initializing the mean point
			{
		meanPoint.X = meanPoint.Y = 0;
	}

	if (1/*m_ballDetected == false*/) // no ball is detected
	{
		for (i = consecutiveBallFrames;
				i < NUM_OF_ITERATIONS_SEARCHING_THE_BALL; i++) // searching the ball
				{
			if ((DetectBall(image, allFrame, m_ballPlaceHistory[i])) == true) // search ball
				//	{
				// set the new boundaries for the next rectangle

				SetNewBoundingRectangle(boundariesRectangle,
						m_ballPlaceHistory[i], m_ballPlaceHistory[i],
						image->width, image->height);

			if (DetectBall(image, boundariesRectangle, m_ballPlaceHistory[i])
					== true) {
				meanPoint.X += m_ballPlaceHistory[i].m_center.X; // for mean calculations
				meanPoint.Y += m_ballPlaceHistory[i].m_center.Y; // for mean calculations
				consecutiveBallFrames = 0; // increasing the number of consecutive frames ball was detected

				ball = m_ballPlaceHistory[i];
				ball.m_timeBallDetected.update();

				return true;
			} else {
				consecutiveBallFrames = 0; // initializing the ball detected frames
				return false;
			}
//			}
//			else
//			{
//				consecutiveBallFrames = 0;	// initializing the ball detected frames
//				return false;
//			}
		}

		meanPoint.X = meanPoint.X / (float) NUM_OF_ITERATIONS_SEARCHING_THE_BALL; // mean point calculations
		meanPoint.Y = meanPoint.Y / (float) NUM_OF_ITERATIONS_SEARCHING_THE_BALL; // mean point calculations

		for (i = 0; i < NUM_OF_ITERATIONS_SEARCHING_THE_BALL; i++) {
			distanceX = ABS(m_ballPlaceHistory[i].m_center.X - meanPoint.X); // calculating the distance of the ball from the mean point in x-axis
			distanceY = ABS(m_ballPlaceHistory[i].m_center.Y - meanPoint.Y); // calculating the distance of the ball from the mean point in y-axis

			if (distanceX > MAX_DISTANCE_FROM_AVERAGE_POINT
					|| distanceY > MAX_DISTANCE_FROM_AVERAGE_POINT) {
				// found a ball far from the mean
				consecutiveBallFrames = 0; // initializing the ball detected consecutive frames

				ball = m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL
						- 1];
				return false;
			}
		}

		m_ballDetected = true; // ball was detected. all distances match
		ball = m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 1]; // set ball result to the last good ball detected
		boundariesRectangle = allFrame; // set the next bounding rectangle to all the frame
		return true;
	} else // m_ballDetected = true
	{
		if (DetectBall(image, boundariesRectangle,
				m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 1])
				== true) // detect ball in updated boundaries rectangle
				{
			ball = m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 1];

			cvRectangle(
					image,
					cvPoint(boundariesRectangle.x, boundariesRectangle.y),
					cvPoint(boundariesRectangle.x + boundariesRectangle.width,
							boundariesRectangle.y + boundariesRectangle.height),
					cvScalar(0), 1);

			// distance from last point calculations
			distanceX =
					ABS(m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-2].m_center.X - m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-1].m_center.X);
			distanceY =
					ABS(m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-2].m_center.Y - m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-1].m_center.Y);

			// current point is far from the last one
			if (distanceX > boundariesRectangle.height
					|| distanceY > boundariesRectangle.width) {
				consecutiveBallFrames = 0;
				m_ballDetected = false;
				if (m_debugMode == true) {
//					cout<<"*************************************DISTANCE*****************************************"<<endl;
//
//					cout << "Green Ratio = "<< ballPlace[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-1].m_greenRatio << endl;
//					cout << "Radius Ratio = "<< ballPlace[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-1].m_radiusRatio << endl;
//					cout << "Radius = "<< ballPlace[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-1].m_radius << endl;
//					cout << "Correct/Incorrect Ratio = "<< (float)(ballPlace[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-1].m_correctBallShapeTests)/(float)(ballPlace[NUM_OF_ITERATIONS_SEARCHING_THE_BALL-1].m_incorrectBallShapeTests) << endl;
//
//					cout << "distanceX = " << distanceX << endl;
//					cout << "distanceY = " << distanceY << endl;
//
//					cout << "boundariesRectangle.height = " << boundariesRectangle.height << endl;
//					cout << "boundariesRectangle.width = " << boundariesRectangle.width << endl;
				}
				cvCircle(
						image,
						cvPoint(
								m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL
										- 1].m_center.X,
								m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL
										- 1].m_center.Y),
						m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL
								- 1].m_radius, cvScalar(0, 255, 255),
						CV_FILLED);
				cvCircle(
						image,
						cvPoint(
								m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL
										- 2].m_center.X,
								m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL
										- 2].m_center.Y),
						m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL
								- 2].m_radius, cvScalar(0, 255, 255),
						CV_FILLED);

				cvRectangle(
						image,
						cvPoint(boundariesRectangle.x, boundariesRectangle.y),
						cvPoint(
								boundariesRectangle.x
										+ boundariesRectangle.width,
								boundariesRectangle.y
										+ boundariesRectangle.height),
						cvScalar(120, 255, 255), CV_FILLED);

				return false;
			}

			// ball was found again

			// set the new boundaries for the next rectangle
			SetNewBoundingRectangle(
					boundariesRectangle,
					m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 1],
					m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 2],
					image->width, image->height);

			// update ball place
			m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 2] =
					m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 1];

			// return ball result
			ball = m_ballPlaceHistory[NUM_OF_ITERATIONS_SEARCHING_THE_BALL - 1];

			ball.m_timeBallDetected.update();

			return true; // ball was found
		}

		cvRectangle(
				image,
				cvPoint(boundariesRectangle.x, boundariesRectangle.y),
				cvPoint(boundariesRectangle.x + boundariesRectangle.width,
						boundariesRectangle.y + boundariesRectangle.height),
				cvScalar(120, 255, 255), 2);

		consecutiveBallFrames = 0; // no ball was found this time
		m_ballDetected = false; // set the flag for ball detected
		if (m_debugMode == true) {
			//cout << "*************************Nothing Detected*******************************" << endl;
		}
		return false; // no ball detected
	}
}

/*****************************************************************************************
 * Function Name: SetNewBoundingRectangle
 * Description: This function sets the boundaries of the new rectangle, calculated using the
 * 				current and the last ball points, predicting the next center point of the rectangle.
 * Return Value: None.
 *****************************************************************************************/
void ObjectDetector::SetNewBoundingRectangle(CvRect& boundariesRectangle,
		Ball& currentBall, Ball& previousBall, int xAxisLimit, int yAxisLimit) {
	boundariesRectangle.height = currentBall.m_diameter * 2; // set the new rectangle boundaries
	boundariesRectangle.width = currentBall.m_diameter * 2; // set the new rectangle boundaries

	// checking width and height boundaries
	if (boundariesRectangle.height < MIN_BOUNDARIES_RECTANGLE) {
		boundariesRectangle.height = boundariesRectangle.width =
				MIN_BOUNDARIES_RECTANGLE;
	} else if (boundariesRectangle.height > MAX_BOUNDARIES_RECTANGLE) {
		boundariesRectangle.height = boundariesRectangle.width =
				MAX_BOUNDARIES_RECTANGLE;
	}

//	if (m_headMoveInLastFrame == false)
//	{
//		// setting top left point of the new rectangle
//		boundariesRectangle.x = (2*currentBall.m_center.X - previousBall.m_center.X) - boundariesRectangle.width/2;
//		boundariesRectangle.y = (2*currentBall.m_center.Y - previousBall.m_center.Y) - boundariesRectangle.height/2;
//	}
//	else
//	{
//		boundariesRectangle.x = Camera::WIDTH/2 - boundariesRectangle.width/2;
//		boundariesRectangle.y = Camera::HEIGHT/2 - boundariesRectangle.height/2;
//	}
	boundariesRectangle.x = (currentBall.m_center.X)
			- boundariesRectangle.width / 2;
	boundariesRectangle.y = (currentBall.m_center.Y)
			- boundariesRectangle.height / 2;

	// checking boundaries
	if (boundariesRectangle.x < 0) {
		boundariesRectangle.x = 0;
	} else if (boundariesRectangle.x > xAxisLimit) {
		boundariesRectangle.x = xAxisLimit;
	}

	if (boundariesRectangle.y < 0) {
		boundariesRectangle.y = 0;
	} else if (boundariesRectangle.y > yAxisLimit) {
		boundariesRectangle.y = yAxisLimit;
	}

}

void ObjectDetector::DetectGreenLimits(IplImage* hsvImage) {
	int row, col, hue, sat, val;
	uchar* hsvImagePtr;

	int greenCounterSequence;

	int numOfLimitsInFirstRow = 0;

	for (col = 0; col < hsvImage->width; col++) {
		greenCounterSequence = 0;
		m_greenLimitPerColumn[col] = Camera::HEIGHT - 1;

		for (row = 0; row < hsvImage->height; row++) {
			hsvImagePtr = (uchar*) (hsvImage->imageData
					+ row * hsvImage->widthStep + 3 * col);
			hue = *(hsvImagePtr);
			sat = *(hsvImagePtr + 1);
			val = *(hsvImagePtr + 2);

			if ((hue == GREEN_AVG_HUE || hue == ORANGE_AVG_HUE)
					&& sat == MAX_SAT && val == MAX_VAL) {
				greenCounterSequence++;
			} else {
				greenCounterSequence = MAX(greenCounterSequence - 1, 0);
			}

			if (greenCounterSequence == MIN_GREEN_SEQUENCE_TO_FIELD_LIMITS) {
				m_greenLimitPerColumn[col] = MAX(row - 3,0);

				break;
			}
		}

		if (row - MIN_GREEN_SEQUENCE_TO_FIELD_LIMITS + 1
				<= MIN_GREEN_SEQUENCE_TO_FIELD_LIMITS + 1) {
			numOfLimitsInFirstRow++;
		}

		//	hsvImagePtr = (uchar*) (hsvImage->imageData + (m_greenLimitPerColumn[col]) * hsvImage->widthStep + 3*col);
		//	*(hsvImagePtr) = 0;
		//	*(hsvImagePtr+1) = 255;
		//	*(hsvImagePtr+2) = 255;

	}

	if (numOfLimitsInFirstRow
			> MIN_RATIO_TO_ZERO_FIELD_LIMITS * Camera::WIDTH) {
		memset(m_greenLimitPerColumn, 0, sizeof(int) * Camera::WIDTH);
	}

	//FixGreenLimitByFIR();

	//for(col = 0 ; col < hsvImage->width ; col++ )
	//{
	//	hsvImagePtr = (uchar*) (hsvImage->imageData + (m_greenLimitPerColumn[col]) * hsvImage->widthStep + 3*col);
	//	*(hsvImagePtr) = 0;
	//	*(hsvImagePtr+1) = 255;
	//	*(hsvImagePtr+2) = 255;
	//}
}

void ObjectDetector::FixGreenLimitByFIR() {
	FirTrendline greenLimitsFIR;
	Point2DInt_t newPointForFIR;
	Point2DInt_t pointToRemoveFromFIR;

	int predicedGreenLimitsRow;

	//initialize FIR Trendline with FIR_TRENDLINE_SIZE first cols
	for (int i = 0; i < FIR_TRENDLINE_SIZE; i++) {
		newPointForFIR.x = i;
		newPointForFIR.y = m_greenLimitPerColumn[i];
//		cout << "col = " << i << " Predicted = " << newPointForFIR.y << endl;
		greenLimitsFIR.AddPoint(&newPointForFIR);
	}

	greenLimitsFIR.CalculateSlope();
	greenLimitsFIR.CalculateIntercept();

	//cout<<"Slope = "<<greenLimitsFIR.GetSlope()<<" inter = "<<greenLimitsFIR.GetIntecept()<<"\n";

	for (int i = FIR_TRENDLINE_SIZE; i < Camera::WIDTH; i++) {
		newPointForFIR.x = i;
		newPointForFIR.y = m_greenLimitPerColumn[i];

		pointToRemoveFromFIR.x = i - FIR_TRENDLINE_SIZE;
		pointToRemoveFromFIR.y = m_greenLimitPerColumn[i - FIR_TRENDLINE_SIZE];

		predicedGreenLimitsRow = greenLimitsFIR.PredictPoint(i);

//		cout << "col = " << i << " Predicted = " << predicedGreenLimitsRow << " Real = " << newPointForFIR.y <<  endl;
		if (ABS(predicedGreenLimitsRow - newPointForFIR.y)
				> PREDICTED_GREEN_LIMITS_ERROR) {
			cout << "Fixing Trendline\n";

			newPointForFIR.y = predicedGreenLimitsRow;

			newPointForFIR.y = MIN(newPointForFIR.y,Camera::HEIGHT - 1);
			newPointForFIR.y = MAX(newPointForFIR.y,0);

			m_greenLimitPerColumn[i] = newPointForFIR.y;
		}

		greenLimitsFIR.UpdateFirTrendline(&newPointForFIR,
				&pointToRemoveFromFIR);
//		cout << "newP x = " <<newPointForFIR.x<< " y = "<<newPointForFIR.y<<
//		", To remove Point x = " <<pointToRemoveFromFIR.x<< " y = " <<pointToRemoveFromFIR.y<< "\n";

//		cout<<"Slope = "<<greenLimitsFIR.GetSlope()<<" inter = "<<greenLimitsFIR.GetIntecept()<<"\n";
	}

}

void ObjectDetector::TheNewDetectFieldPoles(IplImage* image, Pole& firstPole,
		Pole& secondPole, Pole& localizationPole) {
	InitializeFieldPolesDetection(firstPole, secondPole, localizationPole); // step 0
	DetectPotentialColumns(image); // step 1
	MergeColumnsToSpots(image); // step 2

	FindStartOfPoles(image); // step 3
	DetectPoles(image); // step 4

	CheckPoleRules(image); // step 5
	FixHighestLocalizationPoint(image);
	CalculateSignificantPolePoints(image);

	IdentifyPoles(image, firstPole, secondPole, localizationPole); // step 6
	IdentifySideOfGoalPoles(image, firstPole, secondPole); // step 7

}

void ObjectDetector::InitializeFieldPolesDetection(Pole& firstPole,
		Pole& secondPole, Pole& localizationPole) {
	firstPole.m_dataValid = false;
	secondPole.m_dataValid = false;
	localizationPole.m_dataValid = false;
	m_poleCandidatesList.RemoveAllPoleCandidates();
}

void ObjectDetector::DetectPotentialColumns(IplImage* image) {
	int row, col;
	int numOfColoredPixels;

	uint8_t hue, sat, val;

	uint8_t* imagePtr;

	float coloredPixelsRatio;

	for (col = 0; col < image->width; col++) // go over all the rows in the current column
			{
		numOfColoredPixels = 0;

		for (row = m_greenLimitPerColumn[col];
				row
						> m_greenLimitPerColumn[col]
								- SIZE_OF_SEARCH_LINE_FOR_POTENTIAL_COLUMN;
				row--) // go over all the rows in the current column
				{
			imagePtr = (uint8_t*) (image->imageData + row * image->widthStep
					+ 3 * col);
			hue = *(imagePtr); // the hue value of the pixel
			sat = *(imagePtr + 1);
			val = *(imagePtr + 2);

			if ((hue == BLUE_AVG_HUE || hue == YELLOW_AVG_HUE) && sat == MAX_SAT
					&& val == MAX_VAL) {
				numOfColoredPixels++;
			}

		}

		coloredPixelsRatio = (float) numOfColoredPixels
				/ SIZE_OF_SEARCH_LINE_FOR_POTENTIAL_COLUMN;

		if (coloredPixelsRatio > MIN_RATIO_COLOR_PIXELS_FOR_POTENTIAL_COLUMN) {
			m_potientialColumns[col] = true;
			//		cvRectangle(image, cvPoint(col, m_greenLimitPerColumn[col]), cvPoint(col, m_greenLimitPerColumn[col] - SIZE_OF_SEARCH_LINE_FOR_POTENTIAL_COLUMN), cvScalar(0, 255, 255), 1);
		} else {
			m_potientialColumns[col] = false;
		}
	}
}

void ObjectDetector::MergeColumnsToSpots(IplImage* image) {
	int col;
	Pole* poleCandidate;
	bool currentlyInPole = false;

	int startOfPole, endOfPole, notColumnSequences;
	m_poleCandidatesList.RemoveAllPoleCandidates();

	for (col = 0; col < Camera::WIDTH; col++) {
		if ((m_potientialColumns[col] == true) && (currentlyInPole == false)) {
			currentlyInPole = true;
			endOfPole = startOfPole = col;
			notColumnSequences = 0;
		} else if (m_potientialColumns[col] == true) {
			endOfPole = col;
			notColumnSequences = 0;
		} else if ((m_potientialColumns[col] == false)
				&& (currentlyInPole == true)) {
			notColumnSequences++;
		}

		if (currentlyInPole
				== true && notColumnSequences == GET_OUT_OF_POTENTIAL_SPOT_COLUMN_SEQUENCES) {
			currentlyInPole = false;
			poleCandidate = new Pole((startOfPole + endOfPole) / 2,
					m_greenLimitPerColumn[(int) (startOfPole + endOfPole) / 2]);
			m_poleCandidatesList.AddPoleCandidate(poleCandidate); // adds the ball to the list

			//cvCircle(image, cvPoint((startOfPole + endOfPole)/2, m_greenLimitPerColumn[(int)(startOfPole + endOfPole)/2]), 3, cvScalar(0, 255, 255), CV_FILLED);
		}

	}
}

void ObjectDetector::FindStartOfPoles(IplImage* image) {

	int squareTopLeftPointRow, squareTopLeftPointCol; // coordinates of the top-left point of the bounding square
	int squareBottomRightPointRow, squareBottomRightPointCol; // coordinates of the bottom-right point of bounding square

	uint8_t* imagePtr; // the image pointer
	int row, col; // loop variables
	uint8_t hue, sat, val; // hue of segmented HSV frame

	Pole* iterator = m_poleCandidatesList.m_head;
	int sumOfColsLowestPixels;
	int numOfColsLowestPixels;

	int numOfPoleCandidates = m_poleCandidatesList.m_numberOfPoleCandidates;

	for (int i = 0; i < numOfPoleCandidates; i++) {
		squareTopLeftPointRow =
				MAX(iterator->m_centerOfMinPointSearch.y - SQUARE_SEARCH_SIZE_OFFSET_Y,0);
		squareTopLeftPointCol =
				MAX(iterator->m_centerOfMinPointSearch.x - SQUARE_SEARCH_SIZE_OFFSET_X,0);

		// determining the bottom right point of the square
		squareBottomRightPointRow =
				MIN(iterator->m_centerOfMinPointSearch.y + 3,image->height);
		squareBottomRightPointCol =
				MIN(iterator->m_centerOfMinPointSearch.x + SQUARE_SEARCH_SIZE_OFFSET_X,image->width);

		//	cvRectangle(image, cvPoint(squareTopLeftPointCol, squareTopLeftPointRow), cvPoint(squareBottomRightPointCol, squareBottomRightPointRow), cvScalar(0,255, 255), 2);
		sumOfColsLowestPixels = 0;
		numOfColsLowestPixels = 0;

		for (row = squareBottomRightPointRow; row >= squareTopLeftPointRow;
				row--) {
			for (col = squareTopLeftPointCol; col < squareBottomRightPointCol;
					col++) {
				imagePtr = (uint8_t*) (image->imageData + row * image->widthStep
						+ 3 * col);
				hue = *(imagePtr);
				sat = *(imagePtr + 1);
				val = *(imagePtr + 2);

				if ((hue == BLUE_AVG_HUE || hue == YELLOW_AVG_HUE)
						&& sat == MAX_SAT && val == MAX_VAL) {
					if (row
							> iterator->m_lowestPixelPosition
									- NUM_OF_ROWS_TO_AVG_POLE_STARTING_POINT) {
						if (row > iterator->m_lowestPixelPosition) {
							iterator->m_lowestPixelPosition = row;
						}
						sumOfColsLowestPixels += col;
						numOfColsLowestPixels++;
					}
				}

			}
		}
		if (numOfColsLowestPixels == 0) {
			Pole* iteratorTemp = iterator;
			iterator = iterator->m_next;
			m_poleCandidatesList.RemovePoleCandidate(iteratorTemp);
		} else {
			int avgOfColsLowestPixels = sumOfColsLowestPixels
					/ numOfColsLowestPixels;

			iterator->m_poleStartingPoint.x = avgOfColsLowestPixels;
			iterator->m_poleStartingPoint.y = iterator->m_lowestPixelPosition;

			//		cvCircle(image, cvPoint(iterator->m_poleStartingPoint.x, iterator->m_poleStartingPoint.y), 2, cvScalar(0, 255, 255), CV_FILLED);
			iterator = iterator->m_next;
		}
	}

}

void ObjectDetector::DetectPoles(IplImage* image) {
	Pole* iterator = m_poleCandidatesList.m_head;

	int numOfPoleCandidates = m_poleCandidatesList.m_numberOfPoleCandidates;
	Point2DInt_t currentPoleCenter;

	int notColoredPixelsSequence;

	uint8_t* imagePtr; // the image pointer
	int row, col; // loop variables
	uint8_t hue, val, sat; // hue of segmented HSV frame

	bool endOfPole;
	bool validRow;

	int numOfRowsWitheoutColoredPixelsSequence;

	for (int i = 0; i < numOfPoleCandidates; i++) {
		endOfPole = false;

		currentPoleCenter.x = iterator->m_poleStartingPoint.x;
		currentPoleCenter.y = MAX(iterator->m_poleStartingPoint.y-1 , 0);

		numOfRowsWitheoutColoredPixelsSequence = 0;

		while (endOfPole == false) {
			row = currentPoleCenter.y;
			col = currentPoleCenter.x;

			if (currentPoleCenter.y < 0) {
				endOfPole = true;
				break;
			}
			iterator->InitializePoleForNewRow();

			notColoredPixelsSequence = 0;

			validRow = false;

			//searching left border of pole in a row
			while (notColoredPixelsSequence
					< MAX_NUM_OF_NOT_COLORED_PIXELS_SEQUENCE && col >= 0) {

				imagePtr = (uint8_t*) (image->imageData + row * image->widthStep
						+ 3 * col);
				hue = *(imagePtr);
				sat = *(imagePtr + 1);
				val = *(imagePtr + 2);

				if ((hue == BLUE_AVG_HUE || hue == YELLOW_AVG_HUE)
						&& sat == MAX_SAT && val == MAX_VAL) {
					validRow = true;
					notColoredPixelsSequence = 0;
					iterator->UpdatePoleCharacteristics(col, hue);
				} else {
					notColoredPixelsSequence++;
				}

				col--;
			}

			col = currentPoleCenter.x + 1;
			notColoredPixelsSequence = 0;

			//searching right border of pole in a row
			while (notColoredPixelsSequence
					< MAX_NUM_OF_NOT_COLORED_PIXELS_SEQUENCE
					&& col < image->width) {
				imagePtr = (uint8_t*) (image->imageData + row * image->widthStep
						+ 3 * col);
				hue = *(imagePtr);
				sat = *(imagePtr + 1);
				val = *(imagePtr + 2);

				if ((hue == BLUE_AVG_HUE || hue == YELLOW_AVG_HUE)
						&& sat == MAX_SAT && val == MAX_VAL) {
					validRow = true;
					notColoredPixelsSequence = 0;
					iterator->UpdatePoleCharacteristics(col, hue);
				} else {
					notColoredPixelsSequence++;
				}

				col++;
			}

			if (validRow == true) {
				numOfRowsWitheoutColoredPixelsSequence = 0;
				endOfPole = iterator->UpdatePoleForLastRow(row,
						&currentPoleCenter);

				if (endOfPole == false) {
					currentPoleCenter.x =
							(iterator->m_lastPixelPositionInCurrentRow
									+ iterator->m_firstPixelPositionInCurrentRow)
									/ 2;
					currentPoleCenter.y--;

					//	cvCircle(image, cvPoint(currentPoleCenter.x, currentPoleCenter.y), 1 ,cvScalar(0, 255, 255), 1);
					//	cvRectangle(image, cvPoint(iterator->m_firstPixelPositionInCurrentRow, row), cvPoint(iterator->m_lastPixelPositionInCurrentRow, row), cvScalar(0, 255, 255), 1);

				}
			} else {
				if (numOfRowsWitheoutColoredPixelsSequence
						>= MAX_NUM_OF_ROWS_WITHOUT_COLORED_PIXELS) {
					endOfPole = true;
				} else {
					numOfRowsWitheoutColoredPixelsSequence++;
					currentPoleCenter.y--;
				}
			}
		}

		iterator = iterator->m_next;
	}
}

void ObjectDetector::CheckPoleRules(IplImage* image) {
	Pole* iterator = m_poleCandidatesList.m_head; // iterator, goes over the pole candidates list
	Pole* temp;

	bool polePassedRules = false;

//	cout <<"new frame" << endl;
	while (iterator != NULL) // go over the list of poles
	{
		polePassedRules = false;

		if (iterator->m_poleHeight > MIN_POLE_HEIGHT
				&& (iterator->m_averageOfPoleWidth > MIN_POLE_WIDTH)
				&& (iterator->m_averageOfPoleWidth < MAX_POLE_WIDTH)
				&& (iterator->m_poleHeight / iterator->m_averageOfPoleWidth
						> MIN_WIDTH_HEIGHT_RATIO_FOR_POLE)) {
			// calculate the standard deviation of the pole's width
			if (iterator->ValidPoleWidthSTDForSpecificWidth() == true) {
				//		cout<<"STD = "<<iterator->m_STDOfPoleWidth <<endl;
				polePassedRules = true;

			}
		}

		if (polePassedRules == false) {
			temp = iterator;
			iterator = iterator->m_next;
			m_poleCandidatesList.RemovePoleCandidate(temp);
		} else {
			iterator = iterator->m_next;
		}

	}

}

void ObjectDetector::FixHighestLocalizationPoint(IplImage* image) {
	Pole* iterator = m_poleCandidatesList.m_head; // iterator, goes over the pole candidates list

	while (iterator != NULL) // go over the list of poles
	{
		iterator->m_poleTrendline.EraseLastPointsPrecentage(
				PRECENTAGE_TO_REMOVE_POINTS_FROM_TRENDLINE);
		iterator->FixHighestLocalizationPointForPole(
				(uint8_t*) image->imageData);

		iterator = iterator->m_next;
	}
}

void ObjectDetector::CalculateSignificantPolePoints(IplImage* image) {
	Pole* iterator = m_poleCandidatesList.m_head; // iterator, goes over the pole candidates list

	while (iterator != NULL) // go over the list of poles
	{
		iterator->m_localizationBottomPoint.X = iterator->m_poleStartingPoint.x;
		iterator->m_localizationBottomPoint.Y = iterator->m_poleStartingPoint.y;

		iterator->m_localizationTopPoint.Y = iterator->m_highestPixelPosition;
		iterator->m_localizationTopPoint.X =
				iterator->m_poleTrendline.PredictPoint(
						iterator->m_highestPixelPosition);

		iterator->m_localizationMiddlePoint.Y =
				(iterator->m_localizationTopPoint.Y
						+ iterator->m_localizationBottomPoint.Y) / 2;
		iterator->m_localizationMiddlePoint.X =
				iterator->m_poleTrendline.PredictPoint(
						iterator->m_localizationMiddlePoint.Y);

		iterator->m_localizationEuclideanHeight =
				sqrt(
						(iterator->m_localizationTopPoint.Y
								- iterator->m_localizationBottomPoint.Y)
								* (iterator->m_localizationTopPoint.Y
										- iterator->m_localizationBottomPoint.Y)
								+ (iterator->m_localizationTopPoint.X
										- iterator->m_localizationBottomPoint.X)
										* (iterator->m_localizationTopPoint.X
												- iterator->m_localizationBottomPoint.X));

		//cout << "m_localizationEuclideanHeight = " << iterator->m_localizationEuclideanHeight << endl;

		iterator = iterator->m_next;

	}
}

void ObjectDetector::IdentifyPoles(IplImage* image, Pole& firstPole,
		Pole& secondPole, Pole& localizationPole) {
	Pole* iterator = m_poleCandidatesList.m_head; // iterator, goes over the pole candidates list
	Pole* temp;

	bool isItValidPole;
	bool colorPixelCheck;

	int numOfGoalPolesDetected = 0;

	while (iterator != NULL) // go over the list of poles
	{

		isItValidPole = iterator->CheckTypeOfPole((uint8_t*) image->imageData);
		colorPixelCheck = iterator->CheckColoredPixelPercentage();

		if (colorPixelCheck == true && isItValidPole == true) {
			iterator->m_dataValid = true;

			if (iterator->m_poleColor == YELLOW_GOAL
					|| iterator->m_poleColor == BLUE_GOAL) {

				if (numOfGoalPolesDetected == 0) // save the first pole
						{
					firstPole = *iterator; // save the pole
					numOfGoalPolesDetected = 1; // found one pole

				} else if (numOfGoalPolesDetected == 1) // save the second pole
						{
					secondPole = *iterator; // save the pole
					numOfGoalPolesDetected = 2; // found two poles by now

				}
			} else {
				localizationPole = *iterator; // save the pole
			}
		} else {
			temp = iterator;
			iterator = iterator->m_next;
			m_poleCandidatesList.RemovePoleCandidate(temp);
			continue;

		}

		iterator = iterator->m_next;

	}

}

void ObjectDetector::DetectFaceUp(Pole& pole) {
	if (pole.m_highestPixelPosition < POLE_HEIGHT_LIMIT) {
		pole.m_poleSide = FACE_UP_TO_SEE_POLE_SIDE;
	}
}

void ObjectDetector::IdentifySideOfGoalPoles(IplImage* image, Pole& firstPole,
		Pole& secondPole) {
	if (firstPole.m_dataValid == false) {
		m_polesDetectionStatus = NO_POLES;
		return;
	}
	if (secondPole.m_dataValid == true) {
		DetectFaceUp(firstPole);
		DetectFaceUp(secondPole);

		if (firstPole.m_poleSide == FACE_UP_TO_SEE_POLE_SIDE
				&& secondPole.m_poleSide == FACE_UP_TO_SEE_POLE_SIDE) {
			m_polesDetectionStatus = TWO_POLES_FACE_UP;
		} else if (firstPole.m_poleSide == FACE_UP_TO_SEE_POLE_SIDE) {
			m_polesDetectionStatus = TWO_POLES_FACE_UP_LEFT;
		} else if (secondPole.m_poleSide == FACE_UP_TO_SEE_POLE_SIDE) {
			m_polesDetectionStatus = TWO_POLES_FACE_UP_RIGHT;
		} else {
			m_polesDetectionStatus = TWO_POLES_NO_FACE_UP;
		}

		return;
	}
//	cout << "m_slope = " << firstPole.m_poleTrendline.GetSlope() << endl;

	DetectFaceUp(firstPole);

	if (firstPole.m_poleSide == FACE_UP_TO_SEE_POLE_SIDE) {
		m_polesDetectionStatus = ONE_POLE_FACE_UP;
		return;
	}

	int rightSquareTopLeftPointRow, rightSquareTopLeftPointCol; // coordinates of the top-left point of the bounding square
	int rightSquareBottomRightPointRow, rightSquareBottomRightPointCol; // coordinates of the bottom-right point of bounding square

	int leftSquareTopLeftPointRow, leftSquareTopLeftPointCol; // coordinates of the top-left point of the bounding square
	int leftSquareBottomRightPointRow, leftSquareBottomRightPointCol; // coordinates of the bottom-right point of bounding square

	uchar* imagePtr; // the image pointer
	int row, col; // loop variables
	uchar hue, sat, val; // hue of segmented HSV frame

	int goalColorPixels = 0, totalPixels = 0;

	double poleSlope = firstPole.m_poleTrendline.GetSlope();
	//cout<<"m_highestPixelPosition = "<<pole.m_highestPixelPosition<<"\n";

	int squareOffsetFromSlope = 0;
	if (ABS(poleSlope) > MIN_ANGLE_FOR_POLE_IDENTIFICATION_SIDE_ROTATION) {
		squareOffsetFromSlope =
				-OFFSET_SIZE_FOR_POLE_IDENTIFICATION_SIDE_ROTATION
						* SIGN(poleSlope);
	}

	rightSquareTopLeftPointRow =
			MAX(firstPole.m_highestPixelPosition - POLE_SIDE_SQUARE_OFFSET + squareOffsetFromSlope,0);
	rightSquareTopLeftPointCol =
			MIN(firstPole.m_localizationTopPoint.X + firstPole.m_averageOfPoleWidth/2 + POLE_SIDE_SQUARE_OFFSET,image->width);

	// determining the bottom right point of the square
	rightSquareBottomRightPointRow =
			MIN(firstPole.m_highestPixelPosition + POLE_SIDE_IDENTIFIACTION_SQUARE_SIZE_HEIGHT + squareOffsetFromSlope,image->height);
	rightSquareBottomRightPointCol =
			MIN(firstPole.m_localizationTopPoint.X + firstPole.m_averageOfPoleWidth/2 + POLE_SIDE_SQUARE_OFFSET + POLE_SIDE_IDENTIFIACTION_SQUARE_SIZE_WIDHT,image->width);

	cvRectangle(
			image,
			cvPoint(rightSquareTopLeftPointCol, rightSquareTopLeftPointRow),
			cvPoint(rightSquareBottomRightPointCol,
					rightSquareBottomRightPointRow),
			cvScalar(GREEN_AVG_HUE, 255, 255), 1);

	for (row = rightSquareTopLeftPointRow; row < rightSquareBottomRightPointRow;
			row++) {
		for (col = rightSquareTopLeftPointCol;
				col < rightSquareBottomRightPointCol; col++) {
			imagePtr = (uchar*) (image->imageData + row * image->widthStep
					+ 3 * col);
			hue = *(imagePtr);
			sat = *(imagePtr + 1);
			val = *(imagePtr + 2);

			if ((hue == firstPole.m_poleColor) && sat == MAX_SAT
					&& val == MAX_VAL) {
				goalColorPixels++;

			}
			totalPixels++;

		}
	}

	float rightSquareRatio = (float) goalColorPixels / totalPixels;

	//left square
	leftSquareTopLeftPointRow =
			MAX(firstPole.m_highestPixelPosition - POLE_SIDE_SQUARE_OFFSET - squareOffsetFromSlope,0);
	leftSquareTopLeftPointCol =
			MAX(firstPole.m_localizationTopPoint.X - firstPole.m_averageOfPoleWidth/2 - POLE_SIDE_SQUARE_OFFSET - POLE_SIDE_IDENTIFIACTION_SQUARE_SIZE_WIDHT,0);

	// determining the bottom right point of the square
	leftSquareBottomRightPointRow =
			MIN(firstPole.m_highestPixelPosition + POLE_SIDE_IDENTIFIACTION_SQUARE_SIZE_HEIGHT - squareOffsetFromSlope,image->height);
	leftSquareBottomRightPointCol =
			MAX(firstPole.m_localizationTopPoint.X - firstPole.m_averageOfPoleWidth/2 - POLE_SIDE_SQUARE_OFFSET,0);

	goalColorPixels = 0;
	totalPixels = 0;

	cvRectangle(
			image,
			cvPoint(leftSquareTopLeftPointCol, leftSquareTopLeftPointRow),
			cvPoint(leftSquareBottomRightPointCol,
					leftSquareBottomRightPointRow),
			cvScalar(GREEN_AVG_HUE, 255, 255), 1);

	for (row = leftSquareTopLeftPointRow; row < leftSquareBottomRightPointRow;
			row++) {
		for (col = leftSquareTopLeftPointCol;
				col < leftSquareBottomRightPointCol; col++) {
			imagePtr = (uchar*) (image->imageData + row * image->widthStep
					+ 3 * col);
			hue = *(imagePtr);
			sat = *(imagePtr + 1);
			val = *(imagePtr + 2);

			if ((hue == firstPole.m_poleColor) && sat == MAX_SAT
					&& val == MAX_VAL) {
				goalColorPixels++;
			}
			totalPixels++;

		}
	}

	float leftSquareRatio = (float) goalColorPixels / totalPixels;

	if (leftSquareRatio > rightSquareRatio) {
		if (leftSquareRatio > POLE_SIDE_MIN_SQUARE_RATIO) {
			firstPole.m_poleSide = POLE_SIDE_RIGHT;
			m_polesDetectionStatus = ONE_POLE_RIGHT_SIDE;
		} else {
			firstPole.m_poleSide = UNKNOWN_POLE_SIDE;
			m_polesDetectionStatus = ONE_POLE_UNKNOWN_SIDE;
		}
	} else {
		if (rightSquareRatio > POLE_SIDE_MIN_SQUARE_RATIO) {
			firstPole.m_poleSide = POLE_SIDE_LEFT;
			m_polesDetectionStatus = ONE_POLE_LEFT_SIDE;
		} else {
			firstPole.m_poleSide = UNKNOWN_POLE_SIDE;
			m_polesDetectionStatus = ONE_POLE_UNKNOWN_SIDE;
		}
	}

}

HeadScanForGoal_e ObjectDetector::GetPolesDetectionStatus() {
	return m_polesDetectionStatus;
}

