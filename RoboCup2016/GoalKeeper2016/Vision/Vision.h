#include "GateDetector.h"
#include "BallDetector.h"
#include "SharedMemory.h"

#pragma once

class Vision
{
public:
	// Singletone
	static Vision* GetInstance();

	void RunVisionThread();

	DetectedObject* SafeGetDetectedBall();
	DetectedObject* SafeGetDetectedGate();

	GateDetector m_gateDetector;
	BallDetector m_ballDetector;

	SharedMemory m_gateSharedMemory;
	SharedMemory m_ballSharedMemory;

	void ReadCalibrationFromFile();
	void OpenCamera(VideoCapture& videoCapture);

private:
	static Vision* m_instance;
	Vision();
	~Vision();
};

