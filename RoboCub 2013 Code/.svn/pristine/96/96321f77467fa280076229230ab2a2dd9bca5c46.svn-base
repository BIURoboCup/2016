/*
 * File:   Video_Feed.h
 * Author: Guy Cohen
 *
 * Created on November 15, 2011
 *
 * This class builds a new window to show frames
 * and updates the frame in the window every time UpdateFrame() is called
 */

#ifndef VIDEOFEED_H_
#define	VIDEOFEED_H_

#include "opencv2/opencv.hpp"
#include "IncludesFile.h"

class VideoFeed {

private:

	char m_windowName[15];
	IplImage* m_Frame;
	static bool m_printPixel;

	friend void Mouse_Pos_Print_Pixel(int event, int x, int y, int flags,
			void* param);

public:

	VideoFeed* m_next;

	// C'tor
	VideoFeed(int windowNumber, IplImage* Current_Frame,
			const char* windowName);

	void UpdateFrame(); // update streaming video with new frame

	// D'tor
	~VideoFeed();
};

#endif	/* VIDEOFEED_H_ */
