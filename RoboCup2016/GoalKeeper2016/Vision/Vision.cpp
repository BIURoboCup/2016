#include "Vision.h"

// Boolean which is true while the program wasn't shut down.
// SIGTERM catching set it to false, causing the program to exit.
bool RunVisionThread = true;

void SigTermHandler(int signal)
{
	cout << "Waiting until next frame and killing" << endl;
	RunVisionThread = false;
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

void* VisionActionAsync(void*)
{
	signal(SIGTERM, SigTermHandler);

	Vision* vision = Vision::GetInstance();

	const string outputFile = "/home/robot/workspace2/RoboCup2016/RoboCup2016/GoalKeeper2016/demo.avi";

	VideoCapture videoCapture;
	VideoWriter outputVideo;

	vision->OpenCamera(videoCapture);
	outputVideo.open(outputFile, CV_FOURCC('M','J','P','G'), 10, Size(FRAME_WIDTH,FRAME_HEIGHT), true);

	while (RunVisionThread)
	{
		Mat currentFrame;
		videoCapture >> currentFrame;

		// Close the Frame & save settings by clicking 'q'.
		char exitKey = waitKey(30);
		if (exitKey == 'q')
		{
			break;
		}

		DetectedObject* detectedGate = vision->m_gateDetector.FindGate(currentFrame);
		detectedGate->Draw(currentFrame);
		DetectedObject* detectedBall = vision->m_ballDetector.FindBall(currentFrame);
		detectedBall->Draw(currentFrame);

		imshow("Output", currentFrame);

		outputVideo.write(currentFrame);
	}

	videoCapture.release();
	return NULL;
}

void Vision::RunVisionThread()
{
	pthread_t visionThread;

	if(1 == pthread_create(&visionThread, NULL, VisionActionAsync, NULL))
	{
		fprintf(stderr, "Couldn't create Vision thread\n");
		exit(1);
	}
}

void Vision::OpenCamera(VideoCapture& videoCapture)
{
	int tries = 0;

	videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);


	while (!videoCapture.open(0))
	{
		printf("trying to open camera\n");
		if (++tries > 10)
		{
			printf("Couldn't open camera capture, gave up\n");
			throw new string("Couldn't open camera");
		}
		printf("couldn't open camera capture, try #%d\n", tries);
	}
}

