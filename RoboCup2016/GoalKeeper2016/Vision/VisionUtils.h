#include "../Includes.h"
#include "../Constants.h"

#ifndef VISIONUTILS_H
#define VISIONUTILS_H

static Scalar minWhiteHSV = Scalar(0, 0, 170);
static Scalar maxWhiteHSV = Scalar(255, 50, 255);
static Scalar minGreenBGR = Scalar(0, 70, 0);
static Scalar maxGreenBGR = Scalar(100, 255, 100);

void ImageShowOnDebug(const string& winName, Mat image);
void DrawPoint(Mat &image, Point2i &point);
void DrawRectangle(Mat &image, RotatedRect &rect);
void FindField(Mat& inputImage, Mat& field);
void GetWhiteImage(Mat& inputImage, Mat& onlyWhiteImage);
void PrintStringOnImage(Mat &src, const char* string_to_print);

#endif