#ifndef GLPF_TOOLS
#define GLPF_TOOLS

#include "gl.hpp"

typedef struct {
	int* argc;
	char** argv;
} openglInitData;

// Tool for loading shader from text file.
GLuint initShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);

// For time benchmarking:
#define cycletime unsigned long long
// Cycles since the cpu is up:
static __inline__ cycletime rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

// Main particle filter thread:
void *_glf_Main(void* arg);

// particle size must be less than 4096 = MAX_UNIFORM..Components/sizeof(float)
//		beause we use Weights as uniform data in the shader.
#define PSIZE 4096


#endif
