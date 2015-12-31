#include "findGate.h"

#pragma once

class Vision
{
public:
	Vision();
	~Vision();

	DetectedObject* FindBall(Mat& image);
	DetectedObject* FindGate(Mat& image);

private:
	void ReadCalibration(const char* calibrationFilePath);
	void ParseLineFromCalibrationFile(string line);
};

