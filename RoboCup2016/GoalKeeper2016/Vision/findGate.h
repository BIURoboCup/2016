#include "DetectedGate.h"

using namespace cv;

DetectedObject* FindGate(Mat& inputImage);

// Utils methods 
int CountAndDrawWhitePixels(Mat& BWImage, Mat&outputImageToDraw, float top, float buttom, float left, float right);
RotatedRect FindLargestCandidate(vector<RotatedRect>& postCandidates);
void GetVerticalObjects(Mat &BWImage);
void GetHorizontalObjects(Mat &BWImage);
bool IsPartiallyOnField(Mat& field, Point2f bottomPoint, Point2f centerPoint);
bool IsAngleStraight(float angle);
void FindPostCandidates(Mat& field, vector<RotatedRect>& minRect, vector<RotatedRect>& postCandidates);