/*
 * Corner_Detect.h
 *
 *  Created on: Aug 9, 2015
 *      Author: u1310pass123
 */

#ifndef CORNER_DETECT_H_
#define CORNER_DETECT_H_

#include <iostream>
#include <cv.h>
#include <highgui.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include "IPM.h"
#include "fftlib.h"
using namespace cv;

#define KEY_N 200
#define GB_SIZE 157 //157
#define GB_SIGMA 221
#define WHITE_MIN_THR 180
#define WHITE_MAX_THR 255
#define LOW_BASE 500


void whiteLineFilter(const Mat& Src ,Mat &Dst, int key_n, int gb_size, int gb_sigma);
IPM invPrespctive(Mat &inputImg,Mat &outputImg,int lowBase , int topHeight);
void findCenterCircle(const Mat& inputImg, vector<Vec3f> circles);
void printCircles(const Mat& inputImg ,vector<Vec3f> circles);
vector<Vec4i> findLines(const Mat& inputImg);
void printLines(const Mat& inputImg,vector<Vec4i> lines);
void dftPrc(const Mat& InputImg);
void dftPrc2(const Mat& Src, Mat& Dst,int ch_index,int radius);
void HSV_Threshold(const Mat & Src,Mat &Dst);
void HLS_Threshold(const Mat & Src,Mat &Dst ,int Hmin,int Lmin,int Smin,int Hmax, int Lmax,int Smax);
int highest1(const Mat & Src,int size);//return the Y coordinate of the first 1 chank with size size

#endif /* CORNER_DETECT_H_ */
