/*
 * DebugMode.h
 *
 *  Created on: Dec 30, 2011
 *      Author: isri
 */

#ifndef DEBUGMODE_H_
#define DEBUGMODE_H_

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <iostream>
#include <termios.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>


#include "LinuxDARwIn.h"
#include "General/MyThreadClass.h"
#include "IncludesFile.h"
#include "Vision/header/Vision.h"
#include "Motion/GoTo.h"

char mygetch();

class DebugMode : public MyThreadClass
{
private:

	static bool m_isDebug;
//	char static mygetch();
public:
	static DebugMode* _uniqueInstance;

private:
	Localization* m_localization;
	DebugMode()
	{
		m_localization = Localization::GetInstance();
	}
	void  thread_handler(int sig);
	void* check_thread(void *pnt);

public:
	static DebugMode* GetInstance();
	//starting the demo mode
	void InternalThreadFunc();
	void PrintStatus(WalkingAndActions *walkingAndActions);

};

#endif /* DEBUGMODE_H_ */
