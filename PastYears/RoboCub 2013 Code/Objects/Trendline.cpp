/*
 * Trendline.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: guy
 */

#include "Trendline.h"

#define ABS(x)	( (x) > 0 ? (x) : -(x) )

Trendline::Trendline()
{
	m_count = 0;

	yAxisValuesSum = 0;
	xAxisValuesSum = 0;
	xxSum = 0;
	xySum = 0;
}


void Trendline::UpdateTrendline(Point2DInt_t* pointsArray)
{
	m_points[m_count].x = (*pointsArray).y;
	m_points[m_count].y = (*pointsArray).x;

	m_count++;

	yAxisValuesSum += m_points[m_count-1].y;
	xAxisValuesSum += m_points[m_count-1].x;

	xySum += m_points[m_count-1].x*m_points[m_count-1].y;
	xxSum += m_points[m_count-1].x*m_points[m_count-1].x;

	CalculateSlope();
	CalculateIntercept();

}

void Trendline::AddPoint(Point2DInt_t* pointsArray)
{
	m_points[m_count].x = (*pointsArray).x;
	m_points[m_count].y = (*pointsArray).y;

	m_count++;
}

Trendline::~Trendline() {
	// TODO Auto-generated destructor stub
}

void Trendline::Initialize()
{
	yAxisValuesSum = 0;
	xAxisValuesSum = 0;
	xxSum = 0;
	xySum = 0;

	for (int i = 0; i < m_count; i++)
	{
		yAxisValuesSum += m_points[i].y;
		xAxisValuesSum += m_points[i].x;

		xySum += m_points[i].x*m_points[i].y;
		xxSum += m_points[i].x*m_points[i].x;
	}

	CalculateSlope();
	CalculateIntercept();
}

void Trendline::ResetTrendline()
{
	m_count = 0;

	yAxisValuesSum = 0;
	xAxisValuesSum = 0;
	xxSum = 0;
	xySum = 0;

	//m_slope = 0;
	//m_intercept = 0;
}

void Trendline::CalculateSlope()
{
	long long a = (long long)m_count*xxSum - (long long)xAxisValuesSum*xAxisValuesSum;
	long long b = (long long)m_count*xySum - (long long)xAxisValuesSum*yAxisValuesSum;

	if(a != 0)
	{
		m_slope =  (double)b/a;
	}
	else
	{
		m_slope = INF ;
	}

}


void Trendline::CalculateIntercept()
{
     m_intercept =   (yAxisValuesSum - m_slope*xAxisValuesSum)/m_count;
}


void Trendline::EraseLastPointsPrecentage(float pointsPrecentage)
{
	//cout << "m_count = " << m_count << " m_slope = " << m_slope << " m_intercept = " << m_intercept;

	int placeArrayToRemove = (float)m_count*(1-pointsPrecentage);

	for(int i = placeArrayToRemove; i < m_count ; i++)
	{
		yAxisValuesSum -= m_points[i].y;
		xAxisValuesSum -= m_points[i].x;

		xySum -= m_points[i].x*m_points[i].y;
		xxSum -= m_points[i].x*m_points[i].x;
	}

	m_count = placeArrayToRemove;

	CalculateSlope();
	CalculateIntercept();

//	cout << "New m_count = " << m_count << " m_slope = " << m_slope << " m_intercept = " << m_intercept << "\n";

}
double Trendline::PredictPoint(double x)
{
	if(m_slope == 0)
	{
		return m_points[m_count-1].y;
	}
	return x*m_slope + m_intercept;
}

double Trendline::PredictPointByY(double y)
{
	if(m_slope == 0)
	{
		return -INF;
	}
	return (y - m_intercept)/m_slope;
}


double Trendline::GetSlope()
{

	return m_slope;
}

double Trendline::GetIntecept()
{

	return m_intercept;
}


int Trendline::GetNumOfPoints()
{

	return m_count;
}

double  Trendline::GetDistancePointFromTerndLine(Point2DInt_t* pointsArray)
{

	return (ABS(-m_slope*pointsArray->x + pointsArray->y  - m_intercept))/(sqrt(1+m_slope*m_slope));

}















