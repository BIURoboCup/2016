#include "GateDetector.h"
#include "BallDetector.h"

#pragma once

void* VisionActionAsync(void*);

class Vision
{
public:
	Vision();
	~Vision();

	void RunVisionThread();


	DetectedObject* SafeGetDetectedBall();
	DetectedObject* SafeGetDetectedGate();

	DetectedObject* m_detectedGate;
	DetectedObject* m_detectedBall;

	sem_t m_ballSemaphore;
	sem_t m_gateSemaphore;

	GateDetector m_gateDetector;
	BallDetector m_ballDetector;

	void ReadCalibrationFromFile();
	void OpenCamera(VideoCapture& videoCapture);
};

