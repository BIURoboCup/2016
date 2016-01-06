// RoboCup2016.cpp : Defines the entry point for the console application.
//

#include "Motion.h"
#include "stdafx.h"
#include "findGate.h"

using namespace std;
using namespace cv;


Scalar GreenCalibrationMinHSV;
Scalar GreenCalibrationMaxHSV;

//Scalar BGRToHSVTransform(const Scalar bgrScalar)
//{
//	Scalar hsvScalar = cvtColor(bgrScalar, COLOR_BGR2HSV);
//}

Mat InRange(Mat image, Scalar lowerBound)
{
	Mat output;
	Scalar white(255, 255, 255);
	inRange(image, lowerBound, white, output);
	return output;
}

Mat GetStructuringElement(int structuringElementShape, int size)
{
	return getStructuringElement(structuringElementShape,
		Size(2 * size + 1, 2 * size + 1),
		Point(-1, -1));
}

/// <summary> Erose a given binary image </summary>
/// <param name="structuringElementShape"> Shape of erosion. e.g. "MORPH_ERODE" </param>
/// <param name="size"> Size of erosion </param>
/// <returns> Erosed image </returns>
Mat EroseBinaryImage(Mat inputImage, int structuringElementShape, int size)
{
	Mat element = GetStructuringElement(structuringElementShape, size);

	Mat erosedImage;
	erode(inputImage, erosedImage, element);

	return erosedImage;
}

/// <summary> Dilate a given binary image </summary>
/// <param name="structuringElementShape"> Shape of dilation. e.g. "MORPH_ERODE" </param>
/// <param name="size"> Size of dilation </param>
/// <returns> Dilated image </returns>
Mat DilateBinaryImage(Mat inputImage, int structuringElementShape, int size)
{
	Mat element = GetStructuringElement(structuringElementShape, size);

	Mat dilatedImage;
	dilate(inputImage, dilatedImage, element);

	return dilatedImage;
}

/// <summary> 
/// Gets a binary image and fills the black holes inside it.
/// </summary>
Mat FillHolesInBinrayImage(Mat binaryImage)
{
	Mat dilatedImage = DilateBinaryImage(binaryImage, MORPH_ELLIPSE, 2);
	Mat erosedImage = EroseBinaryImage(binaryImage, MORPH_ELLIPSE, 1);
/*	imshow("source", binaryImage);
	imshow("dilated", dilatedImage);
	imshow("erosed", erosedImage)*/;
	Mat resultImage = EroseBinaryImage(dilatedImage, MORPH_ELLIPSE, 1);
	return resultImage;
}

/// <summary> 
/// Gets a BGR Mat and reutrns a B&W Mat. White represent the field part (Including the side strips).
/// </summary>
//Mat GetFieldPart(Mat bgrImage)
//{	
//	Mat onlyGreenImage;
//	inRange(bgrImage, minGreenBGR, maxGreenBGR, onlyGreenImage);
//	/*blur(onlyGreenImage, onlyGreenImage, Size(20, 10));
//	imshow("green", onlyGreenImage);*/
//
//	Mat hsvImage;
//	Mat onlyWhiteImage;
//	cvtColor(bgrImage, hsvImage, CV_BGR2HSV);
//	inRange(hsvImage, minWhiteHSV, maxWhiteHSV, onlyWhiteImage);
//	//imshow("white", onlyWhiteImage);	
//
//	Mat greenAndWhiteImage;
//	bitwise_or(onlyGreenImage, onlyWhiteImage, greenAndWhiteImage);
//
//	Mat field = FillHolesInBinrayImage(greenAndWhiteImage);
//	return field;
//}

void help()
{
	cout << "\nThis program demonstrates line finding with the Hough transform.\n"
		"Usage:\n"
		"./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main()
{
	const char* filename = "C:\\Users\\arabinow\\git\\RoboCup2016\\Media\\Photos\\White Lines\\onepost.png";

	Mat inputImage = imread(filename, CV_LOAD_IMAGE_COLOR);
	if (inputImage.empty())
	{
		help();
		cout << "can not open " << filename << endl;
		return -1;
	}

	FindGate(inputImage);



	//GreenCalibrationMinHSV = Scalar(41, 13, 42);
	//GreenCalibrationMaxHSV = Scalar(118, 224, 188);
	//Mat field = GetFieldPart(inputImage);
	///*imshow("field", field);
	//waitKey(0);*/

	//Mat inverseField;
	//bitwise_not(field, inverseField);
	//Mat inverseFieldCopy = inverseField.clone();
	//vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;
	//findContours(inverseFieldCopy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	////drawContours(field, contours, -1, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point());
	//imshow("contours", inverseFieldCopy);
	//imshow("field", inverseField);
	//waitKey(0);

/*
	Mat dst, cdst, white, srcHsv, srcGray;
	Scalar MIN_HSV(41, 13, 42);
	Scalar MAX_HSV(118, 224, 188);
	cvtColor(src, srcHsv, CV_BGR2HSV);
	inRange(srcHsv, MIN_HSV, MAX_HSV, srcHsv);
	imshow("grass", srcHsv);
	imshow("source", src);
	waitKey(0);
	cvtColor(src, srcGray, CV_BGR2GRAY);
	inRange(src, Scalar(0, 0, 0), Scalar(50, 255, 50), srcGray);
	srcGray = InRange(srcGray, Scalar(200, 200, 200));
	Canny(srcGray, dst, 50, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);
	imshow("canny", cdst);
	waitKey(0);
	*///vector<Vec4i> lines;
	//HoughLinesP(dst, lines, 1, CV_PI / 180, 100, 50, 20);
	//for (size_t i = 0; i < lines.size(); i++)
	//{
	//	Vec4i l = lines[i];
	//	line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	//}
	//imshow("source", srcGray);
	//imshow("detected lines", cdst);

	return 0;
}