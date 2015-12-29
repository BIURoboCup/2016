/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include "DebugMode.h"

bool DebugMode::m_isDebug = true;

char mygetch()
{
  struct termios oldt,
				 newt;
  int            ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}

DebugMode* DebugMode::_uniqueInstance = 0;

DebugMode* DebugMode::GetInstance() {
	if (_uniqueInstance == 0)	{
		_uniqueInstance = new DebugMode();
	}
	return _uniqueInstance;
}


void DebugMode::InternalThreadFunc()
{
	XYLocation_t point;
	GoTo* goTo = GoTo::GetInstance();
	Localization* loc;
	loc=Localization::GetInstance();
	Vision* vis;
	vis=Vision::GetInstance();

	//get an instance to the motion module
	//cout << "[DebugMode::InternalThreadFunc]: before WalkingAndActions::GetInstance()" << endl;
	WalkingAndActions *walkingAndActions = WalkingAndActions::GetInstance();
	//cout << "[DebugMode::InternalThreadFuncafter]: WalkingAndActions::GetInstance()" << endl;

	walkingAndActions->StartAction(STAND_UP);
	int field=0;
	char input;
	while(exitProgram == false)
	{
		//clear the screen before printing the current status.
		if(m_isDebug)
		{
			//system("clear");
			cout<<endl;
			PrintStatus(walkingAndActions);
		}

		//get an input from the user
		input = mygetch();

		//if quit get out
		if(input == 'q' || input == 'Q')
			break;

		//start walking
		switch(input)
		{
		case 's':
		case 'S':
			cout << "Stand Up" << endl;
			//move head to home position
			walkingAndActions->ResetHeadParams();
			//stop walking and stand up
			walkingAndActions->StopWalking();
			walkingAndActions->StartAction(STAND_UP);
		break;


		case 'i':
		case 'I':
			//increase X direction speed by 1.
			walkingAndActions->ChangeMotionValue(X_SPEED, 1);
		break;

		case 'k':
		case 'K':
			//decrease X direction speed by 1
			walkingAndActions->ChangeMotionValue(X_SPEED, -1);
		break;


		case 'l':
		case 'L':
			//decrease turn angle by 1
			walkingAndActions->ChangeMotionValue(ROTATION_ANGLE, -1);
		break;

		case 'j':
		case 'J':
			//increase turn angle by 1
			walkingAndActions->ChangeMotionValue(ROTATION_ANGLE, 1);
		break;


		case 'x':
		case 'X':
			//increase Y direction speed by 1.
			walkingAndActions->ChangeMotionValue(Y_SPEED, 1);
		break;

		case 'c':
		case 'C':
			//decrease Y direction speed by 1
			walkingAndActions->ChangeMotionValue(Y_SPEED, -1);
		break;

		case 'r':
		case 'R':
			//start walking with walking mode
			cout << "Start walking" << endl;
			walkingAndActions->SetWalkingMode(RUNNING);
			walkingAndActions->StartWalking(0,0,0);
		break;

		case 't':
		case 'T':
			//start walking with walking mode
			cout << "Start running" << endl;
			walkingAndActions->SetWalkingMode(RUNNING);
			walkingAndActions->StartWalking(0,0,0);
		break;

		case '1':
			//Kick left foot
			cout << "Kicking left" << endl;
			walkingAndActions->StopWalking();
			//cout << "angle from me: " << m_localization->GetBallLocation().angleFromMe << "\n";
			walkingAndActions->StartAction(KICK_LEFT);
		break;

		case '2':
			//Kick right foot
			cout << "Kicking right" << endl;
			walkingAndActions->StopWalking();
//			cout << "angle from me: " << m_localization->GetBallLocation().angleFromMe << "\n";
			walkingAndActions->StartAction(KICK_RIGHT);
		break;

		case '3':
			//Pass left foot
			cout << "Kicking left" << endl;
			walkingAndActions->StopWalking();
			walkingAndActions->StartAction(PASS_LEFT);
		break;

		case '4':
			//Pass right foot
			cout << "Kicking right" << endl;
			walkingAndActions->StopWalking();
			walkingAndActions->StartAction(PASS_RIGHT);
		break;

		case '5':
			//Falsh left foot
			cout << "Falsh left" << endl;
			walkingAndActions->StopWalking();
			walkingAndActions->StartAction(FALSH_LEFT);
		break;

		case '6':
			//Falsh right foot
			cout << "Falsh right" << endl;
			walkingAndActions->StopWalking();
			walkingAndActions->StartAction(FALSH_RIGHT);
		break;

		case 'n':
		case 'N':
			//increase Y head angle direction speed by 1.
			walkingAndActions->ChangeHeadAngleValue(X_ANGLE, -1);
		break;

		case 'v':
		case 'V':
			//decrease Y head angle direction speed by 1.
			walkingAndActions->ChangeHeadAngleValue(X_ANGLE, 1);
		break;

		case 'g':
		case 'G':
			//increase X head angle direction speed by 1.
			walkingAndActions->ChangeHeadAngleValue(Y_ANGLE, 1);
		break;

		case 'b':
		case 'B':
			//decrease X head angle direction speed by 1.
			walkingAndActions->ChangeHeadAngleValue(Y_ANGLE, -1);
		break;

		case '0'://localization checks
			point.x = 150;
			point.y = 450;

			vis->SetHeadTrackingStatus(UNKNOWN_TRACKING_STATUS);
			vis->m_head.MoveByAngle(0,55);
			break;

		case 'o':
			while(!goTo->QuickAndDirty(0,20,0))
				usleep(1000*3);
			//while (!goTo->quickAndDirty(50,300,-30))
				//sleep(1);
//			cout<<"start rain dance!"<<endl;
//			walkingAndActions->SetWalkingMode(WALKING);
//			walkingAndActions->StartWalkingAsync(-3, 20, -11);
//			walkingAndActions->StartWalkingAsync(-10,0,0);
			break;

		case '`':
			while(!goTo->QuickAndDirty(100,100))
				usleep(1000*3);
			printf("\n\nreached 100 100 \n\n\n");
			while(!goTo->QuickAndDirty(100,500))
				usleep(1000*3);
			printf("\n\nreached 100 500 \n\n\n");
			while(!goTo->QuickAndDirty(-100,500))
				usleep(1000*3);
			printf("\n\nreached -100 500 \n\n\n");
			while(!goTo->QuickAndDirty(-100,100))
				usleep(1000*3);
			printf("\n\nreached -100 100 \n\n\n");
			break;
		case 'z':
			printf("goto x y test\n");
			printf("insert x y\n");
			cin>>point.x>>point.y;
			while(!goTo->QuickAndDirty(point.x,point.y))
				usleep(1000*3);

			break;
		case 'm':
			m_localization->ReEnterFiled();
			break;
		case 27:
			if (vis->GetHeadTrackingStatus() == BALL_TRACKING)
			{
				vis->SetHeadTrackingStatus(GOAL_TRACKING);
				cout<<"HeadTrackingStatus =  GOAL_TRACKING\n";
			}

			else if (vis->GetHeadTrackingStatus() == GOAL_TRACKING)
			{
				vis->SetHeadTrackingStatus(BALL_AND_GOAL_TRACKING);
				cout<<"HeadTrackingStatus =  BALL_AND_GOAL_TRACKING\n";
			}
			else if (vis->GetHeadTrackingStatus() == BALL_AND_GOAL_TRACKING)
			{
				vis->SetHeadTrackingStatus(KICKING_BALL_TRACKING);
				cout<<"HeadTrackingStatus =  KICKING_BALL_TRACKING\n";
			}
			else if (vis->GetHeadTrackingStatus() == KICKING_BALL_TRACKING)
			{
				vis->SetHeadTrackingStatus(BALL_TRACKING);
				cout<<"HeadTrackingStatus =  BALL_TRACKING\n";
			}
			break;
		default:
			cout << "No such key!" << endl;
			break;
		}

		usleep(250*1000);
	}
	walkingAndActions->StopWalking();
	walkingAndActions->StartAction(SIT_DOWN);

	cout << "Debug Mode Thread exited\n";
}

void DebugMode::PrintStatus(WalkingAndActions *walkingAndActions)
{
	Location myLoc=Localization::GetInstance()->GetMyLocation();

	cout << "Walking mode is: " << walkingAndActions->GetWalkingModeByString() << endl;
	cout << "X speed is: " << walkingAndActions->GetMotionValue(X_SPEED) << endl;
	cout << "Y speed is: " << walkingAndActions->GetMotionValue(Y_SPEED) << endl;
	cout << "Angle is: " << walkingAndActions->GetMotionValue(ROTATION_ANGLE) << endl;
	cout << "X head angle is: " << walkingAndActions->GetHeadAngleValue(X_ANGLE) << endl;
	cout << "Y head angle is: " << walkingAndActions->GetHeadAngleValue(Y_ANGLE) << endl;
	cout <<"["<<myLoc.x<<","<<myLoc.y<<","<<myLoc.angle<<"]"<<endl;
}
