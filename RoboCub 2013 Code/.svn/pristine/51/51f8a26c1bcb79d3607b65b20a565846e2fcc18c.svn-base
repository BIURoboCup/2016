/*
 * LocalizationTypedefs.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#include "LocalizationTypedefs.h"

const double pixel2angleVertical=pixel2angleVertical;

Point2D getCoordinatesFromPix(double headAngle, pixel x, pixel y) {
	static const cm height=33;
	static const double alpha_m=Camera::VIEW_H_ANGLE*DEG2RAD/2;
	static const double sParam=height*tan(alpha_m)/160.0;

	headAngle*=DEG2RAD;
	Point2D result;
	double beta=((120-y)*pixel2angleVertical)*DEG2RAD;
	result.Y=height*tan(alpha_m+beta);
	result.X=(x-160)*sParam/cos(headAngle+beta);
	return result;
}




LocalizationDirection::LocalizationDirection(PoleType_e Forward,PoleType_e Backward, PoleType_e Left, PoleType_e Right)
{
	FORWARD_DIRECTION = Forward;
	BACKWARD_DIRECTION = Backward;
	LEFT_DIRECTION = Left;
	RIGHT_DIRECTION = Right;
}

BlueTeam::BlueTeam():LocalizationDirection(YELLOW_GOAL,BLUE_GOAL,YBY_POLE,BYB_POLE)
{

}

YellowTeam::YellowTeam():LocalizationDirection(BLUE_GOAL,YELLOW_GOAL,BYB_POLE,YBY_POLE)
{

}









