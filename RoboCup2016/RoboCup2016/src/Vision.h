/*
 * VisionMain.h
 *
 *  Created on: Nov 21, 2014
 *      Author: laptop1
 */

#ifndef VISIONMAIN_H_
#define VISIONMAIN_H_

//=========================/ INCLUDES /==========================//

#include <iostream>
#include <string>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/flann/miniflann.hpp"
#include <math.h>
#include <signal.h>
#include <fstream>
#include "consts.h"


using namespace cv; // all the new API is put into "cv" namespace. Export its content
using namespace std;
using namespace cv::flann;


//=========================/ STRUCTURES /=======================//

struct ColorRange
{
    int H_max;
    int S_max;
    int V_max;

    int H_min;
    int S_min;
    int V_min;

    ColorRange():H_max(0),S_max(0),V_max(0),H_min(180),S_min(255),V_min(255)
    {}
};

struct Ball
{
	Point2f center;
	float radius;
	bool found;

    Ball():center(0,0),radius(0), found(false)
    {}
};

//=========================/ CLASSES /==========================//

class Vision
{
public:
	Vision(void);    //constructor
	~Vision(void);

	//////////////////////////////////////////
	//Global
    VideoCapture capture;

	void 		OpenFlyCapCamera();
	void 		GetImage(Mat& Im);
	void 		ShowImage(Mat& Im);
	void 		CreateStreamWindow();
	void 		CloseCamera();
	void 		LoadColorsFromFile();
	void 		saveHSVtofile();
	void        editColorRange(Mat& Im);
	void        showGreen(Mat& Im);
	void 		createGreen();
	void 		closeGreen();

	static void onMouse(int event, int x, int y, int flag, void* param);

	int m_calibrate,m_calib_open,m_calib_close;
	int m_load_file;
	int m_save_file;
	char key;

	static int m_Static_mousePos_x;
	static int m_Static_mousePos_y;
	static int m_type_of_click;
	static bool m_hanled_click;

	ColorRange White,Green;


	//////////////////////////////////////////
	//Distance
	float CameraHight,TILT,Fy ,Fx;
	float motionTilt;

	Point2f uv2xy(int,int);//calculates x,y plane (in relation to camera) coordinates, from picture u,v plane coordinates.
	template <typename T>
		float magnitude(T,T); //calculates magnitude (distance) of point(x,y).
	template <typename _T>
		float magnitude(Point_<_T>);	//calculates magnitude (distance) of point.
	inline void centeruv(int&, int&); //adjusts u,v coordinates so they relate to center of camera.


	//////////////////////////////////////////
	//Find Ball
	Ball Find_Ball(Mat& im1);


};

#endif /* VISIONMAIN_H_ */

