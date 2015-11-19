/*
 * FirTrendline.h
 *
 *  Created on: Jan 19, 2012
 *      Author: guy
 */

#ifndef FirTrendline_H_
#define FirTrendline_H_

#include "IncludesFile.h"
#include "Structs/Point2DInt.h"
#include "Camera.h"

#define INF 999999

#define FIR_TRENDLINE_SIZE	70

using namespace Robot;

class FirTrendline {

private:
	int				xAxisValuesSum;
    int 			xxSum;
    int 			xySum;
    int 			yAxisValuesSum;

    double 			m_slope;
    double 			m_intercept;

public:


    FirTrendline();
    ~FirTrendline();

    void UpdateFirTrendline(Point2DInt_t* newPoint, Point2DInt_t* pointToRemove);
    void AddPoint(Point2DInt_t* pointsArray);

    void CalculateSlope();
    void CalculateIntercept();

    double  PredictPoint(double x);
    double  PredictPointByY(double y);
    double GetDistancePointFromTerndLine(Point2DInt_t* pointsArray);

    double GetSlope();
    double GetIntecept();

};


#endif /* FirTrendline_H_ */

