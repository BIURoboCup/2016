#version 330

uniform sampler2D tex0;

in  vec2 uv;
out vec4 color;

float data = 26.3337;

float proc(float x) {
	return exp(-x);
}

vec4 shipToCpu(float x) {
	// Convert bitwise to IEEE float:
	uint fconv =  floatBitsToUint (proc(data));
	
	// Unpack for 4 different bytes and store in RGBA
	vec4 unpack;
	unpack.a = ((fconv & uint(0xff000000)) >> 24);
	unpack.b = ((fconv & uint(0x00ff0000)) >> 16);
	unpack.g = ((fconv & uint(0x0000ff00)) >> 8);
	unpack.r = ((fconv & uint(0x000000ff)) >> 0);
	
	// Return normilized (glsl requirment):
	return unpack/255.0f;
}

void main () {

float fin = proc(data);
color = shipToCpu(fin);

}


