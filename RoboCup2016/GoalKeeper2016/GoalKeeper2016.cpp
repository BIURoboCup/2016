#include "Vision\Vision.h"

int main(int argc, char* argv[])
{
	const char* filename = "C:\\Users\\arabinow\\git\\RoboCup2016\\Media\\Photos\\White Lines\\onepost.png";

	Mat inputImage = imread(filename, CV_LOAD_IMAGE_COLOR);
	if (inputImage.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}

	DetectedObject* detectedGate;
	detectedGate = FindGate(inputImage);
	detectedGate->Draw(inputImage);
	imshow("gate", inputImage);
	waitKey(0);
	return 0;
}

