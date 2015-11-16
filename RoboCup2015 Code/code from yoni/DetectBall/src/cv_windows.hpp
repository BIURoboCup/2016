/*
 * cv_windows.hpp
 *
 *  Created on: May 6, 2015
 *      Author: u1310pass123
 */

#ifndef CV_WINDOWS_HPP_
#define CV_WINDOWS_HPP_

#include <string.h>

class cvWin {
private:
	std::string myId;
public:
	/*cvWin(char id[]) {
		myId = id;
		cv::namedWindow(myId);
	}*/

	cvWin(std::string id) {
			myId = id;
			cv::namedWindow(myId);
	}

	int createTrackbar(const std::string& name,
	                              int* currentValue, int maxValue,
	                              cv::TrackbarCallback callback = 0,
	                              void* userdata = 0) {
	return cv::createTrackbar(name, myId,currentValue, maxValue, callback, userdata);
	}

	void imshow(cv::InputArray mat) {
		cv::imshow(myId,mat);
	}

	void setMouseCallback(cv::MouseCallback callBack, void* userdata=0) {
		cv::setMouseCallback(myId,callBack,userdata);
	}

	// For using in other methods of opencv:
	operator std::string (){
		return myId;
	}

};


#endif /* CV_WINDOWS_HPP_ */
