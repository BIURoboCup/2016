#include "Calibrator.h"

Mat currentFrame;

Calibrator::Calibrator()
{
	try
	{
		OpenCamera();
		InitializeWindow();
	}
	catch (Exception ex)
	{
		exit(1);
	}
}


Calibrator::~Calibrator()
{
}

void Calibrator::Calibrate()
{
	ReadCalibrationFromFile();

	while (true)
	{
		m_VideoCapture >> currentFrame;
		ShowCalibration(currentFrame);

		// Close the calibrator & save settings by clicking 'q'.
		// Close without saving by clicking 'z'
		char exitKey = waitKey(30);
		if (exitKey == 'q')
		{
			WriteCalibrationToFile();
			return;
		}
		if (exitKey == 'z')
		{
			return;
		}
	}	
}


void onMouse(int event, int mouseX, int mouseY, int flag, void* param)
{
	if (event != CV_EVENT_LBUTTONDOWN && event != CV_EVENT_RBUTTONDOWN)
	{
		return;
	}

	Point2i mousePosition(mouseX,mouseY);
	if (event == CV_EVENT_RBUTTONDOWN)
	{
		ResetValuesToClick(currentFrame, mousePosition);
	}
	
	else
	{
		if (IsGreenCalibration())
		{
			CalibrateGreen(currentFrame, mousePosition);
		}
		else
		{
			cvtColor(currentFrame, currentFrame, CV_BGR2HSV);
			CalibrateWhite(currentFrame, mousePosition);
		}
	}
}

void ResetValuesToClick(Mat image, Point2i position)
{
	if (IsGreenCalibration())
	{
		minGreenBGR = MinAreaScalar(image, position);
		maxGreenBGR = MaxAreaScalar(image, position);
	}
	else
	{
		cvtColor(currentFrame, currentFrame, CV_BGR2HSV);
		minWhiteHSV = MinAreaScalar(image, position);
		maxWhiteHSV = MaxAreaScalar(image, position);
	}
}

void ShowCalibration(Mat image)
{
	Mat threshold;

	if (IsGreenCalibration())
	{
		inRange(image, minGreenBGR, maxGreenBGR, threshold);
	}
	else
	{
		Mat hsvImage;
		cvtColor(image, hsvImage, CV_BGR2HSV);
		inRange(hsvImage, minWhiteHSV, maxWhiteHSV, threshold);
	}
	
	imshow(ImageWindow, currentFrame);
	imshow(CalibrationWindow, threshold);
}

bool IsGreenCalibration()
{
	return getTrackbarPos(TrackbarGreenOrWhite, ImageWindow) == 1;
}

void CalibrateGreen(Mat image, Point2i position)
{
	minGreenBGR = Min(MinAreaScalar(image, position), minGreenBGR);
	maxGreenBGR = Max(MaxAreaScalar(image, position), maxGreenBGR);
}

void CalibrateWhite(Mat image, Point2i position)
{
	minWhiteHSV = Min(MinAreaScalar(image, position), minWhiteHSV);
	maxWhiteHSV = Max(MaxAreaScalar(image, position), maxWhiteHSV);
}

Scalar Min(Scalar a, Scalar b)
{
	Scalar minimum;
	for (int i = 0; i < 3; i++)
	{
		minimum[i] = MIN(a[i], b[i]);
	}

	return minimum;
}

Scalar Max(Scalar a, Scalar b)
{
	Scalar minimum;
	for (int i = 0; i < 3; i++)
	{
		minimum[i] = MAX(a[i], b[i]);
	}

	return minimum;
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

void ReadCalibrationFromFile()
{
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

void WriteCalibrationToFile()
{
	ofstream out(calibrationFilePath);

	out << "White: H_min[" << minWhiteHSV[0] << "]\n";
	out << "White: S_min[" << minWhiteHSV[1] << "]\n";
	out << "White: V_min[" << minWhiteHSV[2] << "]\n";

	out << "\n";

	out << "White: H_max[" << maxWhiteHSV[0] << "]\n";
	out << "White: S_max[" << maxWhiteHSV[1] << "]\n";
	out << "White: V_max[" << maxWhiteHSV[2] << "]\n";

	out << "\n";

	out << "Green: B_min[" << minGreenBGR[0] << "]\n";
	out << "Green: G_min[" << minGreenBGR[1] << "]\n";
	out << "Green: R_min[" << minGreenBGR[2] << "]\n";
	
	out << "\n";

	out << "Green: B_max[" << maxGreenBGR[0] << "]\n";
	out << "Green: G_max[" << maxGreenBGR[1] << "]\n";
	out << "Green: R_max[" << maxGreenBGR[2] << "]\n";

	out.close();
}

bool IsInImageBorders(int x, int y)
{
	return 0 <= x && x < FRAME_WIDTH && 0 <= y && y < FRAME_HEIGHT;
}

Scalar MinAreaScalar(Mat image, Point2i point)
{
	Scalar minScalar(255, 255, 255);

	for (int i = point.x - 1; i <= point.x + 1; i++)
	{
		for (int j = point.y - 1; j <= point.y + 1; j++)
		{
			if (IsInImageBorders(i, j))
			{
				Vec3b currentPixel = image.at<Vec3b>(j, i);
				for (int k = 0; k < 3; k++)
				{
					minScalar[k] = MIN(minScalar[k], currentPixel[k]);
				}
			}
		}
	}

	return minScalar;
}

Scalar MaxAreaScalar(Mat image, Point2i point)
{
	Scalar maxScalar(0, 0, 0);

	for (int i = point.x - 1; i <= point.x + 1; i++)
	{
		for (int j = point.y - 1; j <= point.y + 1; j++)
		{
			if (IsInImageBorders(i, j))
			{
				Vec3b currentPixel = image.at<Vec3b>(j, i);
				for (int k = 0; k < 3; k++)
				{
					maxScalar[k] = MAX(maxScalar[k], currentPixel[k]);
				}
			}
		}
	}

	return maxScalar;
}

void Calibrator::InitializeWindow()
{
	namedWindow(ImageWindow, CV_WINDOW_AUTOSIZE);
	namedWindow(CalibrationWindow, CV_WINDOW_AUTOSIZE);
	createTrackbar(TrackbarGreenOrWhite, ImageWindow, 0, 1);
	setMouseCallback(ImageWindow, onMouse, 0);
}

void Calibrator::OpenCamera()
{
	//Init capture object
	int tries = 0;

	m_VideoCapture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	m_VideoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);


	while (!m_VideoCapture.open(0))
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