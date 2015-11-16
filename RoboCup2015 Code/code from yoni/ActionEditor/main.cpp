/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <iostream>

#include "Camera.h"
#include "Point.h"
#include "mjpg_streamer.h"
#include "minIni.h"
#include "LinuxCamera.h"
#include "ColorFinder.h"

#include "Action.h"
#include "Head.h"
#include "Walking.h"
#include "MX28.h"
#include "MotionManager.h"
#include "LinuxMotionTimer.h"
#include "LinuxCM730.h"
#include "LinuxActionScript.h"

#ifdef MX28_1024
#define MOTION_FILE_PATH    "Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "Data/motion_4096.bin"
#endif
#define INI_FILE_PATH	   "configW.ini"	


#define DLOG(x) std::cout << x << std::endl << std::flush

void OnOffCmd(CM730 *cm730, bool on, int num_param, int *list)
{
	if(num_param == 0)
	{
		for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
			cm730->WriteByte(id, MX28::P_TORQUE_ENABLE, (int)on, 0);
	}
	else
	{
		for(int i=0; i<num_param; i++)
		{
			if(list[i] >= JointData::ID_R_SHOULDER_PITCH && list[i] <= JointData::ID_HEAD_TILT)
				cm730->WriteByte(list[i], MX28::P_TORQUE_ENABLE, (int)on, 0);
		}
	}

	//ReadStep(cm730);
	//DrawStep(7);
}

void change_current_dir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        chdir(dirname(exepath));
}



int main(void)
{
	system("pwd");
	DLOG("trying to open: `" << MOTION_FILE_PATH << "`");

    printf( "\n===== Action script for DARwIn =====\n\n");

    change_current_dir();
    Action::GetInstance()->LoadFile(MOTION_FILE_PATH);

    DLOG("Done motion");

    minIni* ini = new minIni(INI_FILE_PATH);

    DLOG("Done Ini");

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


    DLOG("Done Framework");
    /////////////////////////////////////////////////////////////////////

    Walking::GetInstance()->m_Joint.SetEnableBody(false);
    Head::GetInstance()->m_Joint.SetEnableBody(false);
    Action::GetInstance()->m_Joint.SetEnableBody(true);
    MotionManager::GetInstance()->SetEnable(true);
   

    Action::GetInstance()->Start(1);    /* Init(stand up) pose */

    Action::GetInstance()->Brake();
    while(Action::GetInstance()->IsRunning()) usleep(8*1000);


    DLOG("Done Base");

    printf("Press the ENTER key to begin!\n");
    getchar();

//////
printf("Pan Angle: %d\n",Head::GetInstance()->GetPanAngle());
printf("Tilt Angle: %d\n",Head::GetInstance()->GetTiltAngle()); 
printf("1-sit\n");
printf("2-stand\n");
printf("3-walking\n");
printf("4-stop walking\n");

bool onOff = true;
char* BOOL2STR[] = {"FASLE","TRUE"};

	while(1)
	{
		char c =getchar();
		if (c == '1')
			Action::GetInstance()->Start(15);
			//LinuxActionScript::ScriptStart("Sit.asc");
		else if(c == '2')
			LinuxActionScript::ScriptStart("Stand.asc");
		//while(LinuxActionScript::m_is_running == 1) sleep(10);
		//Action::GetInstance()->Brake();
		else if(c == '3')
		{
			if(Walking::GetInstance()->IsRunning() == false )
			{
				fprintf(stderr, "STARTING\n");		
				Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
				Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true,true);
				Walking::GetInstance()->X_MOVE_AMPLITUDE = 0;
				Walking::GetInstance()->Y_MOVE_AMPLITUDE = 20;
				Walking::GetInstance()->A_MOVE_AMPLITUDE = 0;
				Walking::GetInstance()->Start();			
			}
		}
		else if(c == '4')
		{
			if(Walking::GetInstance()->IsRunning() == true )
			{
				fprintf(stderr, "STOPPING\n");		
				Walking::GetInstance()->Stop();
				while(Walking::GetInstance()->IsRunning() == 1) usleep(8000);
			}
		}
		else if(c == '5') {
			if(Walking::GetInstance()->IsRunning() == false )
			{
				onOff = !onOff;
				DLOG("Change state to: " << BOOL2STR[onOff] );
				cm730.DXLPowerOn(onOff);
			}
		}
		else if(c == 'q')	
		{
			break;
		}
	}
///////

    //LinuxActionScript::ScriptStart("script.asc");
    //while(LinuxActionScript::m_is_running == 1) sleep(10);

    return 0;
}
