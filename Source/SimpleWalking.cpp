 /*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>

#include "../../build/streamer/mjpg_streamer.h"
#include "../../include/LinuxDARwIn.h"


/*
#ifdef MX28_1024
#define MOTION_FILE_PATH    ((char *)"../../../Data/motion_1024.bin")
#else
#define MOTION_FILE_PATH    ((char *)"../../../Data/motion_4096.bin")
#endif
*/

#ifdef MX28_1024
#define MOTION_FILE_PATH    "/home/robot/Desktop/Walk/Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "/home/robot/Desktop/Walk/Data/motion_4096.bin"
#endif

/*
#ifdef MX28_1024
#define MOTION_FILE_PATH    "/home/robot/Robot1/Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "/home/robot/Robot1/Data/motion_4096.bin"
#endif
*/
#define INI_FILE_PATH    "/home/robot/Robot1/Data/config.ini"


////////////////////////////////////////////
void walk(CM730 &cm730);
int change_current_dir()
{
    char exepath[1024] = {0};
    int status = 0;
		if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        status = chdir(dirname(exepath));
		return status;
}
////////////////////////////////////////////


int main()
{    
   change_current_dir();

    minIni* ini = new minIni(INI_FILE_PATH); 

    //////////////////// Framework Initialize ////////////////////////////

    LinuxCM730 linux_cm730("/dev/ttyUSB0");
    CM730 cm730(&linux_cm730);
    if(MotionManager::GetInstance()->Initialize(&cm730) == false)
    {
        linux_cm730.SetPortName("/dev/ttyUSB1");
        if(MotionManager::GetInstance()->Initialize(&cm730) == false)
        {
            printf("Fail to initialize Motion Manager!\n");
            return 0;
        }
    }

    Walking::GetInstance()->LoadINISettings(ini);
	usleep(100);
    MotionManager::GetInstance()->LoadINISettings(ini);

    MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());

    LinuxMotionTimer linuxMotionTimer;
    linuxMotionTimer.Initialize(MotionManager::GetInstance());
    linuxMotionTimer.Start();
  

    Action::GetInstance()->LoadFile(MOTION_FILE_PATH);


    Walking::GetInstance()->LoadINISettings(ini);   
    MotionManager::GetInstance()->LoadINISettings(ini); 

    Walking::GetInstance()->m_Joint.SetEnableBody(false);
    Head::GetInstance()->m_Joint.SetEnableBody(false);
    Action::GetInstance()->m_Joint.SetEnableBody(true);
    MotionManager::GetInstance()->SetEnable(true);
              

    cm730.WriteByte(CM730::P_LED_PANNEL, 0x02, NULL);

    //PS3Controller_Start();
    Action::GetInstance()->Start(1);
    while(Action::GetInstance()->IsRunning()) usleep(8*1000);

    while(1)
    {
      walk(cm730);	
    }

    return 0;
}


void walk(CM730 &cm730)
{
	if(Walking::GetInstance()->IsRunning() == false)
	{
		fprintf(stderr, "STARTING\n");		
		//Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
		Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true,true);
		Walking::GetInstance()->X_MOVE_AMPLITUDE = 0;
		Walking::GetInstance()->Y_MOVE_AMPLITUDE = 0;
		Walking::GetInstance()->A_MOVE_AMPLITUDE = 0;
		Walking::GetInstance()->Start();			
	}
	
	/*
	if(Walking::GetInstance()->IsRunning() == true && PS3.key.Down != 0)
	{
		fprintf(stderr, "STOPPING\n");		
		Walking::GetInstance()->Stop();
		while(Walking::GetInstance()->IsRunning() == 1) usleep(8000);
		Walking::GetInstance()->m_Joint.SetEnableBody(false);
		Head::GetInstance()->m_Joint.SetEnableBody(false);
		Action::GetInstance()->m_Joint.SetEnableBody(true);
		MotionManager::GetInstance()->SetEnable(true);
	}*/

}

