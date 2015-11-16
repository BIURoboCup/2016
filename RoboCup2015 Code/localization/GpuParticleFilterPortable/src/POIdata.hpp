#ifndef _POI_DATA_HEADER_
#define _POI_DATA_HEADER_

//#include <iostream>
#include <vector>


#include "../glm/glm.hpp"
#include "../glm/fwd.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#define DEG2RAD  M_PI/180
#define RAD2DEG  180/M_PI

class RAD;

// To help us know exactly the dype of the degree\rad:
class DEG {
friend class RAD;

private:
  float m;
  DEG(float degree):m(degree){};

public:
	DEG():m(360){};					// 360 is 0, for easy error finding
	DEG(RAD& radian);

	static DEG fromDegree(float degree){return DEG(degree);};
	static DEG fromRadian(float radian){return DEG(radian * RAD2DEG);};

	RAD toRAD();
	float getDEGValue(){return m;};
};

class RAD {
friend class DEG;

private:
  float m;
  RAD(float radian):m(radian){};

public:
	RAD():m(2*M_PI){};				// 2PI is 0, for easy error finding
	RAD(DEG& degree){*this = RAD::fromDegree(degree.m);};

	static RAD fromRadian(float radian){return RAD(radian);};
	static RAD fromDegree(float degree){return RAD(degree * DEG2RAD);};

	DEG toDEG() {return DEG::fromRadian(m);};
	float getRADValue(){return m;};
} ;

typedef struct {
	glm::vec2 pos;
	DEG degree;
} Particle;

typedef enum {
	L_SHAPE			=0,
	T_SHAPE			=1,
	DOT_SHAPE		=2,
	PLUS_SHAPE		=3,
	CIRCLE_SHAPE	=4,
	GENERAL_SHAPE	=5,	// In case we can't really identify it
} POItypes;

typedef struct {
	POItypes type;
	float distance;
	DEG degree;		// angle in degrees.
} POIReport;

POIReport newReport(POItypes _type_, float _distance_, DEG _degree_) ;

// From the official rules for 2015 @ 
//			https://www.robocuphumanoid.org/materials/rules/
enum FieldConsts {
	A = 900,
	B = 600,
	C = 50,
	D = 180,
	E = 60,
	F = 345,
	G = 180,
	H = 150,
	I = 70
} ; 
// Meaningfull names to use in our POI array:
enum allPOIsNames{
	// CENTER
		CENTER_FIELD = 0,
		CENTER_TOP_CORNER = 1,
		CENTER_LOW_CORNER = 2,

	//RIGHT
		RIGHT_PENALTY = 3,
	//RIGHT-TOP
		RIGHT_TOP_FIELD_CORNER = 4,
		RIGHT_TOP_FRONT_NET = 5,
		RIGHT_TOP_BACK_NET = 6,
		RIGHT_TOP_FRONT_GOAL_AREA = 7,
		RIGHT_TOP_BACK_GOAL_AREA = 8,
	//RIGHT-LOW
		RIGHT_LOW_FIELD_CORNER = 9,
		RIGHT_LOW_FRONT_NET = 10,
		RIGHT_LOW_BACK_NET = 11,
		RIGHT_LOW_FRONT_GOAL_AREA = 12,
		RIGHT_LOW_BACK_GOAL_AREA = 13,

	//LEFT
		LEFT_PENALTY = 14,
	//LEFT-TOP
		LEFT_TOP_FIELD_CORNER = 15,
		LEFT_TOP_FRONT_NET = 16,
		LEFT_TOP_BACK_NET = 17,
		LEFT_TOP_FRONT_GOAL_AREA = 18,
		LEFT_TOP_BACK_GOAL_AREA = 19,
	//LEFT-LOW
		LEFT_LOW_FIELD_CORNER = 20,
		LEFT_LOW_FRONT_NET = 21,
		LEFT_LOW_BACK_NET = 22,
		LEFT_LOW_FRONT_GOAL_AREA = 23,
		LEFT_LOW_BACK_GOAL_AREA = 24,

	// Plus:
		CENTER_TOP_PLUS = 25,
		CENTRT_LOW_PLUS = 26,

	// Always make this last:
		POIs_COUNT = 27,
};

//PointOfInterest POIs[];	// XY of each POI

#define c_arrlen(x) (sizeof(x)/sizeof(x[0]))  // length of array in native c.
#define LPOI_COUNT 12	// L
#define TPOI_COUNT 10	// T
#define DPOI_COUNT 2	// . (dot)
#define PPOI_COUNT 2	// + (plus)
#define CPOI_COUNT 1	// (+) <--- the ring in the center of the field



class IndexPOIs {
public:
	static int allPOIs[POIs_COUNT];
	static int LPOIs[LPOI_COUNT];
	static int TPOIs[TPOI_COUNT];
	static int DPOIs[DPOI_COUNT];
	static int PPOIs[PPOI_COUNT];
	static int CPOIs[CPOI_COUNT];

	// Names:
	static const char* allPOIsStrings[POIs_COUNT];
};

typedef glm::vec2 PointOfInterest;	  // Only XY

// XY data:
PointOfInterest *getPOIsXY();

// Group of POIs is presented with integers (indexes)
//		to avoid copy latancy.
//		also the shader will have them as uniform so only the index is important.
typedef std::vector<int>* POIvector;

// Indexes by type:
POIvector vecAllPOIs()		;
POIvector vecLPOIs()		;
POIvector vecTPOIs()		;
POIvector vecDPOIs()		;
POIvector vecPPOIs()		;
POIvector vecCPOIs()		;

void printShaderInfo();

#endif






