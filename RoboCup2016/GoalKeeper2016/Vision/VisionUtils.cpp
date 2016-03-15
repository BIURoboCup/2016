#include "VisionUtils.h"

Scalar minWhiteHSV = Scalar(0, 0, 170);
Scalar maxWhiteHSV = Scalar(255, 50, 255);
Scalar minGreenBGR = Scalar(0, 70, 0);
Scalar maxGreenBGR = Scalar(100, 255, 100);

namespace Colors
{
	Scalar Blue = Scalar(255, 0, 0);
	Scalar Red = Scalar(0, 0, 255);
	Scalar Green = Scalar(0, 255, 0);
	Scalar Purple = Scalar(255, 0, 255);
}

void ImageShowOnDebug(const string& winName, Mat image)
{
#define DEBUG 0
	if (DEBUG == 1)
	{
		imshow(winName, image);
	}
}

void DrawPoint(Mat &image, Point2f &point, Scalar color, int thickness)
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

IplImage* ConvertImageToOpenCV(FlyCapture2::Image* pImage)
{
	FlyCapture2::Image colorImage;
	bool bInitialized = false;

	IplImage* cvImage = NULL;
	bool bColor = true;
	CvSize mySize;
	mySize.height = pImage->GetRows();
	mySize.width = pImage->GetCols();

	switch ( pImage->GetPixelFormat() )
	{
		case FlyCapture2::PIXEL_FORMAT_MONO8:	 cvImage = cvCreateImageHeader(mySize, 8, 1 );
									 cvImage->depth = IPL_DEPTH_8U;
									 cvImage->nChannels = 1;
									 bColor = false;
									 break;
		case FlyCapture2::PIXEL_FORMAT_411YUV8:   cvImage = cvCreateImageHeader(mySize, 8, 3 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_422YUV8:   cvImage = cvCreateImageHeader(mySize, 8, 3 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_444YUV8:   cvImage = cvCreateImageHeader(mySize, 8, 3 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_RGB8:      cvImage = cvCreateImageHeader(mySize, 8, 3 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_MONO16:    cvImage = cvCreateImageHeader(mySize, 16, 1 );
                                     cvImage->depth = IPL_DEPTH_16U;
                                     cvImage->nChannels = 1;
									 bColor = false;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_RGB16:     cvImage = cvCreateImageHeader(mySize, 16, 3 );
                                     cvImage->depth = IPL_DEPTH_16U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_S_MONO16:  cvImage = cvCreateImageHeader(mySize, 16, 1 );
                                     cvImage->depth = IPL_DEPTH_16U;
                                     cvImage->nChannels = 1;
									 bColor = false;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_S_RGB16:   cvImage = cvCreateImageHeader(mySize, 16, 3 );
                                     cvImage->depth = IPL_DEPTH_16U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_RAW8:      cvImage = cvCreateImageHeader(mySize, 8, 3 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_RAW16:     cvImage = cvCreateImageHeader(mySize, 8, 3 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_MONO12:    printf("Not supported by OpenCV");
									 bColor = false;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_RAW12:	 printf("Not supported by OpenCV");
									 break;
		case FlyCapture2::PIXEL_FORMAT_BGR:       cvImage = cvCreateImageHeader(mySize, 8, 3 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 3;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_BGRU:      cvImage = cvCreateImageHeader(mySize, 8, 4 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 4;
                                     break;
		case FlyCapture2::PIXEL_FORMAT_RGBU:      cvImage = cvCreateImageHeader(mySize, 8, 4 );
                                     cvImage->depth = IPL_DEPTH_8U;
                                     cvImage->nChannels = 4;
                                     break;
		default: printf("Some error occured...\n");
				 return NULL;
	}

	if(bColor) {
		if(!bInitialized)
		{
			colorImage.SetData(new unsigned char[pImage->GetCols() * pImage->GetRows()*3], pImage->GetCols() * pImage->GetRows()*3);
			bInitialized = true;
		}

		pImage->Convert(FlyCapture2::PIXEL_FORMAT_BGR, &colorImage); //needs to be as BGR to be saved

	    cvImage->width = colorImage.GetCols();
		cvImage->height = colorImage.GetRows();
		cvImage->widthStep = colorImage.GetStride();

	    cvImage->origin = 0; //interleaved color channels

		cvImage->imageDataOrigin = (char*)colorImage.GetData(); //DataOrigin and Data same pointer, no ROI
	    cvImage->imageData         = (char*)(colorImage.GetData());
		cvImage->widthStep		= colorImage.GetStride();
	    cvImage->nSize = sizeof (IplImage);
	    cvImage->imageSize = cvImage->height * cvImage->widthStep;
	}
	else
	{
        cvImage->imageDataOrigin = (char*)(pImage->GetData());
        cvImage->imageData         = (char*)(pImage->GetData());
        cvImage->widthStep         = pImage->GetStride();
        cvImage->nSize             = sizeof (IplImage);
        cvImage->imageSize         = cvImage->height * cvImage->widthStep;

		//at this point cvImage contains a valid IplImage
     }
	return cvImage;
}
