/*
 * PoleHandler.h
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#ifndef POLEHANDLER_H_
#define POLEHANDLER_H_

#include "LocalizationTypedefs.h"
#include "../../Objects/Pole.h"
#include <list>

#define DEBUG_POLE_HANDLER

class Localization;
class Algorithm;

//DEPRECATED
class PoleHandler {
public:
	PoleHandler(Localization* loc);
	~PoleHandler();

	//called by vision
    void		setPoles(Pole left,Pole right);
    void		setPoles(Pole pole);
    void		setPoles(bool detected){};

    //called by Localization
    void 		sendDataToAlgo();
    bool		isNewData();
    void 		setHasMoved(bool hasMoved) {m_hasMoved=hasMoved;}

    //called Communication
    void		SetDirection(uint8 color);

    static const uint8 BLUE_GOAL_TEAM;
    static const uint8 YELLOW_GOAL_TEAM;
    static const double POLE_RATIO;

    //needed for when resampling the same pole over and over again, may cause to much noise!
    unsigned    		m_samePoleCounter;
    PoleId				m_lastPoleId;
private:

	bool 					m_debug;//TODO replace with #ifdef
	Algorithm* 				m_algo;
	list<PoleData>		 	m_poleDataList;
	double					m_goleHeight2dist;
	double 					m_goleWidth2dist;
	double					m_sidePoleHeight2dist;
	double					m_sidePoleWidth2dist;
	Particle				m_fixedLocation;
	LocalizationDirection*  m_pDirection;
	Angle					m_LastHeadAngle;

	double	getDistFromPole(Pole& pole);
	Angle	getAngleFixedToXY(PoleId id, cm x,cm y);

	bool 					m_hasMoved;

};

#endif /* POLEHANDLER_H_ */
