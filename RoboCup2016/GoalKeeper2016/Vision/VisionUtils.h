#include "../Includes.h"
#include "../Constants.h"

#ifndef VISIONUTILS_H
#define VISIONUTILS_H

extern Scalar minWhiteHSV;
extern Scalar maxWhiteHSV;
extern Scalar minGreenBGR;
extern Scalar maxGreenBGR;

void FindField(Mat& inputImage, Mat& field);
void GetWhiteImage(Mat& inputImage, Mat& onlyWhiteImage);

void ImageShowOnDebug(const string& winName, Mat image);

void DrawPoint(Mat &image, Point2i &point, Scalar color = Scalar(0,0,255), int thickness = 1);
void DrawRectangle(Mat &image, RotatedRect &rect, Scalar color = Scalar(0,0,255), int thickness = 1);
void PrintStringOnImage(Mat &src, const char* string_to_print);

#endif
