/*
 * consts.h
 *
 *  Created on: Sep 17, 2015
 *      Author: root
 */

#define _CRT_SECURE_NO_WARNINGS

#ifndef SRC_CONSTS_H_
#define SRC_CONSTS_H_

/*****************************************************************************************
* Owners: All
* Files affected:
* Comments: None
* Updated: 23.01.15
*****************************************************************************************/
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

/*****************************************************************************************
* Owners: All - opencv print to screen Fonts
* Files affected:
* Comments: None
* Updated: 23.01.15
*****************************************************************************************/

#define Font_Type FONT_HERSHEY_PLAIN
#define Font_Position Point(355,475)
#define Font_Thickness 1
#define Font_Colour CV_RGB(255,0,0)
#define Font_Colour1 CV_RGB(10,0,50)
#define Font_Scale 3
#define FootNoteFont_Scale 0.7

/*****************************************************************************************
* Owners: Elimelech
* Files affected: Vision.h
* Comments: None
* Updated: 23.01.15
*****************************************************************************************/

#define _USE_MATH_DEFINES  //to import math def's from math (M_PI_4)

#define ROBOHEIGHT 59//24
#define U0 FRAME_WIDTH/2
#define V0 FRAME_HEIGHT/2


//#define AXISTRANSPOSE     //define this for upside-down camera

#endif /* SRC_CONSTS_H_ */
