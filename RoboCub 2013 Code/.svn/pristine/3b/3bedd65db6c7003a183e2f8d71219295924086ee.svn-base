#ifndef LOCALIZATION_TYPEDEFS_H
#define LOCALIZATION_TYPEDEFS_H

#include "IncludesFile.h"
#include "VisionEnums.h"
#include "Angle.h"
#include "Structs/Point2DInt.h"
#include "Point.h"
#include <vector>
#include "Camera.h"

using namespace Robot;

#define PI 3.14159265
#define DEG2RAD PI/180
#define RAD2DEG 180/PI
#define USEC2SEC 1/1000000.0

#define BIG_SET 200
#define SMALL_SET 50

typedef Angle AngleDiff;
typedef double cm;
typedef int pixel;
//typedef float Speed;
typedef long long us; //microseconds
typedef int Measure;
typedef unsigned char uint8;

struct Location {
	cm x;
	cm y;
	Angle angle;
	myTimeval time;
	Location() :
			x(0), y(0), angle(0) {
	}
	;
};

struct Particle {
	cm x;
	cm y;
	Angle angle;
	double weight;
	void randomize()
	{
		x = (rand() % 400) - 200; //originally: x=(rand() % 500) - 250;
		y = (rand() % 600);		  //originally: y=(rand() % 800) - 100;
		angle = (rand() % 360) - 180;
		weight = 1.0 / BIG_SET;
	}
	void randomizeEnterField() {
		int randAng, randX, randY;

		randX = (rand() % 2); // 0 or 1
		randY = (rand() % 20) - 10; // [-10,10)
		randAng = (rand() % 2); // 0 or 1
		x = (randX == 0) ? 200 : -200; //-200 or 200
		randX = (rand() % 20) - 10; // [-10,10)
		x += randX; // [-210,-190) or [-190, 210)
		y = 300 + randY; // [190,210)
		angle = randX == 0 ? -90 : 90; //TODO choose by x: if x<0 angle=90 else angle=-90
		randAng = (rand() % 10) - 5;
		angle += randAng;

		weight = 1.0 / BIG_SET;
	}
};

typedef vector<Particle> State;
typedef vector<Particle>::iterator State_iterator;
typedef Location Diff;
struct Step {
	int speed;
	int dir;
	unsigned long time;
};

struct BallLocation {
	cm x, y;
	myTimeval time;
	Angle angleFromMe; //important
	Angle direction;
	cm distanceFromMe; //important
	//Speed speedX,speedY;
	bool isInSight;
};

enum PoleId {
	GOAL_LEFT_FRONT,
	GOAL_RIGHT_FRONT,
	GOAL_UNKNOWN_FRONT,
	GOAL_CENTER_FRONT,
	GOAL_LEFT_BACK,
	GOAL_RIGHT_BACK,
	GOAL_UNKNOWN_BACK,
	GOAL_CENTER_BACK,
	LEFT_POLE,
	RIGHT_POLE,
	NUMBER_OF_POLES
};

struct PoleData {
	PoleId id;
	Angle angleFromMe;
	cm distFromMe;
	myTimeval time;
	bool IsHeadMove;
};

#define NUMBER_OF_POIS 24
enum PoiId {
	UNKNOWN,
	HOME_LEFT_CORNER,
	HOME_RIGHT_CORNER,
	HOME_LEFT_PENALTY_BACK,
	HOME_RIGHT_PENALTY_BACK,
	HOME_LEFT_PENALTY_FRONT,
	HOME_RIGHT_PENALTY_FRONT,
	HOME_LEFT_POLE,
	HOME_RIGHT_POLE,
	HOME_CENTER,
	RIVAL_LEFT_CORNER,
	RIVAL_RIGHT_CORNER,
	RIVAL_LEFT_PENALTY_BACK,
	RIVAL_RIGHT_PENALTY_BACK,
	RIVAL_LEFT_PENALTY_FRONT,
	RIVAL_RIGHT_PENALTY_FRONT,
	RIVAL_LEFT_POLE,
	RIVAL_RIGHT_POLE,
	RIVAL_CENTER,
	CENTER_FAR_LEFT,
	CENTER_FAR_RIGHT,
	CENTER_LEFT,
	CENTER_RIGHT,
	CENTER
};

struct PoiGroup
{
	int numberOfPois;
	virtual PoiId GetId(int index) = 0;

	virtual ~PoiGroup()
	{

	}
};

struct PoiGroupCorners : PoiGroup
{
private:
	PoiId poisArray[4];
public:
	PoiGroupCorners()
	{
		numberOfPois = 4;
		poisArray[0] = HOME_LEFT_CORNER;
		poisArray[1] = HOME_RIGHT_CORNER;
		poisArray[2] = RIVAL_LEFT_CORNER;
		poisArray[3] = RIVAL_RIGHT_CORNER;
	}

	PoiId GetId(int index)
	{
		return poisArray[index];
	}
};

struct PoiData {
	PoiId id;
	PoiGroup* poiGroup;
	Angle angleFromMe;
	cm distFromMe;
	myTimeval time;
	bool IsHeadMove;

	PoiData()
	{
		poiGroup = NULL;
	}

	~PoiData()
	{
		if (poiGroup != NULL)
		{
			delete poiGroup;
		}
	}

};

/*
 * GOAL_LEFT_FRONT 		{-80,600}
 * GOAL_RIGHT_FRONT 	{80,600}
 * GOAL_UNKNOWN_FRONT 	{1000,1000}
 * GOAL_CENTER_FRONT 	{0,600}
 * GOAL_LEFT_BACK 		{-80,0}
 * GOAL_RIGHT_BACK 		{80,0}
 * GOAL_UNKNOWN_BACK 	{-1000,-1000}
 * GOAL_CENTER_BACK  	{0,0}
 * LEFT_POLE 			{-240,300}
 * RIGHT_POLE 			{240,300}
 */
typedef Point2DInt_s FixedPole;
typedef Point2DInt_s FixedPoi;

static const FixedPole FixedPoles[NUMBER_OF_POLES] = { { -80, 600 },
		{ 80, 600 }, { 1000, 1000 }, { 0, 600 }, { -80, 0 }, { 80, 0 }, { -1000,
				-1000 }, { 0, 0 }, { -240, 300 }, { 240, 300 } };

/*
 * UNKNOWN						{1000,1000}
 * HOME_LEFT_CORNER				{-200,0}
 * HOME_RIGHT_CORNER			{200,0}
 * HOME_LEFT_PENALTY_BACK		{-110,0}
 * HOME_RIGHT_PENATLY_BACK		{110,0}
 * HOME_LEFT_PENATLY_FRONT		{-110,65}
 * HOME_RIGHT_PENATLY_FRONT		{110,65}
 * HOME_LEFT_POLE				{-80,0}
 * HOME_RIGHT_POLE				{80,0}
 * HOME_CENTER					{0,180}
 * RIVAL_LEFT_CORNER			{-200,600}
 * RIVAL_RIGHT_CORNER			{200,600}
 * RIVAL_LEFT_PENALTY_BACK		{-110,600}
 * RIVAL_RIGHT_PENATLY_BACK		{110,600}
 * RIVAL_LEFT_PENATLY_FRONT		{-110,535}
 * RIVAL_RIGHT_PENATLY_FRONT	{110,535}
 * RIVAL_LEFT_POLE				{-80,600}
 * RIVAL_RIGHT_POLE				{80,600}
 * RIVAL_CENTER					{0,420}
 * CENTER_FAR_LEFT				{-200,300}
 * CENTER_FAR_RIGHT				{200,300}
 * CENTER_LEFT					{-70,300}
 * CENTER_RIGHT					{70,300}
 * CENTER						{0,300}
 */
static const FixedPoi FixedPois[NUMBER_OF_POIS] = { {1000,1000}, {-200,0}, {200,0}, {-110,0},
		{110,0}, {-110,65}, {110,65}, {-80,0}, {80,0}, {0,180}, {-200,600}, {200,600},
		{-110,600}, {110,600}, {-110,535}, {110,535}, {-80,600}, {80,600}, {0,420},
		{-200,300}, {200,300}, {-70,300}, {70,300}, {0,300} };

Point2D getCoordinatesFromPix(double headAngle, pixel x, pixel y);

class LocalizationDirection {
public:
	LocalizationDirection();
	LocalizationDirection(PoleType_e Forward, PoleType_e Backward,
			PoleType_e Left, PoleType_e Right);
	PoleType_e FORWARD_DIRECTION, BACKWARD_DIRECTION, RIGHT_DIRECTION,
			LEFT_DIRECTION;
};

class BlueTeam: public LocalizationDirection {
public:
	BlueTeam();
};

class YellowTeam: public LocalizationDirection {
public:
	YellowTeam();
};

#endif
