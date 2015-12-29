/*
 * Corner_Detect.cpp
 *
 *  Created on: Dec 11, 2014
 *      Author: Avia Yaacov
 *		Description:
 */

#include "stdafx.h"
#include "Corner_Detect.h"

/** @function main */

void whiteLineFilter(const Mat& Src ,Mat &Dst, int key_n, int gb_size, int gb_sigma)
{
	Mat elementG = getStructuringElement( MORPH_CROSS,Size(10,10),Point(5,5) );
	Mat threshold;

	//remove noise
	dilate(Src, threshold, elementG);
	erode(threshold,threshold,elementG);

	//namedWindow( "open", CV_WINDOW_NORMAL );
    //imshow( "open", threshold );
	Mat gaus;
	GaussianBlur( threshold, gaus, Size(gb_size, gb_size), gb_sigma, gb_sigma );
	//elementG = getStructuringElement( MORPH_ELLIPSE,Size(41,41),Point(20,20) );
	//medianBlur(threshold,gaus,201);
	//dilate(threshold,gaus,elementG);
	//namedWindow( "Gaussed", CV_WINDOW_NORMAL);
	//imshow( "Gaussed", gaus );

	Mat xor_image;
	bitwise_xor(gaus,threshold,xor_image);
	//namedWindow( "xor_image", CV_WINDOW_NORMAL);
	//imshow( "xor_image", xor_image );

	Mat line_threshold;
	inRange(xor_image,Scalar(WHITE_MIN_THR,WHITE_MIN_THR,WHITE_MIN_THR)
			,Scalar(WHITE_MAX_THR,WHITE_MAX_THR,WHITE_MAX_THR),line_threshold);
	/*namedWindow( "line_threshold", CV_WINDOW_NORMAL);
	imshow( "line_threshold", line_threshold );*/

	//remove frame caused by IPM
	Mat frameRemoved;
	elementG = getStructuringElement( MORPH_CROSS,Size(12,12),Point(6,6) );
	erode(line_threshold,frameRemoved,elementG);
	elementG = getStructuringElement( MORPH_ELLIPSE,Size(15,15),Point(7,7) );
	dilate(frameRemoved, frameRemoved, elementG);

	Dst = frameRemoved;//for circle find green threshold is better
}

IPM invPrespctive(Mat &inputImg,Mat &outputImg,int lowBase , int topHeight)
{
	int width=inputImg.cols;
	int height=inputImg.rows;

	// The 4-points at the input image
	vector<Point2f> origPoints;
	origPoints.push_back( Point2f(0, height) );
	origPoints.push_back( Point2f(width, height) );
	origPoints.push_back( Point2f(width, height - topHeight) );// rows 2448
	origPoints.push_back( Point2f(0, height - topHeight) );//cols 3264

	// The 4-points correspondences in the destination image
	vector<Point2f> dstPoints;
	dstPoints.push_back( Point2f(width/2-lowBase, height) ); //usually (0,height)
	dstPoints.push_back( Point2f(width/2+lowBase, height) ); //usually (width,height)
	dstPoints.push_back( Point2f(width, 0) );
	dstPoints.push_back( Point2f(0, 0) );

	// IPM object
	IPM ipm( Size(width, height), Size(width, height), origPoints, dstPoints );

	// Process
	//clock_t begin = clock();
	ipm.applyHomography( inputImg, outputImg );
	//clock_t end = clock();
	//double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//printf("IPM: %.2f (ms)\r", 1000*elapsed_secs);
	ipm.drawPoints(origPoints, inputImg );

	return ipm;
}

void findCenterCircle(const Mat& inputImg, vector<Vec3f> circles)
{
	  // Apply the Hough Transform to find the circles
	  HoughCircles( inputImg, circles, CV_HOUGH_GRADIENT, 2, inputImg.rows, 250, 100, 100 ,0 );   //distance orange_dest.rows means
	                                                                                                // ;
}

void printCircles(const Mat& inputImg ,vector<Vec3f> circles)
{
	  Mat outputImg = inputImg;

	  // Draw the circles detected
	  	for( unsigned int i = 0; i < circles.size(); i++ )
	  	{
	  		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	  	    int radius = cvRound(circles[i][2]);
	  	    // circle center
	  	    circle( outputImg, center, 3, Scalar(0,255,0), -1, 8, 0 );
	  	    // circle outline
	  	    circle( outputImg, center, radius, Scalar(0,255,0), 3, 8, 0 );
	  	}

	  //displaying result
	namedWindow( "findCenterCircle", CV_WINDOW_NORMAL );
	imshow( "findCenterCircle", outputImg );
}

vector<Vec4i> findLines(const Mat& inputImg)
{
	 vector<Vec4i> lines;
	 HoughLinesP( inputImg, lines, 1, 2*CV_PI/180, 100, 100, 50 );
	 return lines;
}

void printLines(const Mat& inputImg,vector<Vec4i> lines)
{
	Mat outputImg = inputImg;
	cvtColor(outputImg,outputImg,CV_HLS2BGR);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( outputImg, cv::Point(lines[i][0], lines[i][1]),
              cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 1);
    }

	namedWindow( "findLines", CV_WINDOW_NORMAL );
	imshow( "findLines", outputImg );
}

void dftPrc(const Mat& I)
{
	Mat InputImg = I;
	cvtColor(InputImg,InputImg,CV_BGR2GRAY);
	    //if( I.empty())
	    //   return -1;

	    Mat padded;                            //expand input image to optimal size
	    int m = getOptimalDFTSize( InputImg.rows );
	    int n = getOptimalDFTSize( InputImg.cols ); // on the border add zero values
	    copyMakeBorder(InputImg, padded, 0, m - InputImg.rows, 0, n - InputImg.cols, BORDER_CONSTANT, Scalar::all(0));

	    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
	    Mat complexI;
	    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	    dft(complexI, complexI);            // this way the result may fit in the source matrix

	    // compute the magnitude and switch to logarithmic scale
	    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	    Mat magI = planes[0];

	    magI += Scalar::all(1);                    // switch to logarithmic scale
	    log(magI, magI);

	    // crop the spectrum, if it has an odd number of rows or columns
	    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	    // rearrange the quadrants of Fourier image  so that the origin is at the image center
	    int cx = magI.cols/2;
	    int cy = magI.rows/2;

	    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	    q0.copyTo(tmp);
	    q3.copyTo(q0);
	    tmp.copyTo(q3);

	    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	    q2.copyTo(q1);
	    tmp.copyTo(q2);

	    imwrite("DFTspec.jpg",magI);
	    normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
	                                            // viewable image form (float between values 0 and 1).
		//calculating the idft

	        cv::Mat inverseTransform;
	        cv::dft(complexI, inverseTransform, cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);
	        normalize(inverseTransform, inverseTransform, 0, 1, CV_MINMAX);


	    namedWindow( "Input Image Grayscale" , CV_WINDOW_NORMAL );
	    namedWindow( "spectrum magnitude" , CV_WINDOW_NORMAL );
	    imshow("Input Image Grayscale" , InputImg   );    // Show the result
	    imshow("spectrum magnitude", magI);
	    namedWindow( "Reconstructed" , CV_WINDOW_NORMAL );
        imshow("Reconstructed", inverseTransform);

}

void dftPrc2(const Mat& Src, Mat& Dst,int ch_index, int radius)
{
    // src image
    Mat img;
    // Magnitude
    Mat Mag;
    // Phase
    Mat Phase;
    // Image loading (grayscale)
    img= Src;
    //split to channels
    vector<Mat> channels;
    split(img,channels);
    //----------------------------------------------------------
    // FFT - output Mag and Phase
    //----------------------------------------------------------
    ForwardFFT_Mag_Phase(channels[ch_index],Mag,Phase);
    //namedWindow( "Input Image Lightness" , CV_WINDOW_NORMAL );
    //imshow("Input Image Lightness" , channels[ch_index]   );    // Show the result
    //imwrite("f_channel.jpg",channels[ch_index]);
    //----------------------------------------------------------
    // Filter
    // fft of Hue channel has in general low frequencys only. therefore  filtering LPF dont give allot
    // fft on Lightness gives Mag with notch. therefore LPF or BPF will smooth the grass pattern.
    //----------------------------------------------------------
    LPF(Mag,Mag,radius);//r=120
    //LPF(Phase,Phase,radius);//r=120
    //medianBlur(f_channel,f_channel,15);
    //medianBlur(channels[2],channels[2],9);
    //----------------------------------------------------------
    // Inverse transform
    //----------------------------------------------------------
    InverseFFT_Mag_Phase(Mag,Phase,channels[ch_index]);
    //merge image back
    channels[ch_index].convertTo(channels[ch_index],CV_8UC1);
    merge(channels,Dst);

    Mat LogMag;
    Mag += Scalar::all(1);                    // switch to logarithmic scale
    log(Mag, LogMag);
   //normalize(LogMag, LogMag, 0, 1, CV_MINMAX);
    //---------------------------------------------------
    //namedWindow( "Magnitude Log" , CV_WINDOW_NORMAL );
    //imshow("Magnitude Log", LogMag);
   // namedWindow( "Phase" , CV_WINDOW_NORMAL );
   // imshow("Phase", Phase);
   // namedWindow( "Reconstructed" , CV_WINDOW_NORMAL );
    //imshow("Reconstructed", channels[ch_index]);
   // imwrite("f_channel_rec.jpg",channels[ch_index]);
    //namedWindow( "Output Image" , CV_WINDOW_NORMAL );
  //  Mat temp;
    //cvtColor(Dst,temp,CV_HLS2BGR);
   // imshow("Output Image", temp);
   // imwrite("Output Image.jpg",temp);
}

void HSV_Threshold(const Mat & Src,Mat &Dst)
{
	Mat channel[3];
	split(Src,channel);
	int h,s,v;
	h=channel[0].at<uchar>(Src.cols/2,Src.rows/2);
	s=channel[1].at<uchar>(Src.cols/2,Src.rows/2);
	v=channel[2].at<uchar>(Src.cols/2,Src.rows/2);
	printf("h=%d s=%d v=%d col=%d row=%d\n",h,s,v,Src.cols,Src.rows);
	int hh,sh,vh,hl,sl,vl;
	hh=h+50; sh=s+50; vh=v+50;
	hl=h-50; sl=s-50; vl=v-50;
    inRange(Src,Scalar(hl,sl,vl),Scalar(hh,sh,vh),Dst);//for removing all gray colors and remaining the white color only
    namedWindow( "HSV_Threshold" , CV_WINDOW_NORMAL );
    imshow("HSV_Threshold", Dst);
    imwrite("HSV_Threshold.jpg",Dst);
}


void HLS_Threshold(const Mat & Src,Mat &Dst ,int Hmin,int Lmin,int Smin,int Hmax, int Lmax,int Smax)
{
	Mat channel[3];
	split(Src,channel);
	int h,l,s;
	h=channel[0].at<uchar>(Src.cols/2,Src.rows/2 - 200);
	l=channel[1].at<uchar>(Src.cols/2,Src.rows/2 - 200);
	s=channel[2].at<uchar>(Src.cols/2,Src.rows/2 - 200);
	printf("h=%d l=%d s=%d col=%d row=%d\n",h,l,s,Src.cols,Src.rows);
	int hh,lh,sh,hl,ll,sl;
	//hh=h+H; lh=l+L; sh=s+S;
	//hl=h-H; ll=l-L; sl=s-S;
    inRange(Src,Scalar(Hmin,Lmin,Smin),Scalar(Hmax,Lmax,Smax),Dst);
    /*namedWindow( "HSV_Threshold" , CV_WINDOW_NORMAL );
    imshow("HLS_Threshold", Dst);
    imwrite("HLS_Threshold.jpg",Dst);*/
}

/*
 * Function returns the height of the highest 1 chank with size 'size' at 'Src'
 */
int highest1(const Mat & Src,int size)
{
	Mat elementG = getStructuringElement( MORPH_RECT,Size(size,size),Point((int)size/2,(int)size/2) );
	Mat chanks;
	erode(Src,chanks,elementG);
	bool first = 0;
	int row = 0 ,col = 0;
	while ((!first)&&(row < chanks.rows)){
		col = 0;
		while ((!first)&&(col < chanks.cols)){
			if (255==chanks.at<int>(row,col)){
				first=1;
			}
			col=col + 1;
		}
		row = row + 1;
	}
	printf("first chank at %dth row\n",row);
    //namedWindow( "chank" , CV_WINDOW_NORMAL );
    //imshow("chank", chanks);
	if (first)
		return row;
	else
		return -1;
}
