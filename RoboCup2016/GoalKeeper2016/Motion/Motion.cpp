#include "Motion.h"
#include "MotionIncludes.h"


Motion::Motion()
{
	m_linuxCM730 = new LinuxCM730("/dev/ttyUSB0");
	m_cm730 = new CM730(m_linuxCM730);

	const string INI_FILE_PATH = "/home/robot/Robot1/Data/config.ini";
	minIni* ini = new minIni(INI_FILE_PATH);
	Walking::GetInstance()->LoadINISettings(ini);
	MotionManager::GetInstance()->LoadINISettings(ini);

	// Motion config file
	Action::GetInstance()->LoadFile("/home/robot/Robot1/Data/motion_4096.bin");

	//////////////////// Framework Initialize ////////////////////////////
	if (MotionManager::GetInstance()->Initialize(m_cm730) == false)
	{
		m_linuxCM730->SetPortName("/dev/ttyUSB1");
		if (MotionManager::GetInstance()->Initialize(m_cm730) == false)
		{
			cout << "Fail to initialize Motion Manager!\n";
		}
	}

    MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());
    LinuxMotionTimer linuxMotionTimer;
    linuxMotionTimer.Initialize(MotionManager::GetInstance());
    linuxMotionTimer.Start();
    /////////////////////////////////////////////////////////////////////

    Walking::GetInstance()->m_Joint.SetEnableBody(false);
    Head::GetInstance()->m_Joint.SetEnableBody(false);
    Action::GetInstance()->m_Joint.SetEnableBody(true);
    MotionManager::GetInstance()->SetEnable(true);

    Action::GetInstance()->Start(1);     //Init(stand up) pose

    Action::GetInstance()->Brake();
    while(Action::GetInstance()->IsRunning()) usleep(8*1000);

    m_cm730->WriteByte(CM730::P_LED_PANNEL, 0x02, NULL);
    MotionManager::GetInstance()->ResetGyroCalibration();
}

Motion::~Motion()
{
	delete m_cm730;
	delete m_linuxCM730;
}

void Motion::WalkStraight(int cmToWalk)
{

}

void Motion::FreeAllEngines()
{
	m_cm730->WriteByte(CM730::ID_CM, MX28::P_TORQUE_ENABLE, 0, 0);
}

void Motion::SetHeadTilt(S_HeadTilt headTilt)
{
	Head::GetInstance()->MoveByAngle(headTilt.Pan,headTilt.Tilt);
	WaitForActionFinish();
}

S_HeadTilt Motion::GetHeadTilt()
{
	S_HeadTilt headTilt;
	headTilt.Tilt = MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_TILT);
	headTilt.Pan =  MotionStatus::m_CurrentJoints.GetAngle(JointData::ID_HEAD_PAN);
	return headTilt;
}

void Motion::WaitForActionFinish()
{
	int usecondsBetweenSamples = 8*1000;

	// Checks every 'usecondsBetweenSamples' if the action still running.
	while(IsRunning())
	{
		usleep(usecondsBetweenSamples);
	};
}

bool Motion::IsRunning()
{
	return Walking::GetInstance()->IsRunning();
}
