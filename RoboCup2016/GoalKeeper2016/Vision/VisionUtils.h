#include "../Includes.h"
#include "../Constants.h"
#include <Calibrator.h>

#ifndef VISIONUTILS_H
#define VISIONUTILS_H

void FindField(Mat& inputImage, Mat& field);
void GetWhiteImage(Mat& inputImage, Mat& onlyWhiteImage);

void ImageShowOnDebug(const string& winName, Mat image);

void DrawPoint(Mat &image, Point2i &point);
void DrawRectangle(Mat &image, RotatedRect &rect);
void PrintStringOnImage(Mat &src, const char* string_to_print);

#endif