/*
 * PoleHandler.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#include "PoleHandler.h"
#include "../Localization.h"
#include "Vision.h"
//#define DEBUG_POLE_HANDLER
const uint8 PoleHandler::BLUE_GOAL_TEAM = 0;
const uint8 PoleHandler::YELLOW_GOAL_TEAM = 1;
const double PoleHandler::POLE_RATIO = (86.0/90.0);

/*
 * TODO make some automatic function that will calibrate the poleHeight2dist numbers
 */
PoleHandler::PoleHandler(Localization* loc):
m_debug(false),
m_algo(loc->GetAlgo()),
m_goleHeight2dist(37.5), /*this needs calibration is 36.6 or 40.0*/
m_sidePoleHeight2dist(18.6), /*can be 17.5 0r 18.6*/
m_samePoleCounter(0),
m_lastPoleId(NUMBER_OF_POLES),
m_hasMoved(true)
{
	m_pDirection = new YellowTeam();
	m_LastHeadAngle = 0;
}

PoleHandler::~PoleHandler()
{
	delete m_pDirection;
}

/*
 * poleHandler::setPoles(Pole & left, Pole & right)
 * converts to poles into data that could be used by Algorithm
 * If there's a picture of 2 whole poles, meaning you can see all of them, then they could be
 * used to find the fixed location.
 * If even 1 of the poles is not fully seen, then we can use each pole just to see the angle
 */
void PoleHandler::setPoles(Pole left, Pole right) {
	left.m_poleSide = POLE_SIDE_LEFT;
	right.m_poleSide = POLE_SIDE_RIGHT;
	setPoles(left);
	setPoles(right);
}

double PoleHandler::getDistFromPole(Pole& pole) {
	static const double pix2rad=pixel2angleVertical*PI/180.0;

	//-1 is invalid distance --> use only angle from pole
	if ((pole.m_lowestPixelPosition-pole.m_highestPixelPosition)>240) { return -1; }
	if ((pole.m_lowestPixelPosition-pole.m_highestPixelPosition)<=0) { return -1; }
	if (pole.m_lowestPixelPosition>=239) { return -1; }
	if (pole.m_highestPixelPosition<=1) { return -1; }
	if (pole.m_localizationBottomPoint.X<=2) return -1;
	if (pole.m_localizationBottomPoint.X>=318) return -1;
	if (pole.m_localizationTopPoint.X<=2) return -1;
	if (pole.m_localizationTopPoint.X>=318) return -1;

	/*
	 * Objects in the sides of the picture is bigger than in the middle, for some reason.
	 * so if the object is in the side of the picture (vertical or horizontal) make it smaller.
	 * we noticed that by multiplying by cos(angle) we can get close enough numbers
	 */
	double fixedHeight = (pole.m_localizationEuclideanHeight)
			*cos((fabs(pole.m_localizationMiddlePoint.X-160.0)+fabs(pole.m_localizationMiddlePoint.Y-120.0))*pix2rad);

	//side poles
	if (pole.m_poleColor == m_pDirection->LEFT_DIRECTION || pole.m_poleColor == m_pDirection->RIGHT_DIRECTION) {
		return m_sidePoleHeight2dist/tan(pix2rad*(fixedHeight)/2.0);
	}

	//goal poles
	return m_goleHeight2dist/tan(pix2rad*(fixedHeight)/2.0);
}

void PoleHandler::setPoles(Pole pole)
{
	PoleData tmpPoleData;
	double HeadXFromVision;

	if (pole.m_poleColor == m_pDirection->FORWARD_DIRECTION)
	{
		if(pole.m_poleSide == POLE_SIDE_LEFT)
			tmpPoleData.id=GOAL_LEFT_FRONT;
		else if(pole.m_poleSide == POLE_SIDE_RIGHT)
			tmpPoleData.id=GOAL_RIGHT_FRONT;
		else
			tmpPoleData.id=GOAL_UNKNOWN_FRONT;
	}
	else if (pole.m_poleColor == m_pDirection->BACKWARD_DIRECTION)
	{
		if(pole.m_poleSide == POLE_SIDE_LEFT)
			//when looking backwards, things are oposite!!
			tmpPoleData.id=GOAL_RIGHT_BACK;
		else if(pole.m_poleSide == POLE_SIDE_RIGHT)
			tmpPoleData.id=GOAL_LEFT_BACK;
		else
			tmpPoleData.id = GOAL_UNKNOWN_BACK;
	}
	else if (pole.m_poleColor == m_pDirection->LEFT_DIRECTION)
	{
		tmpPoleData.id= LEFT_POLE;
	}
	else if (pole.m_poleColor == m_pDirection->RIGHT_DIRECTION)
	{
		tmpPoleData.id= RIGHT_POLE;
	}

	//check that we are not just sampling the same pole all the time, which is bad.
	if(tmpPoleData.id==m_lastPoleId && !m_hasMoved) {
		if (++m_samePoleCounter>=4) {
			return;
		}
	} else {
		m_lastPoleId=tmpPoleData.id;
		m_samePoleCounter=0;
	}

	tmpPoleData.angleFromMe =pole.getXAngle();
	HeadXFromVision = Vision::GetInstance()->getCurrPicPan();
	tmpPoleData.angleFromMe+=HeadXFromVision;

	tmpPoleData.distFromMe=getDistFromPole(pole);

	m_poleDataList.push_back(tmpPoleData);
	if (m_debug) {//TODO replace with #ifdef
		cout<<"pole type ";
		if (pole.m_poleColor==120)
			cout<<"blue\n";
		else if (pole.m_poleColor==30)
			cout<<"yellow\n";
		else if (pole.m_poleColor==121)
			cout<<"byb\n";
		else if (pole.m_poleColor==122)
			cout<<"yby\n";
		else
			cout<<"unknown color\n";
		#ifdef DEBUG_POLE_HANDLER
		//cout<<"\t#poles: "<<m_poleDataList.size()<<" \t"
			cout	<<" Angle= "<<tmpPoleData.angleFromMe
				<<"\tDist= "<<tmpPoleData.distFromMe
				//<<"\tHeadXAngle= "<<HeadXFromVision
				//<<"\t head_y="<<Vision::GetInstance()->getCurrPicPan()
				<<"\tHight= "<<pole.m_localizationEuclideanHeight
				<<"\tID"<<tmpPoleData.id
				<<endl;
		#endif
	}
}

void PoleHandler::sendDataToAlgo() {
	m_algo->ParticleFilter(m_poleDataList);
}

/*
 * Used by Localization, to know if there's new data to send to Algorithm.
 */
bool PoleHandler::isNewData() {
	return m_poleDataList.size()>0;
}

Angle PoleHandler::getAngleFixedToXY(PoleId id, cm x,cm y)
{
	return Angle(RAD2DEG*atan2(FixedPoles[id].x-x,FixedPoles[id].y-y));
}

void PoleHandler::SetDirection(uint8 color)
{
	if (m_debug) cout<<"PoleHandler::SetDirection()"<<endl;
	if(m_pDirection != NULL)
		delete m_pDirection;
	switch(color)
	{
	case BLUE_GOAL_TEAM:
		m_pDirection = new BlueTeam;
		break;
	case YELLOW_GOAL_TEAM:
		m_pDirection = new YellowTeam;
		break;
	default:
		cout<<"ERROR!! NO SIDE WAS PICKED"<<endl;
		break;
	}
	m_poleDataList.clear();
}
