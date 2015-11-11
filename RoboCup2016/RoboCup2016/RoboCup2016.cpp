// RoboCup2016.cpp : Defines the entry point for the console application.
//

#include "Motion.h"
#include "stdafx.h"

using namespace std;
using namespace cv;

int main(){
	string pathToAPhoto = "";
	Mat im = imread(pathToAPhoto);
	if (im.empty()){

		cout << "Cannot load image!" << endl;
		return -1;

	}

	imshow("Image", im);
	waitKey(0);
}



