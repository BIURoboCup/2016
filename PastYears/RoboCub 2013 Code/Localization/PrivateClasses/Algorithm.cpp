/*
 * Algorithm.cpp
 *
 *  Created on: Jan 7, 2012
 *      Author: miller
 */

#include "Algorithm.h"
#include "../Localization.h"

Algorithm::Algorithm(Localization* locPtr):
m_loc(locPtr),//TODO remove this
m_currTimePtr(m_loc->getCurrTimeByRef()),
m_histogram(false),//TODO replace with #ifdef
m_outOfLocation(true),
m_stepsWithoutPolePicture(0) //a counter to know when we think we are out
{
	m_bestLocation.angle = 0;
	m_bestLocation.x = 0;
	m_bestLocation.y = 0;

	/*
	 * TODO some day try to make the size of the State vector dynamic: big when we don't know.
	 * small when we do know where we are. but its not so important.
	 */
	m_currState.resize(BIG_SET);

	for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)
	{
		it->randomize();
	}
}

/*
 * When robot reenters field from the side, change the particles and best location accordingly
 */
void Algorithm::ReEnterToField()
{
	m_bestLocation.angle=0;
	m_bestLocation.x=0; //I think maybe -200 or +200 is a better guess
	m_bestLocation.y=300;

	m_currState.resize(BIG_SET);

	for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)
	{
		it->randomizeEnterField();
	}
}

/*
 * Important function:
 * calculate the difference between the particle's location to the pole he really sees
 * the distance is in 2 ways: angle-diff and location-diff
 * not always we have a valid distance, for example, when the robot doesn't see all the pole's height
 * so we can't know the distance. but the angle is always available
 * so when the distance isn't available, we say the distance-diff is 0 --> doesn't matter.
 */
double Algorithm::getWeightFromPole(PoleData& pole,Particle& particle) //additional weight, not including original weight
{
	/*
	 * example for calculating. say the particle is at [0,400,50] and sees the right pole at distance
	 * 200, and in angle -10 (in the picture after subtracting the head angle).
	 * the angle2pole is atan2(80,600-400)=21.80140948635181
	 * the angle the robot is seeing is pole.angleFromMe=30
	 * so the angle diff is |21.8 - (50+(-10))| = 18.2
	 * the location diff is just pythagoras distance.
	 */
	Angle angle2pole=getAngleFixedToXY(pole.id,particle.x,particle.y);
	double angleDiff=fabs(angle2pole-(particle.angle+pole.angleFromMe));
	cm locationDiff=0;
	if (pole.distFromMe>0 && pole.distFromMe<1000)
	{
		cm distFromParticle=sqrt((particle.x-FixedPoles[pole.id].x)*(particle.x-FixedPoles[pole.id].x)
				+(particle.y-FixedPoles[pole.id].y)*(particle.y-FixedPoles[pole.id].y));
		locationDiff=fabs(distFromParticle-pole.distFromMe);
	}

	//there are many ways to create this value. this worked for us. also we saw something similar
	//somewhere else.
	return exp(-(angleDiff+locationDiff)/50.0);
}

/*
 * This is the most important function in localization. go through the poles in pole list
 * and calculate weight and make decisions...
 */
void Algorithm::ParticleFilter(list<PoleData> & poleList) //i have array of single poles
{
	/*
	 * if there is only one or two poles we can't trust that. so wait for more than 1.
	 * but if the oldest pole is very old, make an iteration.
	 */
	if (/*poleList.front().time-(*m_currTimePtr) > 50000 && */poleList.size()<=2) return;

	//reset counter
	m_stepsWithoutPolePicture=0;

#ifdef LOCALIZATION_DEBUG
	printf("Algorithm::ParticleFilter(list<PoleData> & poleList):\n");
	printf("have %d poles to go through\n",poleList.size());
#endif

	//this was a try to make a low pass filter to throw poles with noise. not tested.
	//TODO try to do this. not so important.
	//	bool DoneLPF = poleList.size() < 3 ? true:false; // lpf only for 3 or more poles;
	//	PoleData temp[3];
	//	int i;
	//	list<PoleData>::iterator it;
	//	list<PoleData>::iterator itErase;

	double w;
	while (poleList.size()>0)
	{
		//LPF stuff
		/*
		//		cout<<"START 1  "<<poleList.size()<<endl;
		//		it = poleList.begin();
		//		PoleId ID = it->id;
		//		i=0;
		//		while((!DoneLPF) && it!= poleList.end()) //LPF
		//		{
		//			cout<<"START 2  "<<poleList.size()<<"\ti= "<<i<<endl;
		//			if(it->id == ID)
		//			{
		//				if(it->distFromMe < 0)
		//				{
		//					DoneLPF = true;
		//					continue;
		//				}
		//				else
		//				{
		//					temp[i] = *it;
		//					if(i == 1)
		//						itErase = it;// the pole to erase
		//					it++;
		//					i++;
		//				}
		//			}
		//			if(i == 3)
		//			{
		//				cout<<"IF 2  "<<poleList.size()<<"\ti= "<<i<<endl;
		//				DoneLPF = true;
		//				if((fabs(temp[0].distFromMe - temp[2].distFromMe) < fabs(temp[0].distFromMe - temp[1].distFromMe )) && (temp[2].time - temp[0].time < 1000000))
		//				{
		//					poleList.erase(itErase);
		//				}
		//			}
		//		}
		*/

		PoleData pole=poleList.front();
		poleList.pop_front();

#ifdef LOCALIZATION_DEBUG
		cout<<"\t#poles: "<<poleList.size()<<" \t";
		cout<<"Dist: "<<pole.distFromMe<<"\tID: "<<pole.id<<endl;

		//if(DoneLPF && i==3)
		//	cout<<"Dist: "<<temp[1].distFromMe<<"\tID: "<<temp[1].id<<"\tTHROWED"<<endl;
#endif

		//go through each particle and calculate the wieght
		for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)//update weight
		{
			/*
			 * TODO: OMG found a bug!!
			 * what happens when the pole.id==GOAL_UNKNOWN_BACK ???!!!
			 * TODO FIXME
			 */
			if (pole.id==GOAL_UNKNOWN_FRONT) // either left or right front pole, give maximum weight.
			{
				double w0,w1;
				pole.id=GOAL_LEFT_FRONT;
				w0=getWeightFromPole(pole,*it);
				pole.id=GOAL_RIGHT_FRONT;
				w1=getWeightFromPole(pole,*it);
				pole.id=GOAL_UNKNOWN_FRONT;
				if (w0>w1) w=w0; else w=w1;
			}
			else //We know what pole it is.
			{
				w=getWeightFromPole(pole,*it);
			}

			//give new weight for the particle, while considering old weight.
			(it->weight)*=w;
		}
	}

	double sumOfWeight=0;
	for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++) {
		sumOfWeight+=it->weight;
	}
	sumOfWeight = 1 / sumOfWeight;

	//if all the sum of weight is very low, it means all the particles are out
	if (sumOfWeight<0.001)
		m_outOfLocation=true;
	else
		m_outOfLocation=false;

	//TODO use the sumOfWeight we calculated before and send to this function
	//normalize weight for the monte carlo
	NormalizeWeight(sumOfWeight); //sum of weights = 1

	//choose new set
	UpdateStates();
}

//This function will eventually replace the above function to handle
// POIs instead of poles
//The weight returned is additional weight, not including original weight
double Algorithm::GetWeightFromPoi(PoiData& poi,Particle& particle)
{
	/*
	 * example for calculating. say the particle is at [0,400,50] and sees the right pole at distance
	 * 200, and in angle -10 (in the picture after subtracting the head angle).
	 * the angle2pole is atan2(80,600-400)=21.80140948635181
	 * the angle the robot is seeing is pole.angleFromMe=30
	 * so the angle diff is |21.8 - (50+(-10))| = 18.2
	 * the location diff is just Pythagoras distance.
	 */
	Angle angle2poi=GetAngleFixedToXY(poi.id,particle.x,particle.y);
	double angleDiff=fabs(angle2poi-(particle.angle+poi.angleFromMe));
	cm locationDiff=0;
	if (poi.distFromMe>0 && poi.distFromMe<1000)
	{
		cm distFromParticle=sqrt((particle.x-FixedPois[poi.id].x)*(particle.x-FixedPois[poi.id].x)
				+(particle.y-FixedPois[poi.id].y)*(particle.y-FixedPois[poi.id].y));
		locationDiff=fabs(distFromParticle-poi.distFromMe);
	}
	//There are many ways to create this value. This worked for us. also we saw something similar
	// elsewhere
	double weight = exp(-(angleDiff+locationDiff)/50.0);

	return weight;
}


//This function will eventually replace the above function to handle
// POIs instead of poles
//TODO compare this function to the previous and see if we can upgrade some of the things
// they were unable to do last year
void Algorithm::ParticleFilter(list<PoiData>& poiList)
{
/*
	 * if there is only one or two poles we can't trust that. so wait for more than 1.
	 * but if the oldest pole is very old, make an iteration.
	 */
	//if (/*poleList.front().time-(*m_currTimePtr) > 50000 && */poleList.size()<=2) return;
	//TODO verify this line!!
	if(/**m_currTimePtr - poiList.front().time < 50000 ||*/ poiList.size() <= 2)
		return;

	//reset counter
	m_stepsWithoutPolePicture=0;

#ifdef LOCALIZATION_DEBUG
	cout << "[Algorithm::ParticleFilter]: have " << poiList.size() << " POIs to go through" << endl;
#endif

	//TODO attempt to create a LPF for the POIs
	double w, maxW;
	while (poiList.size()>0)
	{
		PoiData poi = poiList.front();
		poiList.pop_front();

#ifdef LOCALIZATION_DEBUG
		cout<<"\t#POIs: "<<poiList.size()<<" \t";
		cout<<"Dist: "<<poi.distFromMe<<"\tID: "<<poi.id<<endl;
#endif

		//go through each particle and calculate the weight
		for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)//update weight
		{
			if (poi.id==UNKNOWN)
			{
				w = 0;
				maxW = 0;
				for(int i=0;i<NUMBER_OF_POIS;i++)
				{
					poi.id = (PoiId)i;
					w = GetWeightFromPoi(poi,*it);
					maxW = w > maxW ? w : maxW;
				}
				w = maxW;
				poi.id = UNKNOWN;
			}
			else //We know what poi it is.
			{
				w = GetWeightFromPoi(poi,*it);
			}

			//give new weight for the particle, while considering old weight.
			(it->weight)*=w;
		}
	}

	double sumOfWeight=0;
	for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++) {
		sumOfWeight += it->weight;
	}

	//if all the sum of weight is very low, it means all the particles are out
	if ((sumOfWeight/m_currState.size()) < 0.001)
		m_outOfLocation=true;
	else
		m_outOfLocation=false;

	NormalizeWeight(sumOfWeight);

	//choose new set
	UpdateStates();
}

void Algorithm::UpdateStates()
{
	State prevState = m_currState;

	double meanWeight = 1.0/prevState.size();
	//how much noise to add: if we are in good location, don't add much noise,
	//so it won't throw us too far, its important for stability.
	//but if we are in bad location, add more noise, so
	//we will converge to the real location faster.
	int howMuchNoise = IN_FIELD_NOISE;
	if (m_outOfLocation)
		howMuchNoise = OUT_FIELD_NOISE;

	unsigned n = 0;
	double randWeight = drand48()*meanWeight;
	double summedWeight = 0;

	//choose new set of particles with Low variance sampler
	for (State_iterator it_prev = prevState.begin(); it_prev != prevState.end() ;it_prev++)
	{
		summedWeight += it_prev->weight;
		while (summedWeight > randWeight)
		{
			//choose the same particle as long its weight is larger than summedWeight
			if(n <= m_currState.size())
				m_currState[n++] = *it_prev;
			else {
				cout << "[Algorithm::UpdateStates] Critical segmentation error!" << endl;
				break;
			}
			randWeight += meanWeight;
		}
	}

	//This "if" should NEVER happen
	if(n==0)
	{
		cout << "[Algorithm::UpdateStates] Critical localization error!" << endl;
		for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)
		{
			it->randomize();
		}
	} else {
		UpdateBestLocation(); //weighted mean of all particles
		addNoise(howMuchNoise);
	}
	printHistogram();
}

void Algorithm::addNoise(double howMuch=1)
{
	for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)
	{
		it->x+=getNoise()*howMuch;
		it->y+=getNoise()*howMuch;
		it->angle+=getNoise()*howMuch;
	}
}

double Algorithm::getNoise()
{
	double noise= 2*drand48()-1; //[-1,1)
	return noise*fabs(noise);
}

double Algorithm::getTriangularNoise(double width)
{
	return 0;//TODO this? not sure its better than the above function
}

Angle Algorithm::getAngleFixedToXY(PoleId id, cm x,cm y)
{
	if(id<NUMBER_OF_POLES) //valid pole
		return Angle(RAD2DEG*atan2(FixedPoles[id].x-x,FixedPoles[id].y-y));
	else
		return 0;
}

//This function will eventually replace the above function to handle
// POIs instead of poles
Angle Algorithm::GetAngleFixedToXY(PoiId id, cm x,cm y)
{
	//Check for validity of the POI
	if(id<NUMBER_OF_POIS)
		return Angle(RAD2DEG*atan2(FixedPois[id].x-x,FixedPois[id].y-y));
	else
		return 0;
}

void Algorithm::NormalizeWeight(const double& sumOfWeights)
{
	for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)
	{
		it->weight /= sumOfWeights;
	}
}

/*
 * When the robot moves.
 * add to each particle the diff according to it's angle.
 * the Diff diff is not aware of the robots angle, it just knows the distance it moved relatively
 * TODO I think we don't need to add the diff to all the particles all the time,
 * just sum the diffs in 1 particle, and when pole data arrives, add all the particles the diff
 * we summed in that 1 particle. then reset that particle to [0,0,0]
 * it will save CPU usage, but need to think if its right to do it, mathematically.
 */
void Algorithm::ParticleFilter(Diff& diff)
{
	if (diff.angle==0 && diff.x==0 && diff.y==0)
		return;
	if (++m_stepsWithoutPolePicture > 20)
		m_outOfLocation = true;

	Angle ang = DEG2RAD*m_bestLocation.angle;

	//update the best particles
	m_bestLocation.time=diff.time;
	m_bestLocation.angle+=diff.angle/2; //walking direction to the mean angle
	m_bestLocation.y+=diff.y*cos(ang);
	m_bestLocation.y+=diff.x*sin(ang);
	m_bestLocation.x+=diff.y*sin(ang);
	m_bestLocation.x+=diff.x*cos(ang);
	m_bestLocation.angle+=diff.angle/2;

	//update all the particles
	for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)
	{
		ang = DEG2RAD*it->angle;
		it->angle+=diff.angle/2;
		it->y+=diff.y*cos(ang);
		it->y+=diff.x*sin(ang);
		it->x+=diff.y*sin(ang);
		it->x+=diff.x*cos(ang);
		it->angle+=diff.angle/2;
	}
	addNoise(0.5);
}

/*
 * calculate the best location according to the State vector's weighted average.
 * notice that finding the average angle isn't trivial, and needed to be done with summing cos and sin
 * look it up in google...
 */
void Algorithm::UpdateBestLocation()
{
	//TODO insert a locking mechanism for m_bestLocation (semaphore)
	Location temp;
	double sinSum = 0, cosSum = 0;
	double totalW = 0;
	Particle maxPart;
	for (State_iterator it = m_currState.begin(); it != m_currState.end(); it++)
	{
#ifdef LOCALIZATION_DEBUG
		if(it->weight > maxPart.weight)
		{
			maxPart.x = it->x;
			maxPart.y = it->y;
			maxPart.weight = it->weight;
		}
#endif
		totalW += it->weight;
		temp.x += it->x * it->weight;
		temp.y += it->y * it->weight;
		sinSum+=sin(DEG2RAD*it->angle)*it->weight;
		cosSum+=cos(DEG2RAD*it->angle)*it->weight;
	}

	//although totalW is NOT supposed to be 0, I don't trust it.
#ifdef LOCALIZATION_DEBUG
	cout << "[Algorithm::UpdateBestLocation] totalW = " << totalW << endl;
	cout << "[Algorithm::UpdateBestLocation] maxParticle: x= " << maxPart.x << " y= " << maxPart.y << endl;
#endif

	temp.x /= totalW;
	temp.y /= totalW;
	temp.angle /= totalW;
	temp.angle=RAD2DEG*atan2(sinSum/totalW,cosSum/totalW);

#ifdef LOCALIZATION_DEBUG
	cout << "[Algorithm::UpdateBestLocation] Best Location: x= " << temp.x << " y= " << temp.y << " angle=" << temp.angle<< endl;
#endif

	//check if the old location is far away from the new location
	if (fabs(temp.x-m_bestLocation.x) > 15 ||
		fabs(temp.y-m_bestLocation.y) > 15 ||
		fabs(temp.angle-m_bestLocation.angle) > 15)
	{
		m_bestLocation = temp;
		m_outOfLocation = true;
	}

	m_bestLocation.time=(*m_currTimePtr);

	//check for localization mega fail!!
	if (abs(m_bestLocation.x)>500 || (abs(m_bestLocation.y)>1200) )
	{
		for (State_iterator it = m_currState.begin(); it != m_currState.end() ;it++)
		{
			it->randomize();
		}
	}
}

/*
 * for debugging
 * print to stdout for each location in the field, how much weight is there.
 * if you'll try this you will see that sometimes there is some particles stuck in the corner
 * of the field --> there is a bug!
 */
void Algorithm::printHistogram() {
	if(!m_histogram) return;

	//make the histogram
	double hist[20][30];
	for (int i=0;i<20;++i) {
		for (int j=0;j<30;++j) {
			hist[i][j]=0;
		}
	}

	for (unsigned i=0;i<m_currState.size();++i) {
		if (abs(m_currState[i].x)>=200) continue;
		if (m_currState[i].y>=600) continue;
		if (m_currState[i].y<0) continue;

		hist[int(m_currState[i].x/20+10)][int(m_currState[i].y/20)]
		                                            +=m_currState[i].weight;
	}

	cout<<"---------------------------------------*******************-----------------------------------------\n";
	for (int j=0;j<30;++j) {
		for (int i=0;i<20;++i) {
			if (hist[i][30-j])
				printf("%0.2f ",hist[i][30-j]);
			else
				printf("---- ");
		}
		cout<<"\n";
	}
	cout<<m_bestLocation.x<<","<<m_bestLocation.y<<","<<m_bestLocation.angle<<endl;
}

/*
 * when half time, flip the coordinates of the field
 */
// TODO : find out what this does
void Algorithm::flipField(){
	m_bestLocation.x=-m_bestLocation.x;
	m_bestLocation.y=600-m_bestLocation.y;
	m_bestLocation.angle+=180;
	for (unsigned i=0;i<m_currState.size();++i){
		m_currState[i].x=-m_currState[i].x;
		m_currState[i].y=600-m_currState[i].y;
		m_currState[i].angle+=180;
	}
}
