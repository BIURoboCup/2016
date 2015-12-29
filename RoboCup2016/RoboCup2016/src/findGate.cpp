#include "findGate.h"
#define DEBUG 0

using namespace std;
using namespace cv;


void ImageShowOnDebug(const string& winName, Mat image)
{
	if (DEBUG == 1)
	{
		imshow(winName, image);
	}
}

/*****************************************************************************************
* Method Name: FindGate
* Description: The function search for a couple of white vertical rectangles, whose buttom points
* 				are on green && upper part is not on green.
* 				This rectangles are marked as gate posts.
* 				If only one post is present in the frame, the function tries to find the direction
* 				of the second vertical post, using the upper post direction.
* Arguments: BWImage, Output - DetectedGate
* Return Values: DetectedGate object
* Owner: Hen Shoob, Assaf Rabinowitz
*****************************************************************************************/
void FindGate(Mat& inputImage, DetectedGate& detectedGate)
{
	Mat onlyWhiteImage;
	GetWhiteImage(inputImage, onlyWhiteImage);

	Mat whiteVertical = onlyWhiteImage.clone();
	GetVerticalObjects(whiteVertical);

	//creating Mat thr with contours, using canny
	Mat src;
	cvtColor(whiteVertical, src, CV_GRAY2BGR);
	Mat canny;
	Canny(whiteVertical, canny, 50, 190, 3, false);
	ImageShowOnDebug("canny", canny);

	//crating contours vector
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(canny, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	cvtColor(canny, canny, CV_GRAY2BGR);
	//finding the blocking rectangles of the contours
	vector<RotatedRect> minRect(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contours[i]));
		DrawRectangle(canny, minRect[i]);
	}
	ImageShowOnDebug("contours", canny);

	Mat field;
	FindField(inputImage, field);

	//Shwoing the contours + field in one image
	cvtColor(canny, canny, CV_BGR2GRAY);
	Mat or;
	bitwise_or(field, canny, or);
	ImageShowOnDebug("convex", or);

	vector<RotatedRect> post1Candidates;
	FindPostCandidates(field, minRect, post1Candidates);

	RotatedRect post1 = FindLargestCandidate(post1Candidates);
	DrawRectangle(src, post1);
	ImageShowOnDebug("post1", src);

	vector<RotatedRect> post2Candidates;
	for (size_t i = 0; i < post1Candidates.size(); i++)
	{
		if (post1Candidates[i].center.x != post1.center.x)
		{
			post2Candidates.push_back(post1Candidates[i]);
		}
	}

	RotatedRect post2 = FindLargestCandidate(post2Candidates);
	DrawRectangle(src, post2);
	ImageShowOnDebug("post2", src);

	//////////////////////////////////////////////////////////////////////////////////////////

	if (post1.size.area() > 0 && post2.size.area() > 0)
	{
		detectedGate.InitializeFromTwoPosts(post1, post2);
		PrintStringOnImage(src, "Full Gate");
		return;
	}

	if (post1.size.area() == 0 && post2.size.area() == 0)
	{
		detectedGate.SetNotDetected();
		PrintStringOnImage(src, "Gate Not Found");
		return;
	}

	// One post - detect which.

	float longSide = (post1.size.height > post1.size.width) ? post1.size.height : post1.size.width;
	float shortSide = (post1.size.height < post1.size.width) ? post1.size.height : post1.size.width;

	Mat whiteHorizontal = onlyWhiteImage.clone();
	GetHorizontalObjects(whiteHorizontal);
	ImageShowOnDebug("whiteHorizontal", whiteHorizontal);

	//counting white pixels near the top of the post on both sides
	// We set the range to look for white pixels in the upper post.
	float upperPostTop = MAX(post1.center.y - longSide / 2 - 5, 0);
	float upperPostButtom = upperPostTop + shortSide + 5;	

	//Counting near the left post
	float upperPostLeft = MAX(post1.center.x - shortSide * 3, 0);
	float upperPostRight = MAX(post1.center.x - shortSide, 0);
	unsigned int leftCounter = CountAndDrawWhitePixels(whiteHorizontal, src, upperPostTop, upperPostButtom, upperPostLeft, upperPostRight);

	//finding right check rectangle, left and right side
	upperPostRight = MIN(post1.center.x + shortSide * 3, FRAME_WIDTH - 1);
	upperPostLeft = MIN(post1.center.x + shortSide, FRAME_WIDTH - 1);
	unsigned int rightCounter = CountAndDrawWhitePixels(whiteHorizontal, src, upperPostTop, upperPostButtom, upperPostLeft, upperPostRight);
	
	if (rightCounter > leftCounter  &&  rightCounter - leftCounter > 5)
	{
		detectedGate.InitializeFromOnePost(post1, E_DetectedPost_LEFT);
		PrintStringOnImage(src, "Left post -->");
	}
	else if (rightCounter < leftCounter  &&  leftCounter - rightCounter > 5)
	{
		detectedGate.InitializeFromOnePost(post1, E_DetectedPost_RIGHT);
		PrintStringOnImage(src, "Right post <--");
	}
	else
	{
		detectedGate.SetNotDetected();
		PrintStringOnImage(src, "Move camera up");
	}	
}

void DrawGateOnImage(Mat& image, DetectedGate& gate)
{
	if (gate.DetectedPost == E_DetectedPost_BOTH)
	{
		DrawRectangle(image, gate.LeftPost);
		DrawRectangle(image, gate.RightPost);
		DrawPoint(image, gate.Center);
	}
	if (gate.DetectedPost == E_DetectedPost_LEFT)
	{
		DrawRectangle(image, gate.LeftPost);
	}
	if (gate.DetectedPost == E_DetectedPost_RIGHT)
	{
		DrawRectangle(image, gate.RightPost);
	}
}

int CountAndDrawWhitePixels(Mat& BWImage, Mat&outputImageToDraw, float top, float buttom, float left, float right)
{
	int counter = 0;

	for (size_t i = top; i < buttom; i++)
	{
		for (size_t j = left; j < right; j++)
		{
			if (BWImage.at<unsigned char>(i, j) > 100)
			{
				counter++;
				outputImageToDraw.at<Vec3b>(i, j)[0] = 255;
				outputImageToDraw.at<Vec3b>(i, j)[1] = 0;
				outputImageToDraw.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}

	return counter;
}

//finding the largest rectangle among the qualified
RotatedRect FindLargestCandidate(vector<RotatedRect>& postCandidates)
{
	RotatedRect largestCandidate;
	if (postCandidates.size() > 0)
	{
		Size2f s = postCandidates[0].size;
		size_t pl = 0;
		for (size_t i = 0; i < postCandidates.size(); i++)
		{
			if (postCandidates[i].size.area() > s.area())
			{
				pl = i;
				s = postCandidates[i].size;
			}
		}
		largestCandidate = postCandidates[pl];
		return largestCandidate;
	}
	return largestCandidate;
}

void GetVerticalObjects(Mat &BWImage)
{
	//dilation of white to horizontaly fill the shapes
	Mat elementG1 = getStructuringElement(MORPH_RECT, Size(3, 5), Point(2, 2));
	dilate(BWImage, BWImage, elementG1);

	//eroding to remove horizontal lines
	Mat elementG2 = getStructuringElement(MORPH_RECT, Size(1, 41));
	erode(BWImage, BWImage, elementG2);

	//dilation of white to fill the shapes
	Mat elementG3 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(2, 2));
	dilate(BWImage, BWImage, elementG3);
}

void GetHorizontalObjects(Mat &BWImage)
{
	//dilation of white to fill the shapes
	Mat elementG4 = getStructuringElement(MORPH_RECT, Size(5, 3), Point(2, 2));
	dilate(BWImage, BWImage, elementG4);
}

bool IsPartiallyOnField(Mat& field, Point2f bottomPoint, Point2f topPoint)
{
	bottomPoint.y = (bottomPoint.y + 30 <= FRAME_HEIGHT) ? bottomPoint.y + 30 : bottomPoint.y;

	return (field.at<unsigned char>(bottomPoint) > 100
		&& field.at<unsigned char>(topPoint) < 100);
}

bool IsAngleStraight(float angle)
{
	return (angle > -10 && angle < 10) || (100 > angle && angle > 80) || (-100 < angle && angle < -80);
}

void FindPostCandidates(Mat& field, vector<RotatedRect>& minRect, vector<RotatedRect>& postCandidates)
{
	// Check foreach rect:
	// 1. Bottom of the rectangle is close enough to the field.
	// 2. Long side is at least 5 times the short side.
	// TODO: 3. Angle of the rectangle is close to 0 or 90.

	for (size_t i = 0; i < minRect.size(); i++)
	{
		float longSide = (minRect[i].size.height > minRect[i].size.width) ? minRect[i].size.height : minRect[i].size.width;
		float shortSide = (minRect[i].size.height < minRect[i].size.width) ? minRect[i].size.height : minRect[i].size.width;

		Point2f bottomPoint;
		bottomPoint.y = MIN(minRect[i].center.y + longSide / 2 + 5, FRAME_HEIGHT - 1);
		bottomPoint.x = minRect[i].center.x;
		Point2f topPoint;
		topPoint.y = MAX(minRect[i].center.y - longSide / 2 + 5, 0);
		topPoint.x = minRect[i].center.x;

		if ((IsPartiallyOnField(field, bottomPoint, topPoint))
			&& (longSide > 5 * shortSide)
			&& (IsAngleStraight(minRect[i].angle)))
		{
			postCandidates.push_back(minRect[i]);
		}
	}
}

void FindField(Mat& inputImage, Mat& field)
{
	Mat onlyGreenImage;
	inRange(inputImage, minGreenBGR, maxGreenBGR, onlyGreenImage);
	Mat grass_cpy = onlyGreenImage.clone();
	Mat raw_shapes;

	vector<vector<Point> > _contours; // Each contour stored as an array of points according to API.
	vector<Vec4i> _heir;

	cv::findContours(grass_cpy, _contours, _heir,
		CV_RETR_EXTERNAL,			// Only external shapes. (not one inside other)
		CV_CHAIN_APPROX_SIMPLE);	// how much point data about shape (From API )

	cv::threshold(onlyGreenImage, raw_shapes, 255, 0, THRESH_BINARY); // Make shapes black 1U image.
	field = raw_shapes.clone(); // Clone (copy) blank image with same Dimention (Width,Height);

	int maxindex = -1; // Find the max shape by area = the green field.
	int maxArea = -1, currentArea;
	for (size_t i = 0; i<(int)_contours.size(); i++)
	{
		if ((currentArea = cv::contourArea(_contours[i], false)) > maxArea)
		{
			maxindex = i;
			maxArea = currentArea;
		}
	}

	if (maxindex > -1)   // Found field
	{
		//  Find Convex of field:
		vector<vector<Point> > hull;
		hull.push_back(vector<Point>());

		cv::convexHull(_contours[maxindex], hull[0], true, true);
		cv::drawContours(field, hull, 0, Scalar(255, 255, 255), -1, 4);

		// Draw field without convex:

		cv::drawContours(raw_shapes, _contours,
			maxindex,						// Inddex to draw -1 for all.
			Scalar(255, 255, 255), 3,
			4,								// Drawing percision - more = slower (4 look like enough).
			_heir, 2);
	}
	ImageShowOnDebug("field", field);
}

void GetWhiteImage(Mat& inputImage, Mat& onlyWhiteImage)
{
	Mat hsvImage;
	cvtColor(inputImage, hsvImage, CV_BGR2HSV);
	inRange(hsvImage, minWhiteHSV, maxWhiteHSV, onlyWhiteImage);
	ImageShowOnDebug("white", onlyWhiteImage);
}

void DrawPoint(Mat &image, Point2i &point)
{
	circle(image, point, 10, Scalar(0, 0, 255));
}

void DrawRectangle(Mat &image, RotatedRect &rect)
{
	Point2f rect_points[4];
	rect.points(rect_points);
	for (int j = 0; j < 4; j++)
	{
		line(image, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255));
	}
}

void PrintStringOnImage(Mat &src, const char* string_to_print)
{
	char text[20];  //creating array that will store the print data
	sprintf_s(text, string_to_print);
	putText(src, text, Point(10, 50), Font_Type, Font_Scale, Font_Colour, 2); //Display the text in image window
	ImageShowOnDebug("text", src);
	cout << string_to_print;
}