#include "../Includes.h"
#include "../Constants.h"

#ifndef VISIONUTILS_H
#define VISIONUTILS_H

// Calibration
extern Scalar minWhiteHSV;
extern Scalar maxWhiteHSV;
extern Scalar minGreenBGR;
extern Scalar maxGreenBGR;

// Colors for drawing
namespace Colors
{
	extern Scalar Blue;
	extern Scalar Red;
	extern Scalar Green;
	extern Scalar Purple;
}

void FindField(Mat& inputImage, Mat& field);
void GetWhiteImage(Mat& inputImage, Mat& onlyWhiteImage);

void ImageShowOnDebug(const string& winName, Mat image);

void DrawPoint(Mat &image, Point2f &point, Scalar color = Scalar(0,0,255), int thickness = 1);
void DrawRectangle(Mat &image, RotatedRect &rect, Scalar color = Scalar(0,0,255), int thickness = 1);
void PrintStringOnImage(Mat &src, const char* string_to_print);

#endif
