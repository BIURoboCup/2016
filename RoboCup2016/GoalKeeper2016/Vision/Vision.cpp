#include "Vision.h"

Vision::Vision()
{
	const char* calibrationFilePath = "";
	ReadCalibration(calibrationFilePath);
}

void Vision::ReadCalibration(const char* calibrationFilePath)
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

void Vision::ParseLineFromCalibrationFile(string line)
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

	