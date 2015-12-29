#include <iostream>
#include <stdio.h>

// Manage threads and signals:
#include <pthread.h>
#include <signal.h>

#include <unistd.h>


// OpenCV Includes:
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "gl.hpp"
#include "gl_pf.hpp"
#include "cpu_pf.hpp"

#include "POIdata.hpp"
#include "timer.hpp"


using namespace std;
using namespace cv;
//using namespace cv::cuda;

struct DrawData
{
    ogl::Arrays arr;
    ogl::Texture2D tex;
    ogl::Buffer indices;
};

int main(int argc, char* argv[])
{

	int error=0;
	pthread_t t_opengl;
	openglInitData _data = {&argc, argv};
	if((error=pthread_create(&t_opengl, NULL , _glf_Main , &_data)))
	{
		cout << "cant create opengl thread! Error code: " << error << endl;
		return 0;
	}

	/*
	_cpu_pf_init_test();

	pf_Timer _oc;
	int _debug_perfomance=0;
	while(_debug_perfomance++ < 2000) {
		//sleep(1);
		_oc.start();
		_cpu_pf_main();
		std::cout << _debug_perfomance <<  "," << _oc.nanostop() << endl;
		std::cout << system("ps auxf | grep \"GpuParticleFilterPortable\" | awk -F\" \" '{if(NR==1)print $3 \" , \" }'") ;
	}
	exit(0);
	*/

	// to Prove that normal opencv works:
	cv::namedWindow("ME",WINDOW_OPENGL);
	Mat img = imread("lena.jpg");
	imshow("ME",img);

	// Glut run in a different thread so we need somthing
	// to prevent closing the main thread.
	char q=' ';
	while( q != 'q') {
			q = waitKey(30);
	}


	cout << "Ending Main" << endl;

  return 0;

}
