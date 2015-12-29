#include <stdio.h>
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/flann/miniflann.hpp"

#include "ccolor.hpp" // my console color
#include "cv_windows.hpp"

#define PI 3.14159265358979323846

#define loopi(count) for(int i=0;i<count;i++)

#define SETTING_WIDTH 640	// The width of the settings windows.

using namespace cv; // all the new API is put into "cv" namespace. Export its content
using namespace std;
using namespace cv::flann;

cColor cGreen(cColor::attr::Bold, cColor::cText::Green, cColor::cBg::NoChange),
	   cBold(cColor::attr::Bold, cColor::cText::NoChange, cColor::cBg::NoChange),
	   cUnderline(cColor::attr::UnderScore, cColor::cText::NoChange, cColor::cBg::NoChange);

// Grass value range:
int v[6] = {39, 91 ,0,
			63, 255, 255};

// Ball value range:
int vBall[6] = {21, 0 ,0,
			73, 91, 255};

/*int vBall[6] = {26, 12 ,250,
			45, 41, 255};*/

int gblur = 1;	// Guassian blur
int mblur = 7;	// Median Blur
int canny_parm[2] = {100,200};

int par[2] = {100, 50};	// 2 Parameters for cv--Hough ball detection
int current_img = 3; 	// 1-3 png images

Mat im1, im1_info,					// The original image + image after detection
	im1hsv, im1grey,				// HSV + Grey
	grass, raw_shapes, convex,		// Convex mask of grass.
	ball, masked_ball,				// Finding the ball.
	im1edges;


// Empty image to use on our settings windows
Mat EmptyImage(1,SETTING_WIDTH,CV_8UC1,Scalar(1));

cvWin winMainSettings("Field Grass"),
	  //winContour("The raw shape (biggest?)"),
	  //winConvex("Convex Try"),
	  winBallSettings("Finding the ball using 2 Methods"),
	  winCurrent("The current Picture"),
	  winOrigin("Original");

int currentStep = 2;
#define STEPS_COUNT 8

void mouseCallBack( int event, int x, int y, int flags, void* userdata) {
	static Point2i last; 				// The points for use in our rectangle
	static bool inprogress = false;		// Are we expecting end?

	// && - Logical AND
	// & - Bitwise AND

	if ( EVENT_LBUTTONDOWN == event && (EVENT_FLAG_CTRLKEY & flags) !=0 ) {
		inprogress = true;
		last = Point2i(x,y);

	} else if (EVENT_LBUTTONUP == event && inprogress) { // We dont care any more about Ctrl
		inprogress = false;
		// Finising draging:
		printf("Rect: (%d,%d) --> (%d,%d)\n",
				last.x, last.y, x,y
				);
	}else if (inprogress && EVENT_MOUSEMOVE == event){
		// Show feedback on image:
		Mat feedback = im1_info.clone();
		cv::rectangle(feedback,last,Point2i(x,y),Scalar(0,255,0),2);
		winOrigin.imshow(feedback);
	}

	// Debug: cout << "event: " << event << " flag: " << flags << endl;
}

void showStepResult(int a=0, void* b=0) {
	Mat allSteps[STEPS_COUNT] =
		{im1, im1hsv, grass, raw_shapes, convex,
				ball, masked_ball, im1edges};

	if (currentStep > STEPS_COUNT-1 || currentStep < 0) {
		cout << "Current step (value: " << currentStep << ") is out of bound!!\n";
	}



	if (
			!allSteps[currentStep].empty() && allSteps[currentStep].data)
		winCurrent.imshow(allSteps[currentStep]);

	// Always show the original:
	winOrigin.imshow(im1_info);

}

void findBall(int a=0, void* b=0) {
	cout << "Ball tracking ...\n";


	//Scalar MIN_HSV(vBall[0],vBall[1],vBall[2]),		MAX_HSV(vBall[3],vBall[4],vBall[5]);
	Scalar MIN_HSV(v[0],v[1],v[2]),		MAX_HSV(v[3],v[4],v[5]);
	cv::inRange(im1hsv,MIN_HSV,MAX_HSV,ball); // Specific range for ball
	cv::bitwise_not(ball,ball);

	// Mask:
	if (!ball.empty() && !convex.empty()) {
		cv::bitwise_and(ball,convex,masked_ball);
	}else{
		cout << "No data ...\n";
		return; // No data to process...

	}

	//if (gblur > 0 ) cv::blur(masked_ball,masked_ball,Size2i(gblur*2 +1 ,gblur *2 + 1));
	if (mblur > 0 ) cv::medianBlur(masked_ball,masked_ball ,mblur*2 +1 );

	vector<vector<Point> > _contours; // Each contour stored as an array of points according to API.
	vector<Vec4i> _heir;

	Mat blur_cpy = masked_ball.clone();
	// ** Warning! ** the input Mat data will change! so we use a copy.
	cv::findContours(blur_cpy, _contours, _heir,
			CV_RETR_EXTERNAL,			// Only external shapes. (not one inside other)
			CV_CHAIN_APPROX_SIMPLE);	// how much point data about shape (From API )

	blur_cpy.release();

	vector<Point2f> _center;
	vector<float> _radius;
	_center.resize(_contours.size());
	_radius.resize(_contours.size());

	Point2f max_center;
	float max_radius;
	float max_ratio = -1 ;

	loopi((int)_contours.size()) {
		//cv::Rect bound = cv::boundingRect(Mat(_contours[i]));
		//cv::rectangle(masked_ball, bound, Scalar(120,120,120),1,4);

		minEnclosingCircle(Mat(_contours[i]),_center[i],_radius[i]);
		cv::circle(masked_ball, _center[i], (int)_radius[i]+1,
				Scalar(126,126,126),1,4);

		double area =  cv::contourArea(_contours[i]);

		if ( max_ratio < area/(PI*powf(_radius[i],2)) ) {
			max_ratio = area/(PI*powf(_radius[i],2));
			max_radius = _radius[i];
			max_center = _center[i];
		}


		//cout << i <<  ") " << area/(PI*powf(_radius[i],2))
		//		<< " (" << _center[i].x << "," << _center[i].y << ")" <<
		//		endl;
	}

	cv::circle(masked_ball, max_center, (int)max_radius+1,
					Scalar(255,255,255),4,4);

	im1_info=im1.clone();
	// On original image:
	cv::circle(im1_info, max_center, (int)max_radius+1,
						Scalar(255,0,0),4,4);

	cout << "MAX: " << max_ratio
			<< " (" << max_center.x << "," << max_center.y << ")" <<
			endl;

	im1edges = im1grey.clone();
	cv::blur(im1edges,im1edges,Size2i(gblur*2+1,gblur*2+1));
	cv::Canny(im1edges,im1edges,canny_parm[0],canny_parm[1]);




	/**** Circle finding (Good for Big balls) with medianblur=9, p1=110, p2=36.
	vector<Vec3f> _circles; // x,y + radius
	cv::HoughCircles( masked_ball, _circles, CV_HOUGH_GRADIENT, 1 ,masked_ball.rows/8, par[0]+2, par[1]+1, 0 , 0 );

	/// Draw the circles detected
	cout << "Found " << _circles.size() << "Circles. \n";
	loopi((int)_circles.size())
	{
		Point center(cvRound(_circles[i][0]), cvRound(_circles[i][1]));
		int radius = cvRound(_circles[i][2]);
		// circle center
		cv::circle( masked_ball, center, 3, Scalar(255,255,255), -1, 8, 0 );
		// circle outline
		cv::circle( masked_ball, center, radius,  Scalar(255,255,255), 3, 8, 0 );
	}
	***/


	showStepResult();
}

void findGrass(int a=0, void* b=0) {
	cout << "Finding Field...\n";

	// We find the grass using hsv filter because the green of th grass
	// 		can be grouped to one chunk in HSV space.

	Scalar MIN_HSV(v[0],v[1],v[2]),		MAX_HSV(v[3],v[4],v[5]);
	cv::inRange(im1hsv,MIN_HSV,MAX_HSV,grass);



	vector<vector<Point> > _contours; // Each contour stored as an array of points according to API.
	vector<Vec4i> _heir;

	Mat grass_cpy = grass.clone();


	// ** Warning! ** the input Mat data will change! so we use a copy.
	cv::findContours(grass_cpy, _contours, _heir,
			CV_RETR_EXTERNAL,			// Only external shapes. (not one inside other)
			CV_CHAIN_APPROX_SIMPLE);	// how much point data about shape (From API )

	grass_cpy.release(); // Release copy of data.


	cv::threshold(grass,raw_shapes,255,0,THRESH_BINARY); // Make shapes black 1U image.
	convex = raw_shapes.clone(); // Clone (copy) blank image with same Dimention (Width,Height);

	int maxindex =-1; // Find the max shape by area = the green field.
	int maxArea = -1, currentArea;
	loopi((int)_contours.size()) {
		if ((currentArea = cv::contourArea(_contours[i],false)) > maxArea) {
			maxindex = i;
			maxArea = currentArea;
		}
	}

	if (maxindex > -1) { // Found field

		//  Find Convex of field:
		vector<vector<Point> > hull;
		hull.push_back(vector<Point>());

		cv::convexHull(_contours[maxindex],hull[0],true,true);
		cv::drawContours(convex,hull,0,Scalar(255,255,255),-1,4);



		// Draw field without convex:

		cv::drawContours(raw_shapes,_contours,
				maxindex,						// Inddex to draw -1 for all.
				Scalar(255,255,255),3,
				4,								// Drawing percision - more = slower (4 look like enough).
				_heir,2);


	}


	findBall();
}

cv::VideoCapture cap;

void openCamera() {
		cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		cap.set(CV_CAP_PROP_FRAME_WIDTH, 480);

		if (!cap.open(0)) {
			cout << "Cant open camera!\n";
			exit(-1);
		}
}

void loadCameraImg() {
	cap.read(im1);
	while (im1.empty()) {cap.read(im1);} // Solve bugs with our camera.

	cv::cvtColor(im1,im1hsv,CV_BGR2HSV);
	cv::cvtColor(im1,im1grey,CV_BGR2GRAY);

	// Find grass and ball on the new image:
	findGrass();
}

void loagPNGImage(int a=0, void* b=0) {
	if (current_img > 3 || current_img < 1) return;
	cout << "Loading new image ...\n";

	std::stringstream path;
	path << "img" << current_img << ".png";

	im1 = cv::imread(path.str().c_str(), CV_LOAD_IMAGE_COLOR);
		if (im1.empty()) {
			cout << "Error loading file: " << path.str() << endl;
			exit(-1);
		}


	cv::cvtColor(im1,im1hsv,CV_BGR2HSV);
	cv::cvtColor(im1,im1grey,CV_BGR2GRAY);

	// Find grass and ball on the new image:
	findGrass();
}

#define CAMERA_FEED false

int main( int argc, char** argv ) {
	cout << "Current path: \n" << cUnderline << flush;
	system("pwd");
	cout << cColor::Default <<  '\n';



	//* Creating first setting to control finding the grass:
	winMainSettings.createTrackbar("Current Step:", &currentStep, STEPS_COUNT-1, showStepResult);
	winMainSettings.createTrackbar(".PNG Source", &current_img , 3, loagPNGImage);
	//** 		Min HSV to grass:
	winMainSettings.createTrackbar("H min", v+0,180, findGrass);
	winMainSettings.createTrackbar("S min", v+1,255, findGrass);
	winMainSettings.createTrackbar("V min", v+2,255, findGrass);
	//**		Max HSV to grass:
	winMainSettings.createTrackbar("H max", v+3,180, findGrass);
	winMainSettings.createTrackbar("S max", v+4,255, findGrass);
	winMainSettings.createTrackbar("V max", v+5,255, findGrass);

	//* Creating second setting to control finding of ball:
	winBallSettings.createTrackbar("Median Blur", &mblur ,255, findBall);
	winBallSettings.createTrackbar("Gaussian Blur", &gblur ,255, findBall);
	winBallSettings.createTrackbar("Circle Par1 ", par ,1000, findBall);
	winBallSettings.createTrackbar("Circle Par2 ", par+1 ,1000, findBall);
	winBallSettings.createTrackbar("Edge Par1 ", canny_parm ,1000, findBall);
	winBallSettings.createTrackbar("Edge Par2 ", canny_parm+1 ,1000, findBall);


	winMainSettings.imshow(EmptyImage);
	winBallSettings.imshow(EmptyImage);

	winOrigin.setMouseCallback(mouseCallBack);

	char quit = '\0';
	if (CAMERA_FEED) {
		openCamera();

		while ('q' != quit) {
			// Find grass -> finding ball -> showing result:
			// Load image from Camera.
			loadCameraImg();
			findGrass();
			quit = cv::waitKey(60);
		}

	}else{
		// Find grass -> finding ball -> showing result:
		// Load image from saved PNG in the exe directory:
		loagPNGImage();
		findGrass();
		while ('q'!=quit)
			quit=cv::waitKey(60);
	}

	// Return to the OS with default console color.
	cout << cColor::Default << endl;
}

