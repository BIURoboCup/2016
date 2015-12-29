#include "Vision.h"
#include "DetectedGate.h"

using namespace cv;

const Scalar minWhiteHSV = Scalar(0, 0, 170);
const Scalar maxWhiteHSV = Scalar(255, 50, 255);
const Scalar minGreenBGR = Scalar(0, 70, 0);
const Scalar maxGreenBGR = Scalar(100, 255, 100);

void FindGate(Mat& inputImage, DetectedGate& detectedGate);
void DrawGateOnImage(Mat& inputImage, DetectedGate& detectedGate);

// Utils methods 
int CountAndDrawWhitePixels(Mat& BWImage, Mat&outputImageToDraw, float top, float buttom, float left, float right);
RotatedRect FindLargestCandidate(vector<RotatedRect>& postCandidates);
void GetVerticalObjects(Mat &BWImage);
void GetHorizontalObjects(Mat &BWImage);
bool IsPartiallyOnField(Mat& field, Point2f bottomPoint, Point2f centerPoint);
bool IsAngleStraight(float angle);
void FindPostCandidates(Mat& field, vector<RotatedRect>& minRect, vector<RotatedRect>& postCandidates);
void FindField(Mat& inputImage, Mat& field);
void DrawRectangle(Mat &image, RotatedRect &rect);
void DrawPoint(Mat &image, Point2i &point);
void PrintStringOnImage(Mat &src, const char* string_to_print);
void ImageShowOnDebug(const string& winName, Mat image);
void GetWhiteImage(Mat& inputImage, Mat& onlyWhiteImage);