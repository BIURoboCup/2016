/*
 * creators and owners:
 * Yoav Miller yoavmil@gmail.com
 * Gilad Arnon (add your mail if you want)
 * This is the heavy class!!!
 *
 */
//TODO remove any references to Poles when POIs are in place and working
#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "LocalizationTypedefs.h"
#include "../Objects/Angle.h"
#include <list>

#define IN_FIELD_NOISE 10
#define OUT_FIELD_NOISE 30

class Localization;


class Algorithm {
public:
	Algorithm(Localization* loc);
	~Algorithm() {};

	Location* 		getLocation() {return &m_bestLocation;}
	void 			ParticleFilter(Diff& diff);//step diff
	void 			ParticleFilter(list<PoleData>& poleList);
	void 			ParticleFilter(list<PoiData>& poiList);
	bool 			amIOut() {return m_outOfLocation;}
	void			flipField();
	void			ReEnterToField();

private:
	Localization*	m_loc;//TODO remove this // ??
	myTimeval* 		m_currTimePtr;

	//State is the set of the particles
	State			m_currState;

	Location		m_bestLocation;

	bool			m_histogram;

	//some indication when we think we are out of location
	bool 			m_outOfLocation;
	unsigned		m_stepsWithoutPolePicture;

	void			UpdateStates();
	void 			UpdateBestLocation();
	Angle			getAngleFixedToXY(PoleId id, cm x,cm y);
	Angle			GetAngleFixedToXY(PoiId id, cm x,cm y);
	void			NormalizeWeight(const double& sumOfWeight);
	void 			printSnapshot();
	void			addNoise(double howMuch);
	double 			getNoise();
	double			getTriangularNoise(double width);
	double 			getWeightFromPole(PoleData& pole,Particle& particle);
	double 			GetWeightFromPoi(PoiData& poi,Particle& particle);
	void 			printHistogram();
};

#endif /* ALGORITHM_H_ */
