// Vision2016.cpp : Defines the entry point for the console application.
//

#include "Vision.h"
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

	DetectedGate detectedGate;
	FindGate(inputImage, detectedGate);
	DrawGateOnImage(inputImage, detectedGate);
	imshow("gate", inputImage);
	waitKey(0);

	Mat field;
	FindField(inputImage, field);
	waitKey(0);

	Mat onlyWhiteImage;
	GetWhiteImage(inputImage, onlyWhiteImage);

	Mat canny;
	Canny(onlyWhiteImage, canny, 50, 200, 3);

	vector<Vec4i> lines;
	HoughLinesP(canny, lines, 1, CV_PI / 180, 100, 50, 20);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(inputImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, CV_AA);
	}

	//imshow("lines", inputImage);
	waitKey(0);

	return 0;
}