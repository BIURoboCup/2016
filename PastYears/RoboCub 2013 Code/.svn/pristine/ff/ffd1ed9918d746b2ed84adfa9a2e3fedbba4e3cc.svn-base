/*
 * FirTrendline.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: guy
 */

#include "FirTrendline.h"
#include <math.h>
#define ABS(x)	( (x) > 0 ? (x) : -(x) )


FirTrendline::FirTrendline()
{
	yAxisValuesSum = 0;
	xAxisValuesSum = 0;
	xxSum = 0;
	xySum = 0;
}
void FirTrendline::AddPoint(Point2DInt_t* point)
{

	yAxisValuesSum += point->y;
	xAxisValuesSum += point->x;

	xySum += point->y*point->x;
	xxSum += point->x*point->x;

}


void FirTrendline::UpdateFirTrendline(Point2DInt_t* newPoint, Point2DInt_t* pointToRemove)
{
	yAxisValuesSum +=   newPoint->y - pointToRemove->y;
	xAxisValuesSum +=   newPoint->x - pointToRemove->x;

	xySum += (newPoint->y)*(newPoint->x) - (pointToRemove->y)*(pointToRemove->x);
	xxSum += (newPoint->x)*(newPoint->x) - (pointToRemove->x)*(pointToRemove->x);


//	cout << "yAxisChange = " << newPoint->y - pointToRemove->y << " xAxisChange = " << newPoint->x - pointToRemove->x <<
//			" xySum minus =  << " << (pointToRemove->y - pointToRemove->x)*(pointToRemove->y - pointToRemove->x) << " xxMinus = " << (pointToRemove->x)*(pointToRemove->x) << "\n";

	CalculateSlope();
	CalculateIntercept();
}

void FirTrendline::CalculateSlope()
{
	long long a = (long long)FIR_TRENDLINE_SIZE*xxSum - (long long)xAxisValuesSum*xAxisValuesSum;
	long long b = (long long)FIR_TRENDLINE_SIZE*xySum - (long long)xAxisValuesSum*yAxisValuesSum;

	if(a != 0)
	{
		m_slope =  (double)b/a;
	}
	else
	{
		m_slope = 100000 ;
	}

}
void FirTrendline::CalculateIntercept()
{
     m_intercept =   (yAxisValuesSum - m_slope*xAxisValuesSum)/FIR_TRENDLINE_SIZE;
}



double FirTrendline::GetSlope()
{

	return m_slope;
}
double FirTrendline::GetIntecept()
{

	return m_intercept;
}

double FirTrendline::PredictPoint(double x)
{
/*	if(m_slope == 0)
	{
		return m_points[m_count-1].y;
	}
*/
	return x*m_slope + m_intercept;
}

double FirTrendline::PredictPointByY(double y)
{
	if(m_slope == 0)
	{
		return -INF;
	}
	return (y - m_intercept)/m_slope;
}

double  FirTrendline::GetDistancePointFromTerndLine(Point2DInt_t* pointsArray)
{

	return (ABS(-m_slope*pointsArray->x + pointsArray->y  - m_intercept))/(sqrt(1+m_slope*m_slope));

}


FirTrendline::~FirTrendline()
{
	// TODO Auto-generated destructor stub
}














