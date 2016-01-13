#include "VisionUtils.h"

Scalar minWhiteHSV = Scalar(0, 0, 170);
Scalar maxWhiteHSV = Scalar(255, 50, 255);
Scalar minGreenBGR = Scalar(0, 70, 0);
Scalar maxGreenBGR = Scalar(100, 255, 100);

void ImageShowOnDebug(const string& winName, Mat image)
{
#define DEBUG 1
	if (DEBUG == 1)
	{
		imshow(winName, image);
	}
}

void DrawPoint(Mat &image, Point2i &point, Scalar color, int thickness)
{
	circle(image, point, 10, color, 1);
}

void DrawRectangle(Mat &image, RotatedRect &rect, Scalar color, int thickness)
{
	Point2f rect_points[4];
	rect.points(rect_points);
	for (int j = 0; j < 4; j++)
	{
		line(image, rect_points[j], rect_points[(j + 1) % 4], color, thickness);
	}
}

void FindField(Mat& inputImage, Mat& field)
{
	Mat onlyGreenImage;
	inRange(inputImage, minGreenBGR, maxGreenBGR, onlyGreenImage);
	Mat grass_cpy = onlyGreenImage.clone();
	Mat raw_shapes;
	ImageShowOnDebug("grass_cpy",grass_cpy);
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

void PrintStringOnImage(Mat &src, const char* string_to_print)
{
	char text[40];  //creating array that will store the print data
	sprintf(text, string_to_print);
	putText(src, text, Point(10, 50), Font_Type, Font_Scale, Font_Colour, 2); //Display the text in image window
	ImageShowOnDebug("text", src);
	cout << string_to_print << "\n";
}
