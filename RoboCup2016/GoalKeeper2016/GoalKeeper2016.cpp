#include "Vision/Vision.h"

void Debug()
{
	cout << "A";
	int a;
	cin >> a;
}

int main(int argc, char* argv[])
{
	Vision* vision = Vision::GetInstance();

	vision->RunVisionThread();

	getchar();

	char key;
	cout << "Do you want to free all the engines? [y/n]" << endl;
	cin >> key ;
	if(key == 'y' || key == 'Y')
	{
		//TODO: Call FreeAllEngines
	}

	delete vision;

//	const char* filename = "C:\\Users\\arabinow\\git\\RoboCup2016\\Media\\Photos\\White Lines\\onepost.png";
//	Mat inputImage = imread(filename, CV_LOAD_IMAGE_COLOR);
//	if (inputImage.empty())
//	{
//		cout << "can not open " << filename << endl;
//		return -1;
//	}
//
//	DetectedObject* detectedGate;
//	detectedGate = FindGate(inputImage);
//	detectedGate->Draw(inputImage);
//	imshow("gate", inputImage);
//	waitKey(0);
	return 0;
}

