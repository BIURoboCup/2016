//#pragma once
#include "fftlib.h"

//----------------------------------------------------------
// Recombinate quadrants
//----------------------------------------------------------
void Recomb(Mat &src,Mat &dst)
{
    int cx = src.cols>>1;
    int cy = src.rows>>1;
    Mat tmp;
    tmp.create(src.size(),src.type());
    src(Rect(0, 0, cx, cy)).copyTo(tmp(Rect(cx, cy, cx, cy)));
    src(Rect(cx, cy, cx, cy)).copyTo(tmp(Rect(0, 0, cx, cy)));  
    src(Rect(cx, 0, cx, cy)).copyTo(tmp(Rect(0, cy, cx, cy)));
    src(Rect(0, cy, cx, cy)).copyTo(tmp(Rect(cx, 0, cx, cy)));
    dst=tmp;
}
//----------------------------------------------------------
// Forward fft
//----------------------------------------------------------
void ForwardFFT(Mat &Src, Mat *FImg)
{
    Mat fI1;
    Src.convertTo(fI1, CV_32F);

    //expand input image to optimal size
    int m = getOptimalDFTSize( Src.rows );
    int n = getOptimalDFTSize( Src.cols );

    Mat padded1;

    // on the border add zero values
    copyMakeBorder(fI1, padded1, 0, m - Src.rows, 0, n - Src.cols, BORDER_CONSTANT, Scalar::all(0));

    //Perform DFT
    Mat fourierTransform1;
    Mat planes1[2];

    dft(fI1, fourierTransform1, DFT_SCALE|DFT_COMPLEX_OUTPUT);

    shiftDFT(fourierTransform1);
    split(fourierTransform1, planes1);// planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))

    FImg[0]=planes1[0].clone();
    FImg[1]=planes1[1].clone();
}
//----------------------------------------------------------
// Inverse FFT
//----------------------------------------------------------
void InverseFFT(Mat *FImg,Mat &Dst)
{
	Mat fourierTransform1;
	merge(FImg, 2, fourierTransform1);
	shiftDFT(fourierTransform1);

	//Perform IDFT
	dft(fourierTransform1, Dst, DFT_INVERSE|DFT_REAL_OUTPUT);
}
//----------------------------------------------------------
// Forward FFT using Magnitude and phase
//----------------------------------------------------------
void ForwardFFT_Mag_Phase(Mat &src, Mat &Mag,Mat &Phase)
{
    Mat planes[2];
    ForwardFFT(src,planes);
    Mag.zeros(planes[0].rows, planes[0].cols, CV_32F);
    Phase.zeros(planes[0].rows, planes[0].cols, CV_32F);
    cartToPolar(planes[0], planes[1], Mag, Phase);
}
//----------------------------------------------------------
// Inverse FFT using Magnitude and phase
//----------------------------------------------------------
void InverseFFT_Mag_Phase(Mat &Mag,Mat &Phase, Mat &dst)
{
    Mat planes[2];
    cv::polarToCart(Mag,Phase,planes[0],planes[1]);
    InverseFFT(planes,dst);
}

void shiftDFT(Mat &fImage )
{
    Mat tmp, q0, q1, q2, q3;

    // first crop the image, if it has an odd number of rows or columns

    fImage = fImage(Rect(0, 0, fImage.cols & -2, fImage.rows & -2));

    int cx = fImage.cols / 2;
    int cy = fImage.rows / 2;

    // rearrange the quadrants of Fourier image
    // so that the origin is at the image center

    q0 = fImage(Rect(0, 0, cx, cy));
    q1 = fImage(Rect(cx, 0, cx, cy));
    q2 = fImage(Rect(0, cy, cx, cy));
    q3 = fImage(Rect(cx, cy, cx, cy));

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

//Low Pass Filter mask of circle with r radius multiplied with Src
void LPF(Mat &Src,Mat &Dst,int r)
{
    Mat mask;
    mask.create(Src.rows,Src.cols,CV_32F);
    int cx = Src.cols/2;//>>1;
    int cy = Src.rows/2;//>>1;
    mask=1;
    cv::circle(mask,cv::Point(cx,cy),r,CV_RGB(0,0,0),-1);
    mask=1-mask; // uncomment for inversion
    cv::multiply(Src,mask,Dst);
    //namedWindow( "mask" , CV_WINDOW_NORMAL );
    //imshow("mask" , mask   );
    //imwrite("Mask.jpg",mask);

}

//Band/High Pass Filter mask ring multiplied with Src
void BPF_HPF(Mat &Src, Mat &Dst,int r, int R, int bh)
{
    Mat mask;
    mask.create(Src.rows,Src.cols,CV_32F);
    int cx = Src.cols/2;//>>1;
    int cy = Src.rows/2;//>>1;
    mask=1;
    cv::circle(mask,cv::Point(cx,cy),R,CV_RGB(0,0,0),-1);
    cv::circle(mask,cv::Point(cx,cy),r,CV_RGB(1,1,1),-1);
    if (bh == 1)
    	mask=1-mask; // uncomment for inversion

    namedWindow( "mask" , CV_WINDOW_NORMAL );
    imshow("mask" , mask   );
    imwrite("Mask.jpg",mask);
    cv::multiply(Src,mask,Dst); // uncomment to turn filter on
}
