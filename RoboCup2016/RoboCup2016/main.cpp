/*
 * main.cpp
 *
 *  Created on: Aug 9, 2015
 *      Author: u1310pass123
 */

#include "stdafx.h"
#include "Corner_Detect.h"

// Threshold H-L-S gap
#define TH_H 40
#define TH_L 40
#define TH_S 60
/// Global Variables
const int MAX_TH = 255;
const int MAX_ROW = 2448;
const int MAX_COL = 3264;
int Hue_min = 40;
int Light_min = 40;
int Saturation_min = 40;
int Hue_max= 70;
int Light_max = 190;
int Saturation_max = 255;

int windowTop = 1400;//2448
int windowBase = 250;
int radiusLPF = 120;
int houghParam = 81;

/// Matrices to store images
Mat image;
Mat imageCopy;
Mat ipmImg;
Mat notchFiltered;
Mat greenThreshold;
Mat dst;
Mat dstIpm;

void on_trackbar( int, void* );
void on_trackbar2( int, void* );

int main(int argc, char** argv )
{
	if ( argc != 2 )
	{
		printf("usage: DisplayImage.out <Image_Path>\n");
		return -1;
	}
	//Mat image;
	image = imread( argv[1], 1 );
	if ( !image.data )
	{
		printf("No image data \n");
		return -1;
	}
	else
	{
		 /// Create Windows

		 namedWindow("Threshold Limits", CV_WINDOW_NORMAL);
		 createTrackbar( "Hue min:" , "Threshold Limits", &Hue_min, MAX_TH, on_trackbar );
		 createTrackbar( "Light min:" , "Threshold Limits", &Light_min, MAX_TH, on_trackbar );
		 createTrackbar( "Saturation min:" , "Threshold Limits", &Saturation_min, MAX_TH, on_trackbar );
		 createTrackbar( "Hue max:" , "Threshold Limits", &Hue_max, MAX_TH, on_trackbar );
		 createTrackbar( "Light max:" , "Threshold Limits", &Light_max, MAX_TH, on_trackbar );
		 createTrackbar( "Saturation max:" , "Threshold Limits", &Saturation_max, MAX_TH, on_trackbar );
		 namedWindow("IPM window", CV_WINDOW_NORMAL);
		 createTrackbar( "Window top:" , "IPM window", &windowTop, MAX_ROW, on_trackbar2 );
		 createTrackbar( "Base top:" , "IPM window", &windowBase, MAX_ROW, on_trackbar2 );
		 createTrackbar( "LPF Radius:" , "IPM window", &radiusLPF, 1500, on_trackbar2 );
		 createTrackbar( "Hough Param:" , "IPM window", &houghParam, 401, on_trackbar2 );

	namedWindow("HLS Threshold", CV_WINDOW_NORMAL);
	namedWindow("Ipm Threshold", CV_WINDOW_NORMAL);
	//namedWindow("Ipm Threshold org", CV_WINDOW_NORMAL);
	//namedWindow("Ipm Threshold median", CV_WINDOW_NORMAL);
	namedWindow( "ipmImg", CV_WINDOW_NORMAL );
	//----------------------------------------------------------
	//Processing
	//---------------------------------------------------------
	cvtColor(image,image,CV_BGR2HLS);//convert to HLS
	on_trackbar( Light_min, 0 );//Threshold green to find IPM window
	on_trackbar2( windowTop, 0 );//IPM with the window value

	//Center circle detection
	//vector<Vec3f> circles = findCenterCircle(whiteLineImg);
	//printCircles(ipmImg,circles);

	//inverse transform back to original image
	//Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
	//center = ipm.applyHomographyInv(center);
//	circle( image, center, 3, Scalar(255,0,0), 5, 8, 0 );

	/*namedWindow( "centerOrgImg", CV_WINDOW_NORMAL );
	imshow( "centerOrgImg", image );*/

	//White lines detection
	vector<Vec4i> lines = findLines(whiteLineImg);
	printLines(ipmImg,lines);
	//inverse transform back to original image
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( image, ipm.applyHomographyInv(Point(lines[i][0], lines[i][1])),
        		ipm.applyHomographyInv(Point(lines[i][2], lines[i][3])), cv::Scalar(0,0,255), 1);
    }
	namedWindow( "linesOrgImg", CV_WINDOW_NORMAL );
	imshow( "linesOrgImg", image );
				 waitKey(0);

	}
	waitKey(0);
	return 0;
}

/**
 * @function on_trackbar
 * @brief Callback for trackbar
 */
void on_trackbar( int, void* )
{
	Mat medFiltered;
	medianBlur(image,medFiltered,9);
	//Threshold green field
	HLS_Threshold(medFiltered,greenThreshold,Hue_min,Light_min,Saturation_min,Hue_max,Light_max,Saturation_max);
	//finding top of green field
	windowTop=greenThreshold.rows - (highest1(greenThreshold,100)-50);
	imshow( "HLS Threshold", greenThreshold );
}

void on_trackbar2( int, void* )
{
	image.copyTo(imageCopy);
	IPM ipm = invPrespctive(imageCopy,ipmImg,windowBase,windowTop);
	dftPrc2(ipmImg,dstIpm,1,radiusLPF);//LPF on channel 1 - Lightness
	//medianBlur(ipmImg,dstIpm,9);//remove after debug
	Mat greenThreshold;
	HLS_Threshold(dstIpm,greenThreshold,Hue_min,Light_min,Saturation_min,Hue_max,Light_max,Saturation_max);
	//HLS_Threshold(ipmImg,dst,Hue_min,Light_min,Saturation_min,Hue_max,Light_max,Saturation_max);
	//Mat med;
	//medianBlur(ipmImg,med,9);
	//HLS_Threshold(med,med,Hue_min,Light_min,Saturation_min,Hue_max,Light_max,Saturation_max);
	//imshow( "Ipm Threshold org", dst );
	//imshow( "Ipm Threshold median", med );
	Mat temp;
	cvtColor(ipmImg,temp,CV_HLS2BGR);
	imshow("ipmImg",temp );
	imshow( "Ipm Threshold", greenThreshold );

	Mat whiteLineImg;
	whiteLineFilter(greenThreshold,whiteLineImg,KEY_N,GB_SIZE,GB_SIGMA);
	namedWindow( "whiteLineImg", CV_WINDOW_NORMAL );
	imshow("whiteLineImg", whiteLineImg);


	//White lines detection
	vector<Vec4i> lines = findLines(whiteLineImg);
	Mat ipmImgCopy;
	ipmImg.copyTo(ipmImgCopy);
	printLines(ipmImgCopy,lines);

	//inverse transform to original image
	for( size_t i = 0; i < lines.size(); i++ )
	{
		line( imageCopy, ipm.applyHomographyInv(Point(lines[i][0], lines[i][1])),
		ipm.applyHomographyInv(Point(lines[i][2], lines[i][3])), cv::Scalar(0,0,255), 1);
	}
	cvtColor(imageCopy,imageCopy,CV_HLS2BGR);
	namedWindow( "linesOrgImg", CV_WINDOW_NORMAL );
	imshow( "linesOrgImg", imageCopy );

	 // Apply the Hough Transform to find the circles

	vector<Vec3f> circles;
	int minRadius = 250;
	 HoughCircles( whiteLineImg, circles, CV_HOUGH_GRADIENT, 1, whiteLineImg.rows, 1000, 20, minRadius ,0 ); //
	 // draw center circle
		Point center(cvRound(circles[0][0]), cvRound(circles[0][1]));
	    int radius = cvRound(circles[0][2]);
	    //cvtColor(ipmImgCopy,ipmImgCopy,CV_HLS2BGR);//remove if not printing
	    // circle center
	    circle( ipmImgCopy, center, 3, Scalar(0,255,0), -1, 8, 0 );
	    // circle outline
	    circle( ipmImgCopy, center, radius, Scalar(0,255,0), 3, 8, 0 );

		namedWindow( "findCenterCircle", CV_WINDOW_NORMAL );
		imshow( "findCenterCircle", ipmImgCopy );

}
