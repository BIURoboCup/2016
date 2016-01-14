#include "BallDetector.h"

BallDetector::BallDetector() {
	// TODO Auto-generated constructor stub

}

BallDetector::~BallDetector() {
	// TODO Auto-generated destructor stub
}


DetectedObject* BallDetector::FindBall(Mat& inputImage)
{
		#define loopi(count) for(int i=0;i<count;i++)

		int gausianblur = 1;
		int medianblur = 7;
		int cannyArgs[2] = {100,200};

		Mat im1_info, im1edges;

		Mat EmptyImage(1,FRAME_WIDTH,CV_8UC1,Scalar(1));

		Mat inputImageHSV, inputImageGray;
		cvtColor(inputImage,inputImageHSV,CV_BGR2HSV);
		cvtColor(inputImage,inputImageGray,CV_BGR2GRAY);

		Mat field; // B&W image represent the field.
		FindField(inputImage, field);

		Mat ball;
		cv::inRange(inputImage,minGreenBGR,maxGreenBGR,ball);
		cv::bitwise_not(ball,ball);

		// Mask:
		Mat masked_ball;
		if (!ball.empty() && !field.empty())
		{
			cv::bitwise_and(ball,field,masked_ball);
		}
		else
		{
			return new DetectedBall(); // No data to process. Ball not found.
		}

		if (medianblur > 0 )
		{
			cv::medianBlur(masked_ball,masked_ball ,medianblur*2 +1 );
		}

		Mat blur_cpy = masked_ball.clone();

		vector<vector<Point> > _contours; // Each contour stored as an array of points according to API.
				vector<Vec4i> _heir;
		// ** Warning! ** the input Mat data will change! so we use a copy.
		cv::findContours(blur_cpy, _contours, _heir,
				CV_RETR_EXTERNAL,			// Only external shapes. (not one inside other)
				CV_CHAIN_APPROX_SIMPLE);	// how much point data about shape (From API )

		blur_cpy.release();

		vector<Point2f> _center;
		vector<float> _radius;
		_radius.empty();
		_center.empty();

		_center.resize(_contours.size());
		_radius.resize(_contours.size());

		Point2f max_center(0,0);
		float max_radius = 1.0f;
		float max_ratio = -1.0f ;

		loopi((int)_contours.size())
		{

			minEnclosingCircle(Mat(_contours[i]),_center[i],_radius[i]);

			double area =  contourArea(_contours[i]);

			if ( max_ratio < area/(M_PI*pow(_radius[i],2)) )
			{
				max_ratio = area/(M_PI*pow(_radius[i],2));
				max_radius = _radius[i];
				max_center = _center[i];
			}
		}

		if (max_radius < 0.0f || max_radius > 10000.0f)
		{
			printf("Radius is corrupt in BallDetector.FindBall()");
		}

		im1edges = inputImageGray.clone();
		blur(im1edges,im1edges,Size2i(gausianblur*2+1,gausianblur*2+1));
		Canny(im1edges,im1edges,cannyArgs[0],cannyArgs[1]);

		bool isBallFound = (max_ratio > 0.5 && max_radius >10);

		return new DetectedBall(max_center, max_radius, isBallFound);
}
