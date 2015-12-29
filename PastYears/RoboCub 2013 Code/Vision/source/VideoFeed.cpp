/*
 * File:   VideoFeed.cpp
 * Author: Guy Cohen
 *
 * Created on November 15, 2011
 */

#include "VideoFeed.h"
bool VideoFeed::m_printPixel = false;

/*
 * Description: Destructor.
 * Destroys the window.
 * Parameters: None.
 * Return Value: None.
 */
VideoFeed::~VideoFeed() {
	cout << "Video Feed D'tor\n";
	cvDestroyWindow(m_windowName);
}

/*
 * Description: The function updates the streaming video.
 * Parameters: IplImage* new_frame - the new frame we are going to display.
 * Return Value: None.
 */
void VideoFeed::UpdateFrame() {
	cvShowImage(m_windowName, m_Frame);
}

// This is our mouse callback. If the user
// presses the left button, we start a box.
// when the user releases that button, then we
// add the box to the current image. When the
// mouse is dragged (with the button down) we
// resize the box.
//
void Mouse_Pos_Print_Pixel(int event, int x, int y, int flags, void* param) {
	IplImage* image = (IplImage*) param;
	int hue, sat, val;

	switch (event) {

	case CV_EVENT_MOUSEMOVE:

		if (VideoFeed::m_printPixel) {

			uchar *pHSV = (uchar*) (image->imageData + y * (image->widthStep)
					+ x * 3);
			hue = *(uchar*) (pHSV + 0); // H component
			sat = *(uchar*) (pHSV + 1); // S component
			val = *(uchar*) (pHSV + 2); // V component

			//		printf("hue:  %u sat: %u val: %u \n",hue,sat,val);
			cout << "hue: " << hue << " sat: " << sat << " val: " << val
					<< "\n";
		}
		break;

	case CV_EVENT_LBUTTONDOWN:

		VideoFeed::m_printPixel = !VideoFeed::m_printPixel;
		break;
	}

}
/*
 * Description: Constructor.
 * It creates a new window to display the new frame.
 * Parameters: int windowNumber - the window number in the camera's streaming list
 * Return Value: None.
 */
VideoFeed::VideoFeed(int windowNumber, IplImage* Current_Frame,
		const char* windowName) {
	cout << "Video Feed Constructor" << "\n";
	//char chWindownumber[2] = {windowNumber + '0', '\0'};	// convert int to char

	//strcpy(m_windowName, "Debug ");							// window name setup
	//strcat(m_windowName, chWindownumber);					// Concatenate window number to its name

	strcpy(m_windowName, windowName); // window name setup

	cvNamedWindow(m_windowName, CV_WINDOW_AUTOSIZE); // create the new window

	m_printPixel = false;
	cvSetMouseCallback(m_windowName, Mouse_Pos_Print_Pixel,
			(void*) Current_Frame);

	m_Frame = Current_Frame;

	int windowX = 10 + 324 * (windowNumber % 3);
	int windowY = 20 + 280 * (windowNumber / 3);
	cvMoveWindow(m_windowName, windowX, windowY);

}
