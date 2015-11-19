/*
 *	Camera.h
 *
 *  Created on: 6 Oct 2011
 *  Author: Guy
 *
 *  Camera class controls every I/O interaction with the camera
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#define	FRAME_WIDTH		320
#define	FRAME_HEIGHT	240

#include "opencv2/opencv.hpp"
#include "LinuxCamera.h"
#include "Camera.h"
#include "IncludesFile.h"

using namespace Robot;
using namespace std;

class MyCamera {

private:

	static MyCamera* m_uniqueInstance; // the camera's instance

	LinuxCamera* m_linuxCamera;
	CameraSettings m_myCameraSettings;

	IplImage* m_originalImage; // the original image we get from the camera
	CvSize m_originalImageProperties; // the original image size

	CvCapture* m_cameraCapture;
	IplImage* m_largeImage; // the original image we get from the camera

	int m_cameraFPS;

	// Constructor
	MyCamera();

public:

	static MyCamera* GetInstance(); // Get instance of the camera

	// Camera attributes
	void GetFrameUsingV4L2(IplImage* returnedFrame); // queries a frame from camera using camera
	void GetFrameUsingOpenCV(IplImage* returnedFrame);

	CvSize GetImageProperties();
	int GetFPSProperty();
	void PrintCameraSettings();

	bool LoadCameraSettingsFromFile();
	bool SaveCameraSettingsToFile();
	void ChangeCameraBrightness(int offset);
	void ChangeCameraContrast(int offset);
	void ChangeCameraSaturation(int offset);
	void ChangeCameraGain(int offset);
	void ChangeCameraExposure(int offset);

	void LoadIniSettings(minIni* iniFile);

	// Destructor
	~MyCamera();
};

#endif /* CAMERA_H_ */

