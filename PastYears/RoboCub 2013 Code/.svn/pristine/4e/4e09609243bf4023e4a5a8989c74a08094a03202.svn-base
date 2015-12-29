/*
 * Localization.h
 * creators and owners:
 * Yoav Miller yoavmil@gmail.com
 * Gilad Arnon (add your mail if you want)
 */

#ifndef LOCALIZATION_H_
#define LOCALIZATION_H_

#include "IncludesFile.h"
#include "General/MyThreadClass.h"
#include "Communication/Communication.h"
#include "Framework/include/CM730.h"
#include "../Objects/Ball.h"
#include "../Objects/Pole.h"

#include "PrivateClasses/LocalizationTypedefs.h"
#include "PrivateClasses/Algorithm.h"
#include "PrivateClasses/GyroHandler.h"
#include "PrivateClasses/BallHandler.h"
#include "PrivateClasses/PoleHandler.h"
#include "PrivateClasses/PoiHandler.h"
#include "PrivateClasses/StepHandler.h"
#include "AI/ListCombinedData.h"

#define LOCALIZATION_DEBUG
#undef LOCALIZATION_DEBUG
#define LOCALIZATION_PRINT_FIELD
#undef LOCALIZATION_PRINT_FIELD
#define LOCALIZATION_DEPRECATED

class Algorithm;
class GyroHandler;
class BallHandler;
class PoleHandler;
class PoiHandler;
class StepHandler;

class Localization
{
public:
	~Localization();
	void					process(); //main function. called from vision
	static Localization* 	GetInstance();

	BallLocation	 		GetBallLocation()      {return *m_ballLocation;}
	BallLocation*			GetBallLocationByRef() {return m_ballLocation;}
	Location 				GetMyLocation()        {return *m_robotLocation;}
	Location*				GetMyLocationByRef()   {return m_robotLocation;}
	Algorithm*				GetAlgo()              {return m_algorithm;}

	//called by vision
    void 					setBall(Ball& ball);
    void					setBall(bool isBallInSight);

#ifdef LOCALIZATION_DEPRECATED
    //TODO DEPRECATED
    void					setPoles(Pole& left,Pole& right);
    void					setPoles(Pole& pole);
    void					setPoles(bool detected);
#endif
    void					SetPois(list<PoiData>& poiList);

    //called by private classes
    myTimeval*				getCurrTimeByRef() {return &m_currTime;}
    myTimeval*				getPrevTimeByRef() {return &m_prevTime;}
    us*						getUsPassedByRef() {return &m_usPassed;}

    //called by communication
#ifdef LOCALIZATION_DEPRECATED
    void					SetDirection(uint8 color); //TODO DEPRECATED
#endif

    //called by goalie
    ListCombinedData*	 	getBallList() {return m_ballHandler->getBallList();}
    void 					setStepAside(char direction);

	bool 					amIOut() ;

	//when button is pressed
	void					ReEnterFiled () { m_algorithm->ReEnterToField(); }

	void 					printField();

#ifdef LOCALIZATION_DEBUG
	void					setPoiMoved() { m_poiHandler->SetHasMoved(true); }
#endif

private:
	Localization(); //singleton
	static Localization* 	_uniqueInstance;
	Communication* 			m_comm;
	BallHandler* 			m_ballHandler;
	GyroHandler* 			m_GyroHandler;
#ifdef LOCALIZATION_DEPRECATED
	PoleHandler* 			m_poleHandler; //TODO DEPRECATED
#endif
	PoiHandler*				m_poiHandler;
	StepHandler* 			m_stepHandler;
	Algorithm* 				m_algorithm;

	BallLocation*			m_ballLocation;
	Location*				m_robotLocation;

	myTimeval				m_currTime;
	myTimeval				m_prevTime;
	us						m_usPassed;

	myTimeval				m_lastTimeVisionUpdate;

	Diff mergeDiffs(const Diff& fromStep,const AngleDiff& fromGyro);
};

#endif /* LOCALIZATION_H_ */
