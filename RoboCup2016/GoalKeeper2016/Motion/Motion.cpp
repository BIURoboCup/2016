#include "Motion.h"
#include "MotionIncludes.h"

Motion* Motion::m_instance = NULL;
Motion* Motion::GetInstance()
{
    if(m_instance == NULL)
    {
    	m_instance = new Motion();
        return m_instance;
    }
    else
    {
        return m_instance;
    }
}

void ChangeCurrentDir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        chdir(dirname(exepath));
}

Motion::Motion()
{
}

LinuxCM730 m_linuxCM730("/dev/ttyUSB0");
CM730 m_cm730(&m_linuxCM730);
LinuxMotionTimer linuxMotionTimer;

void Motion::StartEngines()
{
	printf("===== Action script for DARwIn =====\n\n");
	ChangeCurrentDir();
	minIni* ini = new minIni(INI_FILE_PATH);
	Action::GetInstance()->LoadFile(MOTION_FILE_PATH);

	//////////////////// Framework Initialize ////////////////////////////
	if (MotionManager::GetInstance()->Initialize(&m_cm730) == false) {
		m_linuxCM730.SetPortName("/dev/ttyUSB1");
		if (MotionManager::GetInstance()->Initialize(&m_cm730) == false) {
			printf("Fail to initialize Motion Manager!\n");
		}
	}
	Walking::GetInstance()->LoadINISettings(ini);
	usleep(100);
	MotionManager::GetInstance()->LoadINISettings(ini);

	MotionManager::GetInstance()->AddModule(
			(MotionModule*) Action::GetInstance());
	MotionManager::GetInstance()->AddModule(
			(MotionModule*) Head::GetInstance());
	MotionManager::GetInstance()->AddModule(
			(MotionModule*) Walking::GetInstance());

	linuxMotionTimer.Initialize(MotionManager::GetInstance());
	linuxMotionTimer.Start();
	/////////////////////////////////////////////////////////////////////

	Walking::GetInstance()->m_Joint.SetEnableBody(false);
	Head::GetInstance()->m_Joint.SetEnableBody(false);
	Action::GetInstance()->m_Joint.SetEnableBody(true);
	MotionManager::GetInstance()->SetEnable(true);

	Action::GetInstance()->Start(1);     //Init(stand up) pose

	Action::GetInstance()->Brake();
	while (Action::GetInstance()->IsRunning())
		usleep(8 * 1000);LinuxMotionTimer linuxMotionTimer;

	m_cm730.WriteByte(CM730::P_LED_PANNEL, 0x02, NULL);
	MotionManager::GetInstance()->ResetGyroCalibration();

	Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
}

Motion::~Motion()
{

}

void Motion::WalkStraight(int cmToWalk)
{

}

void Motion::FreeAllEngines()
{
	m_cm730.WriteByte(CM730::ID_CM, MX28::P_TORQUE_ENABLE, 0, 0);
}

void Motion::SetHeadTilt(HeadTilt headTilt)
{
	Head::GetInstance()->MoveByAngle(headTilt.Pan, headTilt.Tilt);
	WaitForActionFinish();
}

HeadTilt Motion::GetHeadTilt()
{
	float tilt = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_TILT);
	float pan =  MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	return HeadTilt(tilt, pan);
}

void Motion::WaitForActionFinish()
{
	int usecondsBetweenSamples = 8*1000;

	// Checks every 'usecondsBetweenSamples' if the action still running.
	while(IsActionRunning())
	{
		usleep(usecondsBetweenSamples);
	};
}

bool Motion::IsActionRunning()
{
	return Action::GetInstance()->IsRunning();
}
