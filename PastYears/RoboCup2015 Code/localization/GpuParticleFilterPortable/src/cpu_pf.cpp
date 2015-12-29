



#include "cpu_pf.hpp"
#include "POIdata.hpp"


//using namespace std;
using namespace glm;



#define fRAND ((float)rand()/(float)2147483647)


//*********************************************************
//			INPUT, OUTPUT
//*********************************************************

// The particles from last time, and it's size
//	(which is the weight from last time)
// Particle has data (x,y,teta,length=weight)

vec4 oldParticle;
float oldWeight ;

vec4 _oldParticle[CPU_PSIZE];
float _oldWeight [CPU_PSIZE];
int _current=0;

// The new data:
//	1) Particle may move according to motor data.
//	2) Weight change due to new reports.
vec4 outp;
float outw;

vec4 _outp[CPU_PSIZE];
float _outw[CPU_PSIZE];

// The flag to decide if we calculation weight or resampling:
bool _step = 0;


//*********************************************************
//			Weight calculation with Zt Ut
//*********************************************************

/* Vector data structure
struct Report {
	float dis;	// Distance  | = X
	float rad;	// Radians!  | = Y
	float type;	// L,T,C,+   | = Z
};

struct MotorInfo {
	float dis;               | = X
	float rad;               | = Y
};
*/

 vec2 cpu_POIs[30]; // All the POIs (x,y)

// All the report\movment data.
// Since we must use constant arrays in glsl_3.3 we use upper
//		bound of 30, and we will pass the real count with `reportCount`;
 int 		reportCount;
 vec3 		reports[30];
 vec2		movementInfo;

//*********************************************************
//			Resampling new particles
//*********************************************************

// After calculation w_i we need to resample.
 int M;			// Actual number of particles (M <= 4096)

 float a[CPU_PSIZE]; 	// accumlated weight

//*********************************************************
//			General weight function
//*********************************************************

// General weight function:
// all angles must be in radians!
float dcoeff = 0.1f;
float tcoeff = 10.0f;
float w(
	float reportDistance,
	float reportAngle,
	vec4 p,
	int POIindex) {
// The coeff are based on weight trying to find good fit.
float	d =  dcoeff *  abs(reportDistance -  distance(cpu_POIs[POIindex], vec2(p.x,p.y)  ));
float	t =  tcoeff *  abs(
					reportAngle
					- atan(cpu_POIs[POIindex].y - p.y,cpu_POIs[POIindex].x - p.x)
					- p.z // angle in radians.
							);
return exp(-d-t);
}

//*********************************************************
//	Weight per report. Generated with : `printShaderInfo();`
//*********************************************************

// Return weight of particle regarding all known ALL values:
float ALLReport(int reportIndex) {
	float _w = 0.0f;
	vec3 _r = reports[reportIndex];
	int _POIs[27] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	for (int i =0; i < 27; i++) {
		_w = _w +
			w(
				_r.x,
				_r.y,
				oldParticle,
				_POIs[i]
			);
	}
	return _w;
}

// Return weight of particle regarding all known L values:
float LReport(int reportIndex) {
	float _w = 0.0f;
	vec3 _r = reports[reportIndex];
	int _POIs[12] = {4,6,7,9,11,12,15,17,18,20,22,23};
	for (int i =0; i < 12; i++) {
		_w = _w +
			w(
				_r.x,
				_r.y,
				oldParticle,
				_POIs[i]
			);
	}
	return _w;
}

// Return weight of particle regarding all known T values:
float TReport(int reportIndex) {
	float _w = 0.0f;
	vec3 _r = reports[reportIndex];
	int _POIs[10] ={1,2,5,8,10,13,16,19,21,24};
	for (int i =0; i < 10; i++) {
		_w = _w +
			w(
				_r.x,
				_r.y,
				oldParticle,
				_POIs[i]
			);
	}
	return _w;
}

// Return weight of particle regarding all known D values:
float DReport(int reportIndex) {
	float _w = 0.0f;
	vec3 _r = reports[reportIndex];
	int _POIs[2] = {3,14};
	for (int i =0; i < 2; i++) {
		_w = _w +
			w(
				_r.x,
				_r.y,
				oldParticle,
				_POIs[i]
			);
	}
	return _w;
}

// Return weight of particle regarding all known P values:
float PReport(int reportIndex) {
	float _w = 0.0f;
	vec3 _r = reports[reportIndex];
	int _POIs[2] = {25,26};
	for (int i =0; i < 2; i++) {
		_w = _w +
			w(
				_r.x,
				_r.y,
				oldParticle,
				_POIs[i]
			);
	}
	return _w;
}

// Return weight of particle regarding all known C values:
float CReport(int reportIndex) {
	float _w = 0.0f;
	vec3 _r = reports[reportIndex];
	int _POIs[1] = {0};
	for (int i =0; i < 1; i++) {
		_w = _w +
			w(
				_r.x,
				_r.y,
				oldParticle,
				_POIs[i]
			);
	}
	return _w;
}

//*********************************************************
//	Find appropiate particle
//*********************************************************

int binaryRangeFind() {
    int steps=0;
    int start=0,end=M-1;

    if (!(end>=start)) return-1; // Empty array

    float target = (fRAND) * a[M-1];

   	if (target <= a[0]) return 0;

    if (M-2 >= 0)
    	if (target > a[M-2] ) return M-1;

    while (start < end) {
        steps ++;

        int middle = (end-start-(end-start)%2+1)/2 + start  ;
        if (target > a[middle]) {
            start = middle + 1;
        }else {
            end = middle;
        }
    }
    return end;
}

//*********************************************************
//		Main shader actions:
//*********************************************************

void _cpu_pf_main(void)
{
	for (int _cpu_Index = 0; _cpu_Index < CPU_PSIZE ; _cpu_Index ++ ) {
		oldParticle = _oldParticle[_cpu_Index];
		oldWeight = _oldWeight[_cpu_Index];


	if (!_step) {
		//calculate new weight
		float _w;

		// Get weight from each report:
		for (int i=0; i < reportCount; i++) {
			switch (int(reports[i].z)) { // type
				case 0:		// L
					_w += LReport(i);
					break;
				case 1:		// T
					_w += TReport(i);
					break;
				case 2:		// DOT
					_w += DReport(i);
					break;
				case 3:		// PLUS
					_w += PReport(i);
					break;
				case 4:		// Circle
					_w += CReport(i);
					break;
				case 5:		// General (ALL)
					_w += ALLReport(i);
					break;
			}
		}

		// Move particle:
		vec3 movedParticel = vec3(oldParticle.x,oldParticle.y,oldParticle.z); // old postion\angle data

		// Movment algo here.

		outp = vec4(movedParticel, oldParticle.w); // save old weight.
		outw = _w; // new weight

	}else {
		// resample



		_outp[_cpu_Index] = oldParticle;
		_outw[_cpu_Index] = oldWeight;
	}

	}
}


 //*********************************
 //* Test Init:
 //*********************************

#define randRange(a, b) (a + fRAND*(b-a))

// Generate report with uniform noise - N(U).
//#define CONTAINED(x,arr) ( std::find(arr->begin(), arr->end(), x) != arr->end() )

POIReport _cpu_generateReportNu(short name, Particle realPlace,  float derror, RAD aerror) {
	double dist = randRange(-derror, derror);
	double radian = randRange(-aerror.getRADValue(), aerror.getRADValue());



	POItypes reportType = GENERAL_SHAPE;

	// Select what kind of report the robot will give base on the POIs:

	/*
	if ( CONTAINED((int)name,vecLPOIs()) ) {
		reportType = L_SHAPE;
	} else if( CONTAINED((int)name, vecTPOIs()) ) {
		reportType = T_SHAPE;
	} else if( CONTAINED((int)name, vecCPOIs()) ) {
		reportType =CIRCLE_SHAPE;
	} else if( CONTAINED((int)name, vecDPOIs()) ) {
			reportType = DOT_SHAPE;
	} else if( CONTAINED((int)name, vecPPOIs()) ) {
			reportType = PLUS_SHAPE;
	//else - general shape.
	}
	*/

	PointOfInterest* POIs = getPOIsXY();
	// Find angle and distance and add to the noise:
	// distance = built in glm distance function
	dist	+= glm::distance(realPlace.pos, POIs[name]);

	// angle = atan(dy,dx);
	radian	+= glm::atan(POIs[name].y - realPlace.pos.y, POIs[name].x - realPlace.pos.x)
		-  realPlace.degree.toRAD().getRADValue();
				// XY space from Camera space. so (dteta) becomes (p.angles + dteta)

	return newReport(reportType, dist, DEG::fromRadian(radian));
}


void _cpu_pf_rand_particles() {
	for (int i=0;i<CPU_PSIZE;i++) {
		_oldParticle[i].x =  randRange(0,A/2);
		_oldParticle[i].y =  randRange(-B/2,B/2);
		_oldParticle[i].z =  randRange(-M_PI,M_PI);
		_oldParticle[i].w =  1.0f; // Length are equals.
	}
}

void _cpu_pf_set_reports(std::vector<POIReport> reportsList) {
	reportCount = reportsList.size();
	for (int i=0;i<reportCount;i++){
		reports[i].x = reportsList[i].distance;
		reports[i].y = reportsList[i].degree.toRAD().getRADValue(); // Every value is in radians!
		reports[i].z = reportsList[i].type;
	}
}

void _cpu_pf_init_test() {
	PointOfInterest* allXY = getPOIsXY();

	POIvector all = vecAllPOIs();
	for (unsigned int i=0;i < all->size();i++) {
		cpu_POIs[i] = allXY[(*all)[i]];
	}

	// For our porpuses random particles:
	_cpu_pf_rand_particles();

	// Add random reports:
	Particle testP; // The same particle from the documintation
	testP.pos =  glm::vec2(135,-300);
	testP.degree = DEG::fromDegree(45.0f);

	// Cant be more reports than the real POIs.
	std::vector<POIReport> _rps;
	for (int j=0;j<POIs_COUNT;j++) {
		_rps.push_back(
				_cpu_generateReportNu(
						j,
						testP,
						3.2f,
						RAD::fromDegree(5)
				)
				);
	}
	_cpu_pf_set_reports(_rps);


	// Add random motor
	movementInfo.x = randRange(0,10); // distance is >0
	movementInfo.y = randRange(-M_PI,M_PI)/18; // +-10deg
}

/*

void _cpu_bench() {

}
*/
