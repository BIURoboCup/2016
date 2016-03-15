#include "Vision.h"


struct VisionException : public exception
{
   string message;
   VisionException(string exceptionMessage) : message(exceptionMessage) {}
   ~VisionException() throw () {}

	const char* what() const throw () {
		return message.c_str();
	}
};

void SigTermHandler(int signal)
{
	cout << "Waiting until next frame and killing" << endl;
	Vision::GetInstance()->TerminateVisionThread();
}

void* VisionActionAsync(void*);

Vision::Vision():
m_gateSharedMemory(GateDetector::GetDefault()), m_ballSharedMemory(BallDetector::GetDefault())
{
	ReadCalibrationFromFile();
}

Vision::~Vision()
{
	destroyAllWindows();
	Vision::m_instance = NULL;
	CloseFlyCapCamera();
}

Vision* Vision::m_instance = NULL;
Vision* Vision::GetInstance()
{
    if(m_instance == NULL)
    {
    	m_instance = new Vision();
        return m_instance;
    }
    else
    {
        return m_instance;
    }
}

void ParseLineFromCalibrationFile(string line)
{
	string key = line.substr(0, 12);
	size_t keyStart = line.find('[');
	size_t keyEnd = line.find(']');
	int value = atoi(line.substr(keyStart + 1, keyEnd - keyStart).c_str());

	if (key == "White: H_min") minWhiteHSV[0] = value;
	if (key == "White: S_min") minWhiteHSV[1] = value;
	if (key == "White: V_min") minWhiteHSV[2] = value;

	if (key == "White: H_max") maxWhiteHSV[0] = value;
	if (key == "White: S_max") maxWhiteHSV[1] = value;
	if (key == "White: V_max") maxWhiteHSV[2] = value;

	if (key == "Green: B_min") minGreenBGR[0] = value;
	if (key == "Green: G_min") minGreenBGR[1] = value;
	if (key == "Green: R_min") minGreenBGR[2] = value;

	if (key == "Green: B_max") maxGreenBGR[0] = value;
	if (key == "Green: G_max") maxGreenBGR[1] = value;
	if (key == "Green: R_max") maxGreenBGR[2] = value;
}

void Vision::ReadCalibrationFromFile()
{
	static const char* calibrationFilePath = "/home/robot/workspace2/RoboCup2016/RoboCup2016/GoalKeeper2016/calibration.txt";
	string line;
	ifstream myfile(calibrationFilePath);

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			ParseLineFromCalibrationFile(line);
		}
		myfile.close();
	}

	else
	{
		cout << "Unable to open color config file";
	}
}

void RunVision()
{
//	pthread_t visionThread;
//
//	if(1 == pthread_create(&visionThread, NULL, VisionActionAsync, NULL))
//	{
//		fprintf(stderr, "Couldn't create Vision thread\n");
//		exit(1);
//	}
//}
//
//void* VisionActionAsync(void*)
//{
	Vision::GetInstance()->OpenFlyCapCamera();

	signal(SIGTERM, SigTermHandler);

	Vision::GetInstance()->IsVisionThreadRunning = true;

	const string outputFile =
			"/home/robot/workspace2/RoboCup2016/RoboCup2016/GoalKeeper2016/demo.avi";

	VideoWriter outputVideo;
	outputVideo.open(outputFile, CV_FOURCC('M', 'J', 'P', 'G'), 10,
			Size(FRAME_WIDTH, FRAME_HEIGHT), true);

	// capture loop
	char key = 0;
	while (key != 'q' && Vision::GetInstance()->IsVisionThreadRunning == true)
	{
		Mat currentFrame;
		Vision::GetInstance()->GetFrameFromFlyCap(currentFrame);
		Vision::GetInstance()->ProcessCurrentFrame(currentFrame);

		imshow("Outout", currentFrame);
		key = waitKey(30);
		outputVideo.write(currentFrame);
	}

	Vision::GetInstance()->CloseFlyCapCamera();
}

void Vision::ProcessCurrentFrame(Mat& currentFrame)
{
	DetectedObject* detectedGate = m_gateDetector.FindGate(
			currentFrame);
	detectedGate->Draw(currentFrame);
	DetectedObject* detectedBall = m_ballDetector.FindBall(
			currentFrame);
	detectedBall->Draw(currentFrame);
}

void Vision::TerminateVisionThread()
{
	IsVisionThreadRunning = false;
}

void Vision::GetFrameFromFlyCap(Mat& currentFrame)
 {
	// Get the image
	FlyCapture2::Image rawImage;
	FlyCapture2::Error error = flyCapCamera.RetrieveBuffer(&rawImage);
	if (error != FlyCapture2::PGRERROR_OK) {
		throw VisionException("capture error");
	}

	IplImage* image = ConvertImageToOpenCV(&rawImage);

	currentFrame = cv::cvarrToMat(image);

//	// convert to rgb
//	FlyCapture2::Image rgbImage;
//	rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);
//
//	// convert to OpenCV Mat
//	unsigned int rowBytes = (double) rgbImage.GetReceivedDataSize()
//			/ (double) rgbImage.GetRows();
//	currentFrame = Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3,
//			rgbImage.GetData(), rowBytes);
}

void Vision::OpenFlyCapCamera()
{
	FlyCapture2::Error error;
	FlyCapture2::CameraInfo camInfo;

	// Connect the camera
	error = flyCapCamera.Connect(0);
	if (error != FlyCapture2::PGRERROR_OK) {
		throw VisionException("Failed to connect to camera");
	}

	// Get the camera info and print it out
	error = flyCapCamera.GetCameraInfo(&camInfo);
	if (error != FlyCapture2::PGRERROR_OK) {
		throw VisionException("Failed to get camera info from camera");
	}
	cout << camInfo.vendorName << " " << camInfo.modelName << " "
			<< camInfo.serialNumber << endl;

	error = flyCapCamera.StartCapture();
	if (error == FlyCapture2::PGRERROR_ISOCH_BANDWIDTH_EXCEEDED) {
		throw VisionException("Bandwidth exceeded");
	} else if (error != FlyCapture2::PGRERROR_OK) {
		throw VisionException("Failed to start image capture");
	}
}

void Vision::CloseFlyCapCamera()
{
	FlyCapture2::Error error;
	error = flyCapCamera.StopCapture();
	if (error != FlyCapture2::PGRERROR_OK) {
		// This may fail when the camera was removed, so don't show
		// an error message
	}

	flyCapCamera.Disconnect();
}

