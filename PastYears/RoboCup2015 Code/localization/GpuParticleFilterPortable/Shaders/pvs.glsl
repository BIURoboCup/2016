#version 330

//*********************************************************
//			INPUT, OUTPUT
//*********************************************************

// The particles from last time, and it's size 
//	(which is the weight from last time)
// Particle has data (x,y,teta,length=weight)

layout(location = 0) in vec4 oldParticle;
layout(location = 1) in float oldWeight; 

// The new data:
//	1) Particle may move according to motor data.
//	2) Weight change due to new reports.
out vec4 outp;
out float outw;

// The flag to decide if we calculation weight or resampling:
uniform bool step;


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

uniform vec2 POIs[30]; // All the POIs (x,y)

// All the report\movment data.
// Since we must use constant arrays in glsl_3.3 we use upper
//		bound of 30, and we will pass the real count with `reportCount`;
uniform int 		reportCount;
uniform vec3 		reports[30];
uniform vec2		movementInfo;

//*********************************************************
//			Resampling new particles
//*********************************************************

// After calculation w_i we need to resample.
uniform int M;			// Actual number of particles (M <= 4096)
uniform float time;		// for randomness
uniform float a[4096]; 	// accumlated weight

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
float	d =  dcoeff *  abs(reportDistance -  distance(POIs[POIindex],p.xy));
float	t =  tcoeff *  abs(
					reportAngle  
					- atan(POIs[POIindex].y - p.y,POIs[POIindex].x - p.x)
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
	int _POIs[27] = int[] (0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26);
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
	int _POIs[12] = int[] (4,6,7,9,11,12,15,17,18,20,22,23);
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
	int _POIs[10] = int[] (1,2,5,8,10,13,16,19,21,24);
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
	int _POIs[2] = int[] (3,14);
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
	int _POIs[2] = int[] (25,26);
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
	int _POIs[1] = int[] (0);
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

    float target = (noise1(vec3(time,gl_VertexID,gl_InstanceID)) + 1.0f) * a[M-1];
    
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



void main(void)
{
	
	if (!step) {
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
		vec3 movedParticel = oldParticle.xyz; // old postion\angle data

		// Movment algo here.

		outp = vec4(movedParticel, oldParticle.w); // save old weight.
		outw = _w; // new weight

	}else {
		// resample
		


		outp = oldParticle;
		outw = oldWeight;
	}
	
}
