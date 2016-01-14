#include "../GoalKeeper2016/Includes.h"
#include "../GoalKeeper2016/Constants.h"

#define ImageWindow "Image"
#define CalibrationWindow "Calibration"
#define TrackbarGreenOrWhite "Green or White"

static const char* calibrationFilePath = "/home/robot/workspace2/RoboCup2016/RoboCup2016/GoalKeeper2016/calibration.txt";

static Scalar minWhiteHSV = Scalar(0, 0, 170);
static Scalar maxWhiteHSV = Scalar(255, 50, 255);
static Scalar minGreenBGR = Scalar(0, 70, 0);
static Scalar maxGreenBGR = Scalar(100, 255, 100);

void onMouse(int event, int x, int y, int flag, void* param);

void CalibrateGreen(Mat image, Point2i position);
void CalibrateWhite(Mat image, Point2i position);
void ResetValuesToClick(Mat image, Point2i position);

// Returns the min\max scalar in the area of 3X3 around the given point in the given image.
Scalar MinAreaScalar(Mat image, Point2i point);
Scalar MaxAreaScalar(Mat image, Point2i point);
bool IsInImageBorders(int x, int y);

void ShowCalibration(Mat image);
bool IsGreenCalibration();

void WriteCalibrationToFile();
void ReadCalibrationFromFile();

Scalar Min(Scalar a, Scalar b);
Scalar Max(Scalar a, Scalar b);


class Calibrator
{
public:
	Calibrator();
	~Calibrator();
	
	void Calibrate();

private:	
	VideoCapture m_VideoCapture;	

	void OpenCamera();
	void InitializeWindow();
};
