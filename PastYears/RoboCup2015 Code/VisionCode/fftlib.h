/*
 * fftlib.h
 *
 *  Created on: Aug 3, 2015
 *      Author: u1310pass123
 */

#ifndef FFTLIB_H_
#define FFTLIB_H_

#include <string>
#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

void Recomb(Mat &src,Mat &dst);
void ForwardFFT(Mat &Src, Mat *FImg);
void InverseFFT(Mat *FImg,Mat &Dst);
void ForwardFFT_Mag_Phase(Mat &src, Mat &Mag,Mat &Phase);
void InverseFFT_Mag_Phase(Mat &Mag,Mat &Phase, Mat &dst);
void shiftDFT(Mat &fImage );
void LPF(Mat &Src,Mat &Dst,int r=120);
void BPF_HPF(Mat &Src, Mat &Dst,int r=120, int R=250, int bh=0);

#endif /* FFTLIB_H_ */
