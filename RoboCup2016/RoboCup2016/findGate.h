#include "stdafx.h"

using namespace cv;

const Scalar minWhiteHSV = Scalar(0, 0, 170);
const Scalar maxWhiteHSV = Scalar(255, 50, 255);
const Scalar minGreenBGR = Scalar(0, 70, 0);
const Scalar maxGreenBGR = Scalar(100, 255, 100);

void FindGate(Mat inputImage);