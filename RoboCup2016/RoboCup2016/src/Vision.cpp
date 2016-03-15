#include "Vision.h"
#include <math.h>

int Vision::m_type_of_click= 0;
int Vision::m_Static_mousePos_x=0;
int Vision::m_Static_mousePos_y=0;
bool Vision::m_hanled_click = true;

/*****************************************************************************************
* Method Name: VisionMain
* Description: C'tor
* Arguments: None
* Return Values: None
* Wrote: Roman & Gilad
*****************************************************************************************/
Vision::Vision()
{
	m_calibrate=0;
	m_calib_open=1;
	m_calib_close=0;
	m_load_file=0;
	m_save_file=0;
	CameraHight= ROBOHEIGHT;
	TILT=M_PI_2 + 0.331424;
	Fy= 773.111;
	Fx=759.598;
	motionTilt= M_PI - 1.313;
	key = 'y';
}


/*****************************************************************************************
* Method Name: ~VisionMain
* Description: D'tor
* Arguments: None
* Return Values: None
* Wrote: Roman & Gilad
*****************************************************************************************/
Vision::~Vision()
{

}


/*****************************************************************************************
* Method Name: GetImage
* Description:
* Arguments: None
* Return Values: None
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::GetImage(Mat& Im)
{
	capture >> Im;
	key=waitKey(1);
}



/*****************************************************************************************
* Method Name: OpenCamera
* Description:
* Arguments: None
* Return Values: None
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::OpenFlyCapCamera()
{
	//Init capture object
	int tries = 0;

	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	while (!capture.open(0))
	{
		printf("trying to open camera\n");
		if (++tries>10)
		{
			printf("couldn't open camera capture, gave up\n");
			throw new string("Couldn't open camera");
		}
		printf("couldn't open camera capture, try #%d\n", tries);
	}
	//printf("camera open, try #%d\n", tries);
}



/*****************************************************************************************
* Method Name: CreateStreamWindow
* Description:
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::CreateStreamWindow()
{
	namedWindow("Stream", CV_WINDOW_AUTOSIZE);
	createTrackbar( "From file", "Stream", &m_load_file,1,0);
	createTrackbar( "To file", "Stream", &m_save_file,1,0);
	createTrackbar( "Calibrate", "Stream", &m_calibrate,1,0);
	setMouseCallback("Stream",Vision::onMouse, 0);
}



/*****************************************************************************************
* Method Name: ShowImage
* Description:
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::ShowImage(Mat& Im)
{
	imshow("Stream", Im );
}


/*****************************************************Trackbar( "To file", "Stream", &m_save_file,1,0);
	createTrackbar( "Calibrate", "Stream", &m_calibrate,1,0);
	setMouseCallback("Stream",Vision::onMouse, 0);************************************
* Method Name: CloseCamera
* Description:
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::CloseCamera()
{
	capture.release();
}


/*****************************************************************************************
* Method Name: onMouse
* Description: function that gets mouse click location
* Arguments: none
* Return Values: none
* Owner: Roman & Gilad
*****************************************************************************************/
void Vision::onMouse(int event, int x, int y, int flag, void* param)
{
	m_Static_mousePos_x = x;
	m_Static_mousePos_y = y;
	m_type_of_click = event;
	m_hanled_click = false;
}

/*****************************************************************************************
* Method Name: showGreen
* Description:
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::showGreen(Mat& Im)
{
	Mat HSV,thresholdGreen;

	cvtColor(Im, HSV, CV_BGR2HSV);
	inRange(HSV, Scalar(Green.H_min, Green.S_min, Green.V_min), Scalar(Green.H_max, Green.S_max, Green.V_max), thresholdGreen);
	imshow("Green", thresholdGreen );
}

/*****************************************************************************************
* Method Name: createGreen
* Description:
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::createGreen()
{
	namedWindow("Green", CV_WINDOW_AUTOSIZE);
}

/*****************************************************************************************
* Method Name: closeGreen
* Description:
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void Vision::closeGreen()
{
	destroyWindow("Green");
}


/*****************************************************************************************
* Method Name: editColorRange
* Description: function that gets HSV values after mouse click when calibration mode set by trackbar
* Arguments: none
* Return Values: none
* Owner: Roman & Gilad
*****************************************************************************************/
void Vision::editColorRange(Mat& Im)
{
	static int HMin_G_P=180, SMin_G_P=255, VMin_G_P=255, HMax_G_P=0, SMax_G_P=0, VMax_G_P=0;  //calibrateBall sets green

	if (m_hanled_click)
		return;

	Vec3b p;  //vector to store HSV values at mouse click
	Vec3b sur[9];  // array to store HSV values surrounding mouse click
	Mat HSV;

	cvtColor(Im, HSV, CV_BGR2HSV);

	int HMin_G=Green.H_min, SMin_G=Green.S_min, VMin_G=Green.V_min, HMax_G=Green.H_max, SMax_G=Green.S_max, VMax_G=Green.V_max;  //calibrateBall sets green


	if (m_type_of_click==CV_EVENT_LBUTTONDOWN)
	{
		//To access each pixel in the images we are using this syntax: image.at(y,x)[c] where y is the row,
		 // x is the column and c is H, S or V (0, 1 or 2)
		p = HSV.at<Vec3b>(m_Static_mousePos_y,m_Static_mousePos_x);  //Vec3b - Array of 3 uchar numbers

		int x=m_Static_mousePos_x,  y=m_Static_mousePos_y;
		HMin_G_P=HMin_G; SMin_G_P=SMin_G; VMin_G_P=VMin_G;
		HMax_G_P=HMax_G; SMax_G_P=SMax_G; VMax_G_P=VMax_G;


		sur[0]=HSV.at<Vec3b>(y,x-1);
		sur[1]=HSV.at<Vec3b>(y,x);
		sur[2]=HSV.at<Vec3b>(y,x+1);
		sur[3]=HSV.at<Vec3b>(y-1,x-1);
		sur[4]=HSV.at<Vec3b>(y-1,x);
		sur[5]=HSV.at<Vec3b>(y-1,x+1);
		sur[6]=HSV.at<Vec3b>(y+1,x-1);
		sur[7]=HSV.at<Vec3b>(y+1,x);
		sur[8]=HSV.at<Vec3b>(y+1,x+1);

		for (int i=0; i<9; i++)
		{
			if (SMin_G>sur[i][1])
			{
				SMin_G=sur[i][1];
			}
			if (HMin_G>sur[i][0])
			{
				HMin_G=sur[i][0];
			}
			if (VMin_G>sur[i][2])
			{
				VMin_G=sur[i][2];
			}
			if (SMax_G<sur[i][1])
			{
				SMax_G=sur[i][1];
			}
			if (HMax_G<sur[i][0])
			{
				HMax_G=sur[i][0];
			}
			if (VMax_G<sur[i][2])
			{
				VMax_G=sur[i][2];
			}

		}
	}

	else if (m_type_of_click==CV_EVENT_MBUTTONDOWN )
	{
		HMin_G=HMin_G_P; SMin_G=SMin_G_P; VMin_G=VMin_G_P;
		HMax_G=HMax_G_P; SMax_G=SMax_G_P; VMax_G=VMax_G_P;
	}

	Green.H_min=HMin_G;
	Green.S_min=SMin_G;
	Green.V_min=VMin_G;
	Green.H_max=HMax_G;
	Green.S_max=SMax_G;
	Green.V_max=VMax_G;

	m_hanled_click=true;
}


/*****************************************************************************************
* Method Name: uv2xy
* Description: The robot is at xy(0,0) the object is at (x,y) point
* Arguments: None
* Return Values: none
* Wrote: Elimelech
*****************************************************************************************/
Point2f Vision::uv2xy(int u ,int v)	//calculates x,y plane (in relation to camera) coordinates, from picture u,v plane coordinates.
{	//for the moment we assume camera U axis to be aligned to plane.
	float x,y;
	centeruv(u,v);
	y=CameraHight*tan(M_PI -motionTilt+atan(v/Fy));//y= height * tan (angle from height to line of sight)
	if (motionTilt < (0.6*M_PI))
		{
			x=y*u/Fx;// only when tilt => pi/2 (e.g. when looking straight [or almost straight])
		}
	else
		{
			float Yc = CameraHight*tan(M_PI -motionTilt );  //y value of point on field corresponding to center of image .
			float Zc = magnitude(Yc,CameraHight);		//Z center (line of sight distance to the above point).
			float Ze = Yc* (y-Yc)/Zc;					//Z extra - extra distance to effective pinhole-model plane.
			float z = Ze+Zc;							//z is the distance to effective pinhole-model plane
			x=u*z/Fx;		//this should be right!
		}
	return Point2f(x,y);
}

/*****************************************************************************************
* Method Name: magnitude
* Description:
* Arguments: None
* Return Values: none
* Wrote: Elimelech
*****************************************************************************************/
template <typename T>
float Vision::magnitude(T x,T y)//calculates magnitude (distane) of point.
{
	return (sqrt((float)(x*x+y*y)));
}

/*****************************************************************************************
* Method Name: magnitude
* Description:
* Arguments: None
* Return Values: none
* Wrote: Elimelech
*****************************************************************************************/
template <typename _T>
float Vision::magnitude(Point_<_T> p)//calculates magnitude (distane) of point.
{
	return magnitude(p.x,p.y);
}

/*****************************************************************************************
* Method Name: centeruv
* Description:
* Arguments: None
* Return Values: none
* Wrote: Elimelech
*****************************************************************************************/
void Vision::centeruv(int &u, int &v) //centers u,v coordinates to center of camera
{
	v=V0-v; /// V axis is the oposite way (0 is up and +480 is down) so it needs a (-)
	u=u-U0;
}

/*****************************************************************************************
* Method Name: saveHSVtofile
* Description: saves White & Green ColorRange to file
* Arguments: none
* Return Values: none
* Owner: Roman & Gilad
*****************************************************************************************/
void Vision::saveHSVtofile()
{
  ofstream myfile;
  myfile.open ("/home/robot/Desktop/ColorRange");
  //myfile.open ("ColorRange");
  //myfile.open (colorFile.c_str());

  int HMin_G=Green.H_min, SMin_G=Green.S_min, VMin_G=Green.V_min, HMax_G=Green.H_max, SMax_G=Green.S_max, VMax_G=Green.V_max;  //calibrateBall sets green

  myfile << "White: H_min [" << White.H_min << "]\n";
  myfile << "White: S_min [" << White.S_min << "]\n";
  myfile << "White: V_min [" << White.V_min << "]\n";
  myfile << "White: H_max [" << White.H_max << "]\n";
  myfile << "White: S_max [" << White.S_max << "]\n";
  myfile << "White: V_max [" << White.V_max << "]\n";

  myfile <<  "\n";

  myfile << "Green: H_min [" << HMin_G << "]\n";
  myfile << "Green: S_min [" << SMin_G << "]\n";
  myfile << "Green: V_min [" << VMin_G << "]\n";
  myfile << "Green: H_max [" << HMax_G << "]\n";
  myfile << "Green: S_max [" << SMax_G << "]\n";
  myfile << "Green: V_max [" << VMax_G << "]\n";

  myfile.close();
}


/*****************************************************************************************
* Method Name: Load colors from file
* Description: Updates White & Green ColorRange
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
* http://www.cplusplus.com/doc/tutorial/files/
*****************************************************************************************/
void Vision::LoadColorsFromFile()
{
	string line;
	ifstream myfile ("/home/robot/Desktop/ColorRange");

	if (myfile.is_open())
    {
		while ( getline (myfile,line) )
	    {
			if(line.substr(0,12) == "White: H_min")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				White.H_min = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "White: H_min" << " " << White.H_min << endl;
			}

			if(line.substr(0,12) == "White: S_min")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				White.S_min = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "White: S_min" << " " << White.S_min << endl;
			}

			if(line.substr(0,12) == "White: V_min")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				White.V_min = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "White: V_min" << " " << White.V_min << endl;
			}

			if(line.substr(0,12) == "White: H_max")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				White.H_max = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "White: H_max" << " " << White.H_max << endl;
			}

			if(line.substr(0,12) == "White: S_max")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				White.S_max = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "White: S_max" << " " << White.S_max << endl;
			}

			if(line.substr(0,12) == "White: V_max")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				White.V_max = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "White: V_max" << " " << White.V_max << endl;
			}

			if(line.substr(0,12) == "Green: H_min")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				Green.H_min = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "Green: H_min" << " " << Green.H_min << endl;
			}

			if(line.substr(0,12) == "Green: S_min")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				Green.S_min = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "Green: S_min" << " " << Green.S_min << endl;
			}

			if(line.substr(0,12) == "Green: V_min")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				Green.V_min = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "Green: V_min" << " " << Green.V_min << endl;
			}

			if(line.substr(0,12) == "Green: H_max")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				Green.H_max = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "Green: H_max" << " " << Green.H_max << endl;
			}

			if(line.substr(0,12) == "Green: S_max")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				Green.S_max = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "Green: S_max" << " " << Green.S_max << endl;
			}

			if(line.substr(0,12) == "Green: V_max")
			{
				size_t pos1 = line.find('[');
				size_t pos2 = line.find(']');
				Green.V_max = atoi(line.substr(pos1+1,pos2-pos1).c_str());
				cout << "Green: V_max" << " " << Green.V_max << endl;
			}

	    }
	    myfile.close();
	  }

	  else
	  {
		  cout << "Unable to open color config file";
	  }

}



/*****************************************************************************************
* Method Name: FindBall
* Description:
* Arguments: None
* Return Values: none
* Wrote: Yoni & Avi
* Update: 2015
*****************************************************************************************/

Ball Vision::Find_Ball(Mat& im1)
{
	//return Ball();

	int SETTING_WIDTH= 640;	// The width of the settings windows.
	#define loopi(count) for(int i=0;i<count;i++)
	float PI = 3.14159265358979323846;
	//cColor cGreen(cColor::attr::Bold, cColor::cText::Green, cColor::cBg::NoChange),
		//   cBold(cColor::attr::Bold, cColor::cText::NoChange, cColor::cBg::NoChange),
		 //  cUnderline(cColor::attr::UnderScore, cColor::cText::NoChange, cColor::cBg::NoChange);
	// Grass value range:
	int v[6] = {Green.H_min, Green.S_min ,Green.V_min,
			Green.H_max, Green.S_max, Green.V_max};

	// Ball value range:
	int vBall[6] = {21, 0 ,0,
				73, 91, 255};
	int gblur = 1;	// Guassian bl1ur
	int mblur = 7;	// Median Blur
	int canny_parm[2] = {100,200};
	int par[2] = {100, 50};	// 2 Parameters for cv--Hough ball detection
	//int current_img = 3; 	// 1-3 png images


	Mat im1_info,					// The original image + image after detection
		im1hsv, im1grey,				// HSV + Grey
		grass, raw_shapes, convex,		// Convex mask of grass.
		ball, masked_ball,				// Finding the ball.
		im1edges;
	Mat EmptyImage(1,SETTING_WIDTH,CV_8UC1,Scalar(1));
	//cvWin winMainSettings("Field Grass"),
		  //winContour("The raw shape (biggest?)"),
		  //winConvex("Convex Try"),
		//  winBallSettings("Finding the ball using 2 Methods"),
		 // winCurrent("The current Picture"),
		 // winOrigin("Original");
	//int currentStep = 2;
	//int STEPS_COUNT = 8;

	cv::cvtColor(im1,im1hsv,CV_BGR2HSV);
		cv::cvtColor(im1,im1grey,CV_BGR2GRAY);


	//=========================================================
		//cout << "Finding Field...\n";

		// We find the grass using hsv filter because the green of th grass
		// 		can be grouped to one chunk in HSV space.

		Scalar MIN_HSV(v[0],v[1],v[2]);
		Scalar MAX_HSV(v[3],v[4],v[5]);
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
	//=========================================================


	//*cout << "Ball tracking ...\n";


	//Scalar MIN_HSV(vBall[0],vBall[1],vBall[2]),		MAX_HSV(vBall[3],vBall[4],vBall[5]);
	//Scalar MIN_HSV(v[0],v[1],v[2]),		MAX_HSV(v[3],v[4],v[5]);
	cv::inRange(im1hsv,MIN_HSV,MAX_HSV,ball); // Specific range for ball
	cv::bitwise_not(ball,ball);

	// Mask:
	if (!ball.empty() && !convex.empty()) {
		cv::bitwise_and(ball,convex,masked_ball);
	}else{
		cout << "No data ...\n";
		return Ball(); //null //No data to process...

	}

	//if (gblur > 0 ) cv::blur(masked_ball,masked_ball,Size2i(gblur*2 +1 ,gblur *2 + 1));
	if (mblur > 0 ) cv::medianBlur(masked_ball,masked_ball ,mblur*2 +1 );

	//vector<vector<Point> > _contours; // Each contour stored as an array of points according to API.
	//vector<Vec4i> _heir;

	Mat blur_cpy = masked_ball.clone();
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

	loopi((int)_contours.size()) {
		//cv::Rect bound = cv::boundingRect(Mat(_contours[i]));
		//cv::rectangle(masked_ball, bound, Scalar(120,120,120),1,4);

		minEnclosingCircle(Mat(_contours[i]),_center[i],_radius[i]);
		cv::circle(masked_ball, _center[i], (int)_radius[i]+1,
				Scalar(126,126,126),1,4);

		double area =  cv::contourArea(_contours[i]);


		if ( max_ratio < area/(PI*pow(_radius[i],2)) ) {
			max_ratio = area/(PI*pow(_radius[i],2));
			max_radius = _radius[i];
			max_center = _center[i];
		}


		//cout << i <<  ") " << area/(PI*powf(_radius[i],2))
		//		<< " (" << _center[i].x << "," << _center[i].y << ")" <<
		//		endl;
	}

	if (max_radius < 0.0f || max_radius > 10000.0f) printf("Radius is currupt in Vision::yoni::find_ball");

	cv::circle(masked_ball, max_center, (int)max_radius+1,
					Scalar(255,255,255),4,4);

	im1_info=im1.clone();
	// On original image:
	if(max_ratio > 0.5 && max_radius >10)//roman, you have to play with that constanta 0.6 for example
		cv::circle(im1_info, max_center, (int)max_radius+1,
						Scalar(255,0,0),4,4);

	//*cout << "MAX: " << max_ratio
	//		<< " (" << max_center.x << "," << max_center.y << ")" <<
	//		endl;

	im1edges = im1grey.clone();
	cv::blur(im1edges,im1edges,Size2i(gblur*2+1,gblur*2+1));
	cv::Canny(im1edges,im1edges,canny_parm[0],canny_parm[1]);

	Ball ballstruct;
	if(max_ratio > 0.5 && max_radius >10)//roman, you have to play with that constanta 0.6 for example
	{
		ballstruct.center= max_center;
		ballstruct.radius= max_radius;
		ballstruct.found= true;
	}
	else
	{
		ballstruct.center= max_center;
		ballstruct.radius= max_radius;
		ballstruct.found= false;	//this for the error
	}

	im1 = im1_info;
	return ballstruct;

}

/*

Ball Vision::Find_Ball(Mat im1)
{
	int SETTING_WIDTH= 640;	// The width of the settings windows.
	#define loopi(count) for(int i=0;i<count;i++)
	float PI = 3.14159265358979323846;
	//cColor cGreen(cColor::attr::Bold, cColor::cText::Green, cColor::cBg::NoChange),
		//   cBold(cColor::attr::Bold, cColor::cText::NoChange, cColor::cBg::NoChange),
		 //  cUnderline(cColor::attr::UnderScore, cColor::cText::NoChange, cColor::cBg::NoChange);
	// Grass value range:
	int v[6] = {Green.H_min, Green.S_min ,Green.V_min,
			Green.H_max, Green.S_max, Green.V_max};

	// Ball value range:
	int vBall[6] = {21, 0 ,0,
				73, 91, 255};
	int gblur = 1;	// Guassian blur
	int mblur = 7;	// Median Blur
	int canny_parm[2] = {100,200};
	int par[2] = {100, 50};	// 2 Parameters for cv--Hough ball detection
	//int current_img = 3; 	// 1-3 png images


	Mat im1_info,					// The original image + image after detection
		im1hsv, im1grey,				// HSV + Grey
		grass, raw_shapes, convex,		// Convex mask of grass.
		ball, masked_ball,				// Finding the ball.
		im1edges;
	Mat EmptyImage(1,SETTING_WIDTH,CV_8UC1,Scalar(1));
	//cvWin winMainSettings("Field Grass"),
		  //winContour("The raw shape (biggest?)"),
		  //winConvex("Convex Try"),
		//  winBallSettings("Finding the ball using 2 Methods"),
		 // winCurrent("The current Picture"),
		 // winOrigin("Original");
	//int currentStep = 2;
	//int STEPS_COUNT = 8;

	cv::cvtColor(im1,im1hsv,CV_BGR2HSV);
		cv::cvtColor(im1,im1grey,CV_BGR2GRAY);


	//=========================================================
		///*cout << "Finding Field...\n";

		// We find the grass using hsv filter because the green of th grass
		// 		can be grouped to one chunk in HSV space.

		Scalar MIN_HSV(v[0],v[1],v[2]);
		Scalar MAX_HSV(v[3],v[4],v[5]);
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
	//=========================================================


	//*cout << "Ball tracking ...\n";


	//Scalar MIN_HSV(vBall[0],vBall[1],vBall[2]),		MAX_HSV(vBall[3],vBall[4],vBall[5]);
	//Scalar MIN_HSV(v[0],v[1],v[2]),		MAX_HSV(v[3],v[4],v[5]);
	cv::inRange(im1hsv,MIN_HSV,MAX_HSV,ball); // Specific range for ball
	cv::bitwise_not(ball,ball);

	// Mask:
	if (!ball.empty() && !convex.empty()) {
		cv::bitwise_and(ball,convex,masked_ball);
	}else{
		cout << "No data ...\n";
		return Ball(); //null //No data to process...

	}

	//if (gblur > 0 ) cv::blur(masked_ball,masked_ball,Size2i(gblur*2 +1 ,gblur *2 + 1));
	if (mblur > 0 ) cv::medianBlur(masked_ball,masked_ball ,mblur*2 +1 );

	//vector<vector<Point> > _contours; // Each contour stored as an array of points according to API.
	//vector<Vec4i> _heir;

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


		if ( max_ratio < area/(PI*pow(_radius[i],2)) ) {
			max_ratio = area/(PI*pow(_radius[i],2));
			max_radius = _radius[i];
			max_center = _center[i];
		}
circ

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

	//*cout << "MAX: " << max_ratio
	//		<< " (" << max_center.x << "," << max_center.y << ")" <<
	//		endl;

	im1edges = im1grey.clone();
	cv::blur(im1edges,im1edges,Size2i(gblur*2+1,gblur*2+1));
	cv::Canny(im1edges,im1edges,canny_parm[0],canny_parm[1]);

	Ball ballstruct;
	ballstruct.center= max_center;
	ballstruct.radius= max_radius;
	ballstruct.found= true;

	return ballstruct;

}

*/




