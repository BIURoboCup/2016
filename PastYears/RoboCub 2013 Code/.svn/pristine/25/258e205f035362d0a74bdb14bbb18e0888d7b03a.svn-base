/*
 * Trendline.h
 *
 *  Created on: Jan 19, 2012
 *      Author: guy
 */

#ifndef TRENDLINE_H_
#define TRENDLINE_H_

#include "IncludesFile.h"
#include "Structs/Point2DInt.h"
#include "Camera.h"


#define INF 999999

using namespace Robot;

class Trendline {

private:
	int				xAxisValuesSum;
    int 			xxSum;
    int 			xySum;
    int 			yAxisValuesSum;

    double 			m_slope;
    double 			m_intercept;

public:

    Point2DInt_t	m_points[Camera::HEIGHT];
    int 			m_count;

    Trendline();
    ~Trendline();

    void UpdateTrendline(Point2DInt_t* pointsArray);
    void AddPoint(Point2DInt_t* pointsArray);
    void Initialize();
    void ResetTrendline();
    void CalculateSlope();
    void CalculateIntercept();
    void EraseLastPointsPrecentage(float pointsPrecentage);

    double  PredictPoint(double x);
    double  PredictPointByY(double y);
    double  GetDistancePointFromTerndLine(Point2DInt_t* pointsArray);
    double GetSlope();
    double GetIntecept();
    int    GetNumOfPoints();
};


#endif /* TRENDLINE_H_ */

