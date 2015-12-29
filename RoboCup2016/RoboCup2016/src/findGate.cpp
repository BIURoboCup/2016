
#include <iostream>
#include "Vision.h"
#include "findGate.h"

using namespace std;
using namespace cv;

void DrawRectangle(Mat &image, RotatedRect &rect)
{
	Point2f rect_points[4];
	rect.points(rect_points);

	for (int j = 0; j < 4; j++)
	{
		line(image, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255));
	}	
}
void PrintString(Mat &src, const char* string_to_print)
{
	char text[20];  //creating array that will store the print data
	sprintf_s(text, string_to_print);
	putText(src, text, Point(10, 10), Font_Type, Font_Scale, Font_Colour, 2); //Display the text in image window
}

//unsigned int check_side(Mat &whiteHorizontal, Mat &src, float check_Rect_top, float check_Rect_buttom, float check_Rect_left, float check_Rect_right, int vec0, int vec1)
//{
//	unsigned int Counter = 0;
//	for (size_t i = check_Rect_top; i < check_Rect_buttom; i++)
//	{
//		for (size_t j = check_Rect_left; j < check_Rect_right; j++)
//		{
//			if (whiteHorizontal.at<unsigned char>(i, j) > 100)
//			{
//				Counter++;
//				src.at<Vec3b>(i, j)[0] = vec0;
//				src.at<Vec3b>(i, j)[1] = vec1;
//				src.at<Vec3b>(i, j)[2] = 0;
//			}
//		}
//	}
//	return Counter;
//}
//check_side(check_Rect_top, check_Rect_top, right_check_Rect_left, right_check_Rect_right, 0, 255);
/*****************************************************************************************
* Method Name: FindGate
* Description: The function search for a couple of whithe vertical rectangles, whose buttom points
* 				are on green && upper part is not on green.
* 				This rectangles are marked as gate posts.
* 				If only one post is present in the frame, the function tries to find the direction
* 				of the second vertical post, using the upper post direction.
* Arguments: onlyWhiteImage Matrix
* Return Values: Gate location on screen
* Owner: Avi,Roman,Yoni,Gilad
*****************************************************************************************/
void FindGate(Mat inputImage)
{
	//Tresholding White
	//imshow("white", onlyWhiteImage);

	Mat onlyGreenImage;
	inRange(inputImage, minGreenBGR, maxGreenBGR, onlyGreenImage);
	/*blur(onlyGreenImage, onlyGreenImage, Size(20, 10));
	imshow("green", onlyGreenImage);*/

	Mat hsvImage;
	Mat onlyWhiteImage;
	cvtColor(inputImage, hsvImage, CV_BGR2HSV);
	inRange(hsvImage, minWhiteHSV, maxWhiteHSV, onlyWhiteImage);
	imshow("white", onlyWhiteImage);

	//dilation of white to horizontaly fill the shapes
	Mat elementG1 = getStructuringElement( MORPH_RECT, Size( 3, 5), Point( 2, 2 ) );
	dilate(onlyWhiteImage, onlyWhiteImage, elementG1);

	//eroding to remove horizontal lines
	Mat elementG2 = getStructuringElement( MORPH_RECT, Size(1,41));
	erode(onlyWhiteImage, onlyWhiteImage, elementG2);

	//dilation of white to fill the shapes
	Mat elementG3 = getStructuringElement( MORPH_RECT, Size( 5 , 5), Point( 2, 2 ) );
	dilate(onlyWhiteImage, onlyWhiteImage, elementG3);

	//creating Mat thr with contours, using canny
	cvtColor(onlyWhiteImage, onlyWhiteImage, CV_GRAY2BGR);
	Mat src = onlyWhiteImage;
	Mat thr,gray;
	
	cvtColor(onlyWhiteImage, gray, CV_BGR2GRAY);

	Canny(gray,thr,50, 190, 3, false );

	imshow("thr", thr);

	//crating contours vector
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat temp = thr.clone();
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,Point(0,0));

	//finding the blocking rectangles of the contours
	vector<RotatedRect> minRect( contours.size() );
	cvtColor(temp, temp, CV_GRAY2BGR);
	for( size_t i = 0; i < contours.size(); i++ )
	{
	  minRect[i] = minAreaRect( Mat(contours[i]) );
	  DrawRectangle(temp, minRect[i]);
	}

	imshow("contours", temp);

////////////////////////////////////////////////////////////////////////////////////
	//finding green convex field, Yoni, no need to do it twice

	Mat grass_cpy = onlyGreenImage.clone();
	Mat raw_shapes, convex;

	vector<vector<Point> > _contours; // Each contour stored as an array of points according to API.
	vector<Vec4i> _heir;

	cv::findContours(grass_cpy, _contours, _heir,
			CV_RETR_EXTERNAL,			// Only external shapes. (not one inside other)
			CV_CHAIN_APPROX_SIMPLE);	// how much point data about shape (From API )

	cv::threshold(onlyGreenImage,raw_shapes,255,0,THRESH_BINARY); // Make shapes black 1U image.
	convex = raw_shapes.clone(); // Clone (copy) blank image with same Dimention (Width,Height);

	int maxindex =-1; // Find the max shape by area = the green field.
	int maxArea = -1, currentArea;
	for (size_t i=0; i<(int)_contours.size(); i++)
	{
		if ((currentArea = cv::contourArea(_contours[i],false)) > maxArea)
		{
			maxindex = i;
			maxArea = currentArea;
		}
	}

	if (maxindex > -1)   // Found field
	{
		//  Find Convex of field:
		vector<vector<Point> > hull;
		hull.push_back(vector<Point>());

		cv::convexHull(_contours[maxindex],hull[0],true,true);
		cv::drawContours(convex,hull,0,Scalar(255,255,255),-1,4);

		// Draw field without convex:

		cv::drawContours(raw_shapes,_contours,
				maxindex,						// Inddex to draw -1 for all.
				Scalar(255,255,255),3,
				4,								// Drawing percision - more = slower (4 look like enough).
				_heir,2);
	}

	Mat or;
	bitwise_or(convex, thr, or);
	imshow("convex", or);

/////////////////////////////////////////////////////////////////////////////////////

	RotatedRect post1,post2;

	//Removing the rectangles that are not:  part of the rectangle is on green and part is not on green
	vector<RotatedRect> candidateRect;
	for( size_t i = 0; i < contours.size(); i++ )
	{
		float long_side;
		long_side = (minRect[i].size.height > minRect[i].size.width) ? minRect[i].size.height : minRect[i].size.width;
		//TODO: use long_side and and minRect[i].angle sin() calculation to determine lowest point
		Point2f bottom_point = minRect[i].center;
		if (bottom_point.y + long_side/2 +5 < FRAME_HEIGHT -5)
			bottom_point.y = bottom_point.y + long_side/2 +5;
		else
			bottom_point.y = FRAME_HEIGHT -5;

		bottom_point.y = (bottom_point.y + 30 <= FRAME_HEIGHT) ? bottom_point.y + 30 : bottom_point.y;

		//check if bottom part of the rectangle is on green and upper part is not on green
		if (convex.at<unsigned char>(bottom_point)>100
				&& convex.at<unsigned char>(minRect[i].center) < 100)
		{
			candidateRect.push_back(minRect[i]);
		}
	}

	//finding the largest rectangle among the qualified
	if (candidateRect.size() > 0)
	{
		Size2f s=candidateRect[0].size;
		size_t pl=0;
		for( size_t i = 0; i< candidateRect.size(); i++ )
		{
			if (candidateRect[i].size.area() > s.area())
			{
				pl=i;
				s=candidateRect[i].size;
			}
		}
		post1=candidateRect[pl];

		DrawRectangle(inputImage, candidateRect[pl]);
		//Draw the rectangle on src Mat
		Point2f rect_points[4]; candidateRect[pl].points( rect_points );
		for (int j = 0; j < 4; j++)
		{
			line(src, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);
		}
	}

	imshow("post1", src);

	//Removing the rectangles that are near the larest qualified rectangle (to find the second post)
	vector<RotatedRect> candidate2Rect;
	float post1_short_side = (post1.size.height < post1.size.width) ? post1.size.height : post1.size.width;
	for( size_t i = 0; i < candidateRect.size(); i++ )
	{
		//if (candidateRect[i].center.x + 3 * post1_short_side < post1.center.x
		//	|| candidateRect[i].center.x - 3 * post1_short_side > post1.center.x)
		if (candidateRect[i].center.x != post1.center.x)
		{
			candidate2Rect.push_back(candidateRect[i]);
		}
	}

	//finding the largest rectangle among the qualified
	if (candidate2Rect.size() > 0)
	{
		Size2f s=candidate2Rect[0].size;
		size_t pl=0;
		for( size_t i = 0; i< candidate2Rect.size(); i++ )
		{
			if (candidate2Rect[i].size.area() > s.area())
			{
				pl=i;
				s=candidate2Rect[i].size;
			}
		}
		post2=candidate2Rect[pl];

		DrawRectangle(inputImage, candidate2Rect[pl]);
		//Draw the second rectangle on src Mat
		Point2f rect_points[4]; candidate2Rect[pl].points( rect_points );
		for( int j = 0; j < 4; j++ )
		  line(src, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255), 1, 8 );
	}

	imshow("post2", src);
	//imshow("post2", inputImage);
	waitKey(0);

//////////////////////////////////////////////////////////////////////////////////////////

	RotatedRect postLeft,postRight;

	if (post1.size.area() > 0  &&  post2.size.area() > 0)
	{
		//both posts found, insert somekind of check to know if this is definetly the gate
		postLeft=  (post1.center.x < post2.center.x) ? post1 : post2;
		postRight= (post1.center.x > post2.center.x) ? post1 : post2;

		char text[20];  //creating array that will store the print data
		sprintf_s(text, "Full Gate");
		putText(src, text, Point(10,10), Font_Type, Font_Scale, Font_Colour,2); //Display the text in image window
	}
	else if (post1.size.area() > 0  && post2.size.area() == 0)
	{
		//finding second post direction using knowlage from upper post

		float post1_long_side = (post1.size.height > post1.size.width) ? post1.size.height : post1.size.width;
		float post1_short_side = (post1.size.height < post1.size.width) ? post1.size.height : post1.size.width;

		//finding check rectangels boundaries:
		float check_Rect_top, check_Rect_buttom, left_check_Rect_left , left_check_Rect_right,
												right_check_Rect_left , right_check_Rect_right;

		//finding check rectangels top and bottom
		if ( post1.center.y - post1_long_side/2 - 5 > 0 )
		{
			check_Rect_top = post1.center.y - post1_long_side/2 - 5;
			//the gate is symetric so the Y length of upper post is same as X length of side post
			check_Rect_buttom = post1.center.y - post1_long_side/2 + post1_short_side + 5;
		}
		else
		{
			check_Rect_top = 0;
			//the gate is symetric so the Y length of upper post is same as X length of side post
			check_Rect_buttom = 0 + post1_short_side + 5;
		}

		//finding left check rectangle, left and right side
		if ( post1.center.x - post1_short_side * 3 > 0)
		{
			left_check_Rect_left = post1.center.x - post1_short_side * 3;
			left_check_Rect_right= post1.center.x - post1_short_side;
		}
		else
		{
			left_check_Rect_left = 0;
			left_check_Rect_right= (post1.center.x - post1_short_side > 0) ? post1.center.x - post1_short_side : 0;
		}

		//finding right check rectangle, left and right side
		if ( post1.center.x + post1_short_side * 3 < FRAME_WIDTH -1)
		{
			right_check_Rect_left = post1.center.x + post1_short_side;
			right_check_Rect_right= post1.center.x + post1_short_side * 3;
		}
		else
		{
			right_check_Rect_left = (post1.center.x + post1_short_side < FRAME_WIDTH -1) ? post1.center.x + post1_short_side : FRAME_WIDTH -1;
			right_check_Rect_right= FRAME_WIDTH -1;
		}

		//counting white pixels near the top of the post on both sides
		unsigned int RightCounter = 0;
		unsigned int LeftCounter = 0;

		Mat whiteHorizontal = onlyWhiteImage.clone();

		//dilation of white to fill the shapes
		Mat elementG4 = getStructuringElement( MORPH_RECT, Size( 5, 3), Point( 2, 2 ) );
		dilate(whiteHorizontal, whiteHorizontal, elementG4);


		//check left side
		for(size_t i=check_Rect_top; i< check_Rect_buttom ; i++)
		{
			for(size_t j=left_check_Rect_left; j< left_check_Rect_right ; j++)
			{
				if (whiteHorizontal.at<unsigned char>(i,j) > 100)
				{
					LeftCounter++;
					src.at<Vec3b>(i,j)[0]= 255;
					src.at<Vec3b>(i,j)[1]= 0;
					src.at<Vec3b>(i,j)[2]= 0;
				}
			}
		}

		//check right side
		for(size_t i=check_Rect_top; i< check_Rect_buttom ; i++)
		{
			for(size_t j=right_check_Rect_left; j< right_check_Rect_right ; j++)
			{
				if ( whiteHorizontal.at<unsigned char>(i,j) > 100 )
				{
					RightCounter++;
					src.at<Vec3b>(i,j)[0]= 0;
					src.at<Vec3b>(i,j)[1]= 255;
					src.at<Vec3b>(i,j)[2]= 0;
				}
			}
		}

		if (RightCounter > LeftCounter  &&  RightCounter-LeftCounter > 5 )
		{
			PrintString(src, "------->");
		}
		else if (RightCounter < LeftCounter  &&  LeftCounter-RightCounter > 5 )
		{
			PrintString(src, "<-------");
		}
		else
		{
			PrintString(src, "Move camera up");
		}
	}
	else
	{
		PrintString(src, "Gate Not Found");
	}
}