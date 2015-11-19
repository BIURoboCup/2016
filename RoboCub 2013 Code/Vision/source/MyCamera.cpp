/*
 * 	Camera.cpp
 *
 *  Created on: 6 October 2011
 *      Author: Guy
 */

#include "MyCamera.h"
MyCamera* MyCamera::m_uniqueInstance = 0;

/*
 * Description: Private Constructor.
 * Appends the camera. initialize parameters of the camera.
 * Return Value: None.
 */

//#define DEBUG_PRINT_CAMERA_SETTINGS
MyCamera::MyCamera() {
#ifdef DEBUG_PRINT_CAMERA_SETTINGS
	cout << "DBG Camera c'tor" << endl;
#endif
	m_originalImageProperties.width = Camera::WIDTH;
	m_originalImageProperties.height = Camera::HEIGHT;

	m_originalImage = cvCreateImage(
			cvSize(m_originalImageProperties.width,
					m_originalImageProperties.height), IPL_DEPTH_8U, 3);

#ifdef USE_OPENCV
	m_largeImage = cvCreateImage(cvSize(Camera::WIDTH*2, Camera::HEIGHT*2),IPL_DEPTH_8U , 3);
#endif

#ifdef USE_V4L2
	m_linuxCamera = LinuxCamera::GetInstance();
	m_linuxCamera->Initialize(0);
	m_linuxCamera->SetCameraSettings(m_myCameraSettings);
#endif

	//m_linuxCamera->LoadINISettings(ini);
#ifdef USE_OPENCV
	m_cameraCapture = cvCreateCameraCapture(-1);
#endif
}

/*
 * Fucntion Name: GetInstance
 * Description: returns the instance of the camera
 * Parameters: None.
 * Return Value: The camera instance.
 */
MyCamera* MyCamera::GetInstance() {
	if (m_uniqueInstance == 0) {
		m_uniqueInstance = new MyCamera();
	}
	return m_uniqueInstance;
}

/*
 * Function Name: GetFrame
 * Description: The function grabs a new frame from the camera.
 * 				It also reduces the size of the frame by 4.
 * Return Value: IplImage* - the reduced image that we got from the camera.
 */
void MyCamera::GetFrameUsingV4L2(IplImage* returnedFrame) {
	//cout << "Get Frame From System Call" << endl;
	m_linuxCamera->CaptureFrame();

	//cout << "Copying Data..." << endl;
	memcpy(returnedFrame->imageData,
			m_linuxCamera->fbuffer->m_RGBFrame->m_ImageData,
			m_linuxCamera->fbuffer->m_RGBFrame->m_ImageSize);
	//cout << "Data copied" << endl;
	cvFlip(returnedFrame, NULL, -1); // flip both axis
}

void MyCamera::GetFrameUsingOpenCV(IplImage* returnedFrame) {
	//cout << "Query frame. camera image = "<< m_cameraCapture << endl;
	m_largeImage = cvQueryFrame(m_cameraCapture); // grab the frame from the camera

	//cout << "After query frame\n";
	//cout << "Large Image Size: " << "Width = " <<m_largeImage->width << " Height = " << m_largeImage->height << endl;
	//cout << "My Image Size: " << "Width = " <<m_originalImage->width << " Height = " << m_originalImage->height << endl;

	cvPyrDown(m_largeImage, m_originalImage); // reduce image size
	cvFlip(m_originalImage, NULL, -1/*flip both axies*/);

	cvCopy(m_originalImage, returnedFrame);
	returnedFrame = m_originalImage; // return the reduced image
}

/*
 * Function Name: GetOriginalImageProperties
 * Description: The function grabs a new frame from the camera.
 * 				It also reduces the size of the frame by 4.
 * Return Value: IplImage* - the reduced image that we got from the camera.
 */
CvSize MyCamera::GetImageProperties() {
	return m_originalImageProperties;
}

int MyCamera::GetFPSProperty() {
	return m_cameraFPS;
}

void MyCamera::LoadIniSettings(minIni* iniFile) {
	m_linuxCamera->LoadINISettings(iniFile);
}

void MyCamera::PrintCameraSettings() {
#ifdef DEBUG_PRINT_CAMERA_SETTINGS
	CameraSettings cameraSettings = m_linuxCamera->GetCameraSettings();
	printf ("Camera Settings:\n");
	printf ("Brightness = %d\n", cameraSettings.brightness);
	printf ("Contrast = %d\n", cameraSettings.contrast);
	printf ("Saturation = %d\n", cameraSettings.saturation);
	printf ("Gain = %d\n", cameraSettings.gain);
	printf ("Exposure = %d\n", cameraSettings.exposure);
#endif
}

void MyCamera::ChangeCameraBrightness(int offset) {
	CameraSettings cameraSettings = m_linuxCamera->GetCameraSettings();

	cameraSettings.brightness = (cameraSettings.brightness + offset) % 256;

	m_linuxCamera->SetCameraSettings(cameraSettings);
}

void MyCamera::ChangeCameraContrast(int offset) {
	CameraSettings cameraSettings = m_linuxCamera->GetCameraSettings();

	cameraSettings.contrast = (cameraSettings.contrast + offset) % 256;

	m_linuxCamera->SetCameraSettings(cameraSettings);
}

void MyCamera::ChangeCameraSaturation(int offset) {
	CameraSettings cameraSettings = m_linuxCamera->GetCameraSettings();

	cameraSettings.saturation = (cameraSettings.saturation + offset) % 256;

	m_linuxCamera->SetCameraSettings(cameraSettings);
}

void MyCamera::ChangeCameraGain(int offset) {
	CameraSettings cameraSettings = m_linuxCamera->GetCameraSettings();

	cameraSettings.gain = (cameraSettings.gain + offset) % 256;

	m_linuxCamera->SetCameraSettings(cameraSettings);
}

void MyCamera::ChangeCameraExposure(int offset) {
	CameraSettings cameraSettings = m_linuxCamera->GetCameraSettings();

	cameraSettings.exposure = (cameraSettings.exposure + offset) % 10001;

	m_linuxCamera->SetCameraSettings(cameraSettings);
}

/*
 * Description: Destructor
 * Return Value: None.
 */
MyCamera::~MyCamera() {
	cout << "Camera d'tor" << endl;
}
