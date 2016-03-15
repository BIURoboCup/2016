#include "Vision/Vision.h"
#include "Motion/Motion.h"

void Debug()
{
	cout << "A";
	int a;
	cin >> a;
}


int main(int argc, char* argv[])
{
	Motion::GetInstance()->StartEngines();



	//RunVision();

	char key;
	cout << "Do you want to free all the engines? [y/n]" << endl;
	cin >> key;
	if (key == 'y' || key == 'Y') {
		Motion::GetInstance()->FreeAllEngines();
	}

	return 0;
}

void RunBrainThread()
{
	pthread_t visionThread;

	if(1 == pthread_create(&visionThread, NULL, BrainActionAsync, NULL))
	{
		fprintf(stderr, "Couldn't create Vision thread\n");
		exit(1);
	}
}

void* BrainActionAsync(void*)
{


}



