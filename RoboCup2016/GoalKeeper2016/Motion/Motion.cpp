#include "Motion.h"
#include "MotionIncludes.h"

Motion::Motion()
{
	// TODO: initialize motion-related values.

	//minIni* ini = new minIni(INI_FILE_PATH);
	//Action::GetInstance()->LoadFile(MOTION_FILE_PATH);

	////////////////////// Framework Initialize ////////////////////////////
	//if (MotionManager::GetInstance()->Initialize(&cm730) == false)
	//{
	//	linux_cm730.SetPortName("/dev/ttyUSB1");
	//	if (MotionManager::GetInstance()->Initialize(&cm730) == false)
	//	{
	//		printf("Fail to initialize Motion Manager!\n");
	//		return 0;
	//	}
	//}
	//Walking::GetInstance()->LoadINISettings(ini);
	//MotionManager::GetInstance()->LoadINISettings(ini);
}

void Motion::WalkStraight(int cmToWalk)
{
	//MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_TILT);
}

void Motion::SetHeadTilt(S_HeadTilt headTilt)
{

}

S_HeadTilt Motion::GetHeadTilt()
{

}

bool Motion::IsRunning()
{

}