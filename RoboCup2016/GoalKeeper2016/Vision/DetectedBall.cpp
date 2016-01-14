/*
 * DetectedBall.cpp
 *
 *  Created on: Jan 14, 2016
 *      Author: laptopyellow
 */

#include "DetectedBall.h"

DetectedBall::DetectedBall()
{
	m_isFound = false;
	Radius = 0;
}

DetectedBall::DetectedBall(Point2f center, float radius, bool isFound)
{
	m_isFound = isFound;
	Center = center;
	Radius = radius;
}

DetectedBall::~DetectedBall()
{
}

bool DetectedBall::IsDetected()
{
	return m_isFound;
}

void DetectedBall::Draw(Mat& image)
{
	DrawPoint(image, Center, Colors::Blue, 2);
}

