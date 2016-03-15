#include "GateDetector.h"
#include "BallDetector.h"
#include "SharedMemory.h"

#pragma once

void RunVision();

class Vision
{
public:
	// Singletone
	static Vision* GetInstance();

	DetectedObject* SafeGetDetectedBall();
	DetectedObject* SafeGetDetectedGate();

	void TerminateVisionThread();

	~Vision();

	GateDetector m_gateDetector;
	BallDetector m_ballDetector;

	SharedMemory m_gateSharedMemory;
	SharedMemory m_ballSharedMemory;

	bool IsVisionThreadRunning;

	void OpenFlyCapCamera();
	void CloseFlyCapCamera();
	void GetFrameFromFlyCap(Mat& frame);
	void ProcessCurrentFrame(Mat& frame);

	FlyCapture2::Camera flyCapCamera;

private:
	void ReadCalibrationFromFile();

	static Vision* m_instance;
	Vision();
};

