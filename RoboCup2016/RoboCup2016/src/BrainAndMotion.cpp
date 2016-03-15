/*
 * Motion.cpp
 *
 *  Created on: Sep 27, 2015
 *      Author: root
 */

#include "BrainAndMotion.h"

/*****************************************************************************************
* Method Name: GetTilt
* Description:
* Arguments: None
* Return Values: none
* Wrote: Gilad & Roman
*****************************************************************************************/
float GetTilt()
{
	float tiltAngle,panAngle;
	tiltAngle = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_TILT);
	panAngle =  MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	printf("tilt = %d  pan = %d\n",(int)tiltAngle,(int)panAngle);
	return ( tiltAngle /180 * M_PI_2 + M_PI_2 );
}

/*****************************************************************************************
* Method Name: SetTilt
* Description:
* Arguments: None
* Return Values: none
* Wrote: Gilad & Roman
*****************************************************************************************/
void SetTilt(int headPan_, int headTilt_ )
{
	Head::GetInstance()->MoveByAngle(headPan_,headTilt_);
	while(Action::GetInstance()->IsRunning()) usleep(8*1000);
}

/*****************************************************************************************
* Method Name: printenable
* Description:
* Arguments: None
* Return Values: None
* Wrote: Gilad & Roman
*****************************************************************************************/
void printenable()
{
	JointData joint;

	printf("%s  %d\n", "ID_R_HIP_YAW" ,(int)joint.GetEnable(JointData::ID_R_HIP_YAW) );
	printf("%s  %d\n", "ID_R_HIP_ROLL" , (int)joint.GetEnable(JointData::ID_R_HIP_ROLL));
	printf("%s  %d\n", "ID_R_HIP_PITCH",(int)joint.GetEnable(JointData::ID_R_HIP_PITCH));
	printf("%s  %d\n", "ID_R_KNEE",(int)joint.GetEnable(JointData::ID_R_KNEE));
	printf("%s  %d\n", "ID_R_ANKLE_PITCH",(int)joint.GetEnable(JointData::ID_R_ANKLE_PITCH));
	printf("%s  %d\n", "ID_R_ANKLE_ROLL",(int)joint.GetEnable(JointData::ID_R_ANKLE_ROLL));
	printf("%s  %d\n", "ID_L_HIP_ROLL",(int)joint.GetEnable(JointData::ID_L_HIP_ROLL));
	printf("%s  %d\n", "ID_L_HIP_PITCH",(int)joint.GetEnable(JointData::ID_L_HIP_PITCH));
	printf("%s  %d\n", "ID_L_KNEE",(int)joint.GetEnable(JointData::ID_L_KNEE));
	printf("%s  %d\n", "ID_L_ANKLE_ROLL",(int)joint.GetEnable(JointData::ID_L_ANKLE_ROLL));
	printf("%s  %d\n", "ID_R_SHOULDER_PITCH",(int)joint.GetEnable(JointData::ID_R_SHOULDER_PITCH));
	printf("%s  %d\n", "ID_L_SHOULDER_PITCH",(int)joint.GetEnable(JointData::ID_L_SHOULDER_PITCH));
	printf("%s  %d\n", "ID_HEAD_PAN",(int)joint.GetEnable(JointData::ID_HEAD_PAN));
}

/*****************************************************************************************
* Method Name: Reset
* Description:
* Arguments: None
* Return Values: None
* Wrote: Gilad & Roman
*****************************************************************************************/
void Reset(Robot::CM730 *cm730, int id)
{
	int FailCount = 0;
	int FailMaxCount = 10;
	printf(" Reset ID:%d...", id);

	if(cm730->Ping(id, 0) != CM730::SUCCESS)
	{
		printf("Fail\n");
		return;
	}

	FailCount = 0;
	while(1)
	{
		if(cm730->WriteByte(id, MX28::P_RETURN_DELAY_TIME, 0, 0) == CM730::SUCCESS)
			break;

		FailCount++;
		if(FailCount > FailMaxCount)
		{
			printf("Fail\n");
			return;
		}
		usleep(10000);
	}

	FailCount = 0;
	while(1)
	{
		if(cm730->WriteByte(id, MX28::P_RETURN_LEVEL, 2, 0) == CM730::SUCCESS)
			break;

		FailCount++;
		if(FailCount > FailMaxCount)
		{
			printf("Fail\n");
			return;
		}
		usleep(10000);
	}

	if(id != CM730::ID_CM)
	{
		double cwLimit = MX28::MIN_ANGLE;
		double ccwLimit = MX28::MAX_ANGLE;

		switch(id)
		{
		case JointData::ID_R_SHOULDER_ROLL:
			cwLimit = -75.0;
			ccwLimit = 135.0;
			break;

		case JointData::ID_L_SHOULDER_ROLL:
			cwLimit = -135.0;
			ccwLimit = 75.0;
			break;

		case JointData::ID_R_ELBOW:
			cwLimit = -95.0;
			ccwLimit = 70.0;
			break;

		case JointData::ID_L_ELBOW:
			cwLimit = -70.0;
			ccwLimit = 95.0;
			break;

		case JointData::ID_R_HIP_YAW:
            cwLimit = -30.0;
            ccwLimit = 30.0;
            break;

		case JointData::ID_L_HIP_YAW:
			cwLimit = -30.0;
			ccwLimit = 30.0;
			break;

		case JointData::ID_R_HIP_ROLL:
			cwLimit = -45.0;
			ccwLimit = 45.0;
			break;

		case JointData::ID_L_HIP_ROLL:
			cwLimit = -45.0;
			ccwLimit = 45.0;
			break;

		case JointData::ID_R_HIP_PITCH:
			cwLimit = -90.0;
			ccwLimit = 90.0;//29.0;
			break;

		case JointData::ID_L_HIP_PITCH:
			cwLimit = -90.0;//-29.0;
			ccwLimit = 90.0;
			break;

		case JointData::ID_R_KNEE:
			cwLimit = -160.0;
			ccwLimit = 160.0;
			break;

		case JointData::ID_L_KNEE:
			cwLimit = -160.0;
			ccwLimit = 160.0;
			break;

		case JointData::ID_R_ANKLE_PITCH:
			cwLimit = -90.0;
			ccwLimit = 90.0;
			break;

		case JointData::ID_L_ANKLE_PITCH:
			cwLimit = -90.0;
			ccwLimit = 90.0;
			break;

		case JointData::ID_R_ANKLE_ROLL:
			cwLimit = -45.0;
			ccwLimit = 45.0;
			break;

		case JointData::ID_L_ANKLE_ROLL:
			cwLimit = -45.0;
			ccwLimit = 45.0;
			break;

		case JointData::ID_HEAD_TILT:
			cwLimit = -45.0;
			ccwLimit = 45.0;
			break;


		}

		FailCount = 0;
		while(1)
		{
			if(cm730->WriteWord(id, MX28::P_CW_ANGLE_LIMIT_L, MX28::Angle2Value(cwLimit), 0) == CM730::SUCCESS)
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
		FailCount = 0;
		while(1)
		{
			if(cm730->WriteWord(id, MX28::P_CCW_ANGLE_LIMIT_L, MX28::Angle2Value(ccwLimit), 0) == CM730::SUCCESS)
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
		FailCount = 0;
		while(1)
		{
			if(cm730->WriteByte(id, MX28::P_HIGH_LIMIT_TEMPERATURE, 80, 0) == CM730::SUCCESS)
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
		FailCount = 0;
		while(1)
		{
			if(cm730->WriteByte(id, MX28::P_LOW_LIMIT_VOLTAGE, 60, 0) == CM730::SUCCESS)
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
		FailCount = 0;
		while(1)
		{
			if(cm730->WriteByte(id, MX28::P_HIGH_LIMIT_VOLTAGE, 140, 0) == CM730::SUCCESS)
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
		FailCount = 0;
		while(1)
		{
			if(cm730->WriteWord(id, MX28::P_MAX_TORQUE_L, MX28::MAX_VALUE, 0) == CM730::SUCCESS)
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
		FailCount = 0;
		while(1)
		{
			if(cm730->WriteByte(id, MX28::P_ALARM_LED, 36, 0) == CM730::SUCCESS) // Overload, Overheat
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
		FailCount = 0;
		while(1)
		{
			if(cm730->WriteByte(id, MX28::P_ALARM_SHUTDOWN, 36, 0) == CM730::SUCCESS) // Overload, Overheat
				break;

			FailCount++;
			if(FailCount > FailMaxCount)
			{
				printf("Fail\n");
				return;
			}
			usleep(10000);
		}
	}

	printf("Success\n");
}


/*****************************************************************************************
* Method Name: printenable
* Description:
* Arguments: None
* Return Values: None
* Wrote: Gilad & Roman
*****************************************************************************************/
void format_time(){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    printf( "[%d:%d:%d]",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

}

/*****************************************************************************************
* Method Name: startLog
* Description: open log file for writing
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void startLog(ofstream& logFile, std::string logFilePath)
{
	  logFile.open (logFilePath.c_str());
}



/*****************************************************************************************
* Method Name: writeToLog
* Description: write text to specified logFile
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void writeToLog(ofstream& logFile, std::string text)
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    //printf( "[%d:%d:%d]",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	logFile << "[" << timeinfo->tm_hour<< ":"<< timeinfo->tm_min << ":" << timeinfo->tm_sec <<"]" << ":" << text << "\n";
}

/*****************************************************************************************
* Method Name: writeToLog
* Description: write text to specified logFile
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void writeToLog(ofstream& logFile, std::string text1, std::string text2)
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    std::string text = text1;
    text+=' ';
    text+=text2;

    //printf( "[%d:%d:%d]",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	logFile << "[" << timeinfo->tm_hour<< ":"<< timeinfo->tm_min << ":" << timeinfo->tm_sec <<"]" << ":" << text << "\n";
}

/*****************************************************************************************
* Method Name: writeToLog
* Description: write text to specified logFile
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void writeToLog(ofstream& logFile, std::string text1, std::string text2, double dir , int ballSpot)
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    std::string text = text1;
    text+=' ';
    text+=text2;

    //printf( "[%d:%d:%d]",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	logFile << "[" << timeinfo->tm_hour<< ":"<< timeinfo->tm_min << ":" << timeinfo->tm_sec <<"]"
			<< ":" << text <<" " <<"dir:" << dir <<" " << "ballSpot:" <<ballSpot <<"\n";
}

/*****************************************************************************************
* Method Name: endLog
* Description: close log file
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void endLog(ofstream& logFile)
{
	  logFile.close();
}



