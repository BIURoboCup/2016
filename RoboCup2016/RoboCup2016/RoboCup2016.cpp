// RoboCup2016.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;
using namespace cv;

int main(){

	Mat im = imread("C:\\ProgramData\\ITE\\Test Content\\Build_Environment\\Source\\DllTool\\DllTool\\Photos\\CIMG6921.JPG");
	if (im.empty()){

		cout << "Cannot load image!" << endl;
		return -1;

	}

	imshow("Image", im);
	waitKey(0);

}



