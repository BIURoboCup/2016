#include "POIdata.hpp"

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

// We need to define it after the decleration of RAD.
// For more information why it's neccesary read here:
//		http://www.umich.edu/~eecs381/handouts/IncompleteDeclarations.pdf
//		[ Page 3 ]
DEG::DEG(RAD& radian) {
	*this = DEG::fromRadian(radian.m);
}

RAD DEG::toRAD() {
	return RAD::fromDegree(m);
}

POIReport newReport(POItypes _type_, float _distance_, DEG _degree_) {
	POIReport p;
	p.type = _type_; p.distance = _distance_; p.degree = _degree_;
	return p;
}

// 	The points on the field a robot camera can report us he found:
PointOfInterest POIs[] = 
	{
		// Center of the field (Middle of circle)
			PointOfInterest(0,0),		
		// Top Field Corner (T-Shape)
			PointOfInterest(0,A * 0.5f),
		// Top Field Corner (T-Shape)
			PointOfInterest(0,-A * 0.5f),

		// Right side (x>0)
			// Penalty dot:
				PointOfInterest(A * 0.5f -G ,0),	

			// Top side:				
				// Field Corner (L-Shape):
					PointOfInterest(A * 0.5f, B * 0.5f),
				// Front net corner (T-Shape):
					PointOfInterest(A * 0.5f, D * 0.5f),
				// Back net cornet (L-Shape):
					PointOfInterest(A * 0.5f + C, D * 0.5f),
				// Front goalie area corner (L-Shape):
					PointOfInterest(A * 0.5f - E, F * 0.5f),
				// Back goalie area corner (T-Shape):
					PointOfInterest(A * 0.5f, F * 0.5f),

			// Low side:				
				// Field Corner (L-Shape):
					PointOfInterest(A * 0.5f, -B * 0.5f),
				// Front net corner (T-Shape):
					PointOfInterest(A * 0.5f, -D * 0.5f),
				// Back net cornet (L-Shape):
					PointOfInterest(A * 0.5f + C, -D * 0.5f),
				// Front goalie area corner (L-Shape):
					PointOfInterest(A * 0.5f - E, -F * 0.5f),
				// Back goalie area corner (T-Shape):
					PointOfInterest(A * 0.5f, -F * 0.5f),

		// Left side (x>0)
			// Penalty dot:
				PointOfInterest(-A * 0.5f +G ,0),	

			// Top side:				
				// Field Corner (L-Shape):
					PointOfInterest(-A * 0.5f, B * 0.5f),
				// Front net corner (T-Shape):
					PointOfInterest(-A * 0.5f, D * 0.5f),
				// Back net cornet (L-Shape):
					PointOfInterest(-A * 0.5f - C, D * 0.5f),
				// Front goalie area corner (L-Shape):
					PointOfInterest(-A * 0.5f + E, F * 0.5f),
				// Back goalie area corner (T-Shape):
					PointOfInterest(-A * 0.5f, F * 0.5f),

			// Low side:				
				// Field Corner (L-Shape):
					PointOfInterest(-A * 0.5f, -B * 0.5f),
				// Front net corner (T-Shape):
					PointOfInterest(-A * 0.5f, -D * 0.5f),
				// Back net cornet (L-Shape):
					PointOfInterest(-A * 0.5f - C, -D * 0.5f),
				// Front goalie area corner (L-Shape):
					PointOfInterest(-A * 0.5f + E, -F * 0.5f),
				// Back goalie area corner (T-Shape):
					PointOfInterest(-A * 0.5f, -F * 0.5f),

		// Plus:
				// TOP:
					PointOfInterest(0, H*0.5f),
				// LOW:
					PointOfInterest(0, -H*0.5f),
	};

PointOfInterest *getPOIsXY() {return POIs;}



int IndexPOIs::allPOIs[POIs_COUNT] = {
	CENTER_FIELD ,
	CENTER_TOP_CORNER ,
	CENTER_LOW_CORNER ,

	RIGHT_PENALTY ,
	RIGHT_TOP_FIELD_CORNER ,
	RIGHT_TOP_FRONT_NET ,
	RIGHT_TOP_BACK_NET ,
	RIGHT_TOP_FRONT_GOAL_AREA ,
	RIGHT_TOP_BACK_GOAL_AREA ,
	RIGHT_LOW_FIELD_CORNER ,
	RIGHT_LOW_FRONT_NET ,
	RIGHT_LOW_BACK_NET ,
	RIGHT_LOW_FRONT_GOAL_AREA ,
	RIGHT_LOW_BACK_GOAL_AREA ,

	LEFT_PENALTY ,
	LEFT_TOP_FIELD_CORNER ,
	LEFT_TOP_FRONT_NET ,
	LEFT_TOP_BACK_NET ,
	LEFT_TOP_FRONT_GOAL_AREA ,
	LEFT_TOP_BACK_GOAL_AREA ,
	LEFT_LOW_FIELD_CORNER ,
	LEFT_LOW_FRONT_NET ,
	LEFT_LOW_BACK_NET ,
	LEFT_LOW_FRONT_GOAL_AREA ,
	LEFT_LOW_BACK_GOAL_AREA,

	CENTER_TOP_PLUS,
	CENTRT_LOW_PLUS
};


int IndexPOIs::LPOIs[] = {
	RIGHT_TOP_FIELD_CORNER ,
	RIGHT_TOP_BACK_NET ,
	RIGHT_TOP_FRONT_GOAL_AREA ,
	RIGHT_LOW_FIELD_CORNER ,
	RIGHT_LOW_BACK_NET ,
	RIGHT_LOW_FRONT_GOAL_AREA, 

	LEFT_TOP_FIELD_CORNER ,
	LEFT_TOP_BACK_NET ,
	LEFT_TOP_FRONT_GOAL_AREA ,
	LEFT_LOW_FIELD_CORNER ,
	LEFT_LOW_BACK_NET ,
	LEFT_LOW_FRONT_GOAL_AREA 
};

int IndexPOIs::TPOIs[] = {
	CENTER_TOP_CORNER ,
	CENTER_LOW_CORNER ,

	RIGHT_TOP_FRONT_NET ,
	RIGHT_TOP_BACK_GOAL_AREA ,
	RIGHT_LOW_FRONT_NET ,
	RIGHT_LOW_BACK_GOAL_AREA ,
							  
	LEFT_TOP_FRONT_NET ,
	LEFT_TOP_BACK_GOAL_AREA ,
	LEFT_LOW_FRONT_NET ,
	LEFT_LOW_BACK_GOAL_AREA 
};

int IndexPOIs::DPOIs[] = {
	RIGHT_PENALTY ,
	LEFT_PENALTY ,
};

int IndexPOIs::PPOIs[] = {
	CENTER_TOP_PLUS,
	CENTRT_LOW_PLUS
};

int IndexPOIs::CPOIs[] = {
	CENTER_FIELD ,
};


std::vector<int> _vecAllPOIs(IndexPOIs::allPOIs,IndexPOIs::allPOIs + POIs_COUNT );
std::vector<int> _vecLPOIs(IndexPOIs::LPOIs,IndexPOIs::LPOIs + LPOI_COUNT );
std::vector<int> _vecTPOIs(IndexPOIs::TPOIs,IndexPOIs::TPOIs + TPOI_COUNT );
std::vector<int> _vecDPOIs(IndexPOIs::DPOIs,IndexPOIs::DPOIs + DPOI_COUNT );
std::vector<int> _vecPPOIs(IndexPOIs::PPOIs,IndexPOIs::PPOIs + PPOI_COUNT );
std::vector<int> _vecCPOIs(IndexPOIs::CPOIs,IndexPOIs::CPOIs + CPOI_COUNT );


std::vector<int> *vecAllPOIs()		{return &_vecAllPOIs;}
std::vector<int> *vecLPOIs()		{return &_vecLPOIs;}
std::vector<int> *vecTPOIs()		{return &_vecTPOIs;}

std::vector<int> *vecDPOIs()		{return &_vecDPOIs;}
std::vector<int> *vecPPOIs()		{return &_vecPPOIs;}
std::vector<int> *vecCPOIs()		{return &_vecCPOIs;}


const char* IndexPOIs::allPOIsStrings[] = {
	"CENTER_FIELD"			   ,
	"CENTER_TOP_CORNER"		   ,
	"CENTER_LOW_CORNER"		   ,
							   
	"RIGHT_PENALTY"			   ,
	"RIGHT_TOP_FIELD_CORNER"   ,
	"RIGHT_TOP_FRONT_NET"	   ,
	"RIGHT_TOP_BACK_NET"	   ,
	"RIGHT_TOP_FRONT_GOAL_AREA",
	"RIGHT_TOP_BACK_GOAL_AREA" ,
	"RIGHT_LOW_FIELD_CORNER"   ,
	"RIGHT_LOW_FRONT_NET"	   ,
	"RIGHT_LOW_BACK_NET"	   ,
	"RIGHT_LOW_FRONT_GOAL_AREA",
	"RIGHT_LOW_BACK_GOAL_AREA" ,
							   
	"LEFT_PENALTY"			   ,
	"LEFT_TOP_FIELD_CORNER"	   ,
	"LEFT_TOP_FRONT_NET"	   ,
	"LEFT_TOP_BACK_NET"		   ,
	"LEFT_TOP_FRONT_GOAL_AREA" ,
	"LEFT_TOP_BACK_GOAL_AREA"  ,
	"LEFT_LOW_FIELD_CORNER"	   ,
	"LEFT_LOW_FRONT_NET"	   ,
	"LEFT_LOW_BACK_NET"		   ,
	"LEFT_LOW_FRONT_GOAL_AREA" ,
	"LEFT_LOW_BACK_GOAL_AREA " ,

	"CENTER_TOP_PLUS"		   ,
	"CENTRT_LOW_PLUS"          ,
};



/**

 * C++ version std::string style "itoa":

 */

std::string itoa(int value, unsigned int base) {
	const char digitMap[] = "0123456789abcdef";
	std::string buf;

	// Guard:
	if (base == 0 || base > 16) {
		// Error: may add more trace/log output here
		return buf;
	}

	// Take care of negative int:
	std::string sign;
	int _value = value;

	// Check for case when input is zero:
	if (_value == 0) return "0";

	if (value < 0) {
		_value = -value;
		sign = "-";
	}

	// Translating number to string with base:
	for (int i = 30; _value && i ; --i) {
		buf = digitMap[ _value % base ] + buf;
		_value /= base;
	}

	return sign.append(buf);
}


string arraysString(POIvector typeList) {
	string result;
	if (typeList->size() > 0 ) {
		result += itoa ( (int)((*typeList)[0]) , 10 );
		for (unsigned int i=1; i< typeList->size(); i++) {
			result  += string(",") + itoa( (int)((*typeList)[i]) , 10 );
		}
	}
	return result;
}

void printShaderSpecificType(string typeChar, POIvector typeList ){
cout	<< "// Return weight of particle regarding all known " << typeChar << " values:\n"
		<< "float " << typeChar << "Report(int reportIndex) {\n"
		<< "	float _w = 0.0f;\n"
		<< "	Report _r = reports[reportIndex];\n"
		<< "	int _POIs[" << typeList->size() << "] = int[] (" << arraysString(typeList) << ");\n"
		<< "	for (int i =0; i < " << typeList->size() << "; i++) {\n"
		<< "		_w = _w +\n"
		<< "			w(\n"
		<< "				_r.dis,\n"
		<< "				_r.rad,\n"
		<< "				oldParticle,\n"
		<< "				_POIs[i]\n"
		<< "			);\n"
		<< "	}\n"
		<< "	return _w;\n"
		<< "}\n\n"
;
}

void printShaderInfo() {
	printShaderSpecificType("ALL",vecAllPOIs() );
	printShaderSpecificType("L",vecLPOIs()	 );
	printShaderSpecificType("T",vecTPOIs()	 );
	printShaderSpecificType("D",vecDPOIs()	 );
	printShaderSpecificType("P",vecPPOIs()	 );
	printShaderSpecificType("C",vecCPOIs()	 );
}
