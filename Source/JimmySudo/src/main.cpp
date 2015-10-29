/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include "BrainAndMotion.h"


pthread_t VisionT = 0;
bool runthr= true;
void* VisionThread(void*);

S_StateStruct StateList[] =
{
		{	NOP 	 		, "NOP"			},
		{	Init 	 		, "Init"		},
		{	Stop  	 		, "Stop"		},
		{	GoToBall 		, "GoToBall"	},
		{	KickBall 		, "KickBall"	},
		{	LookForBall		, "LookForBall" },
		{	FindGate 		, "FindGate"	},
		{	ChangeSpot		, "ChangeSpot"	},
		{	GoForKick		, "GoForKick"	},
		{	EXIT			, "EXIT"		},
};

LinuxCM730 linux_cm730("/dev/ttyUSB0");
CM730 cm730(&linux_cm730);
ofstream logFile;
char playing = 'y';
Ball ball;
sem_t ball_sem;
S_BrainBall BrainBall;
E_State Robotstate = NOP;

void change_current_dir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        chdir(dirname(exepath));
}


int main(void)
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    stringstream p;
    p<<"[" << timeinfo->tm_hour<< ":"<< timeinfo->tm_min << ":" << timeinfo->tm_sec <<"]";
	std::string logname = "/home/robot/Desktop/LOG";
	logname+=p.str();
    startLog(logFile,logname);

	if (sem_init(&ball_sem, 0, 1) != 0)
	{
		printf("Couldn't sem_init\n");
	}
	for (int i=0; i<BrainBall.x_detail ;i++)
	{
		BrainBall.x_place[i] = 0;
	}



    printf( "\n===== Action script for DARwIn =====\n\n");
    change_current_dir();
    minIni* ini = new minIni(INI_FILE_PATH);
    Action::GetInstance()->LoadFile(MOTION_FILE_PATH);

    //////////////////// Framework Initialize ////////////////////////////
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
    /////////////////////////////////////////////////////////////////////

    Walking::GetInstance()->m_Joint.SetEnableBody(false);
    Head::GetInstance()->m_Joint.SetEnableBody(false);
    Action::GetInstance()->m_Joint.SetEnableBody(true);
    MotionManager::GetInstance()->SetEnable(true);

    Action::GetInstance()->Start(1);     //Init(stand up) pose

    Action::GetInstance()->Brake();
    while(Action::GetInstance()->IsRunning()) usleep(8*1000);

    cm730.WriteByte(CM730::P_LED_PANNEL, 0x02, NULL);
   MotionManager::GetInstance()->ResetGyroCalibration();

   pthread_create(&VisionT, NULL, VisionThread,(void *) NULL);

    printf("Press the ENTER key to begin!\n");
    getchar();

//////
    writeToLog(logFile,"started");

printf("0-Play\n");
printf("1-Kick\n");
printf("2-stand\n");
printf("3-Straight walking\n");
printf("4-Turn right\n");
printf("5-Turn left\n");
printf("6-stop walking\n");
printf("7-reset\n");
printf("8-GetTilt\n");
printf("9-SetTilt\n");
printf("q-Exit\n");

Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);

	while(1)
	{
		char c =getchar();

		if (c == '0')
		{
			play();
		}
		else if (c == '1')
		{
			writeToLog(logFile,"kick");
			Action::GetInstance()->Start(2);
			while(Action::GetInstance()->IsRunning()) usleep(8*1000);
		}
		else if(c == '2')
		{
			writeToLog(logFile,"stand");
			Action::GetInstance()->Start(1);
			while(Action::GetInstance()->IsRunning()) usleep(8*1000);
		}
		else if(c == '3')
		{
			if(Walking::GetInstance()->IsRunning() == false )
			{
				fprintf(stderr, "STARTING\n");
				Action::GetInstance()->Start(9);
				while(Action::GetInstance()->IsRunning()) usleep(8*1000);
				//Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
				Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true,true);
				Walking::GetInstance()->X_MOVE_AMPLITUDE = 0;
				Walking::GetInstance()->Y_MOVE_AMPLITUDE = 0;
				Walking::GetInstance()->A_MOVE_AMPLITUDE = 0;
				Walking::GetInstance()->Start();
			}
			else
				Walking::GetInstance()->A_MOVE_AMPLITUDE = 0;
		}
		else if(c == '4')
		{
			if(Walking::GetInstance()->IsRunning() == true )
			{
				fprintf(stderr, "TURNING RIGHT\n");
				Walking::GetInstance()->A_MOVE_AMPLITUDE = -5;

			}
		}
		else if(c == '5')
		{
			if(Walking::GetInstance()->IsRunning() == true )
			{
				fprintf(stderr, "TURNING LEFT\n");
				Walking::GetInstance()->A_MOVE_AMPLITUDE = 5;
			}
		}
		else if(c == '6')
		{
			if(Walking::GetInstance()->IsRunning() == true )
			{
				fprintf(stderr, "STOPPING\n");		
				Walking::GetInstance()->Stop();
				while(Walking::GetInstance()->IsRunning() == 1) usleep(8000);
				Walking::GetInstance()->m_Joint.SetEnableBody(false);
				Head::GetInstance()->m_Joint.SetEnableBody(false);
				Action::GetInstance()->m_Joint.SetEnableBody(true);
				MotionManager::GetInstance()->SetEnable(true);
			}
		}
		else if(c == '7')
		{
			for(int i=JointData::ID_R_SHOULDER_PITCH; i<JointData::NUMBER_OF_JOINTS; i++)
											Reset(&cm730, i);

									Reset(&cm730, CM730::ID_CM);
		}
		else if(c == '8')
		{

			float tilt;
			tilt = GetTilt();
			printf("%f\n", tilt);

		}
		else if(c == '9')
		{
			float tilt,pan;
			cout<<"enter tilt\n";
			cin>>tilt;
			cout<<"enter pan\n";
			cin>>pan;
			SetTilt(pan,tilt);
		}
		else if(c == 'q')	
		{
			break;
		}
	}

	runthr=false;
	writeToLog(logFile,"normal exit");
	endLog(logFile);
	pthread_join(VisionT, NULL);
	if (sem_destroy(&ball_sem) != 0)
	{
		printf("Couldn't sem_destroy\n");
	}

    return 0;
}


void* VisionThread(void*)
{
	Vision vis;
    vis.OpenCamera();
    vis.CreateStreamWindow();
    S_BrainBall BrainBallCounter;

	for (int i=0; i<BrainBall.x_detail ;i++)
	{
		BrainBallCounter.x_place[i] = 0;
	}

    Mat m;
    sleep(2);

    while (runthr)
    {
    	vis.GetImage(m);
    	if( vis.key == 'q')
    			playing = vis.key;

    	if (vis.m_load_file)
    	{
    		vis.LoadColorsFromFile();
    	}
    	else if (vis.m_save_file)
    	{
    		vis.saveHSVtofile();
    	}

    	if (vis.m_calibrate && vis.m_calib_open)
    	{
    		vis.createGreen();
    		vis.m_calib_open=0;
    		vis.m_calib_close=1;
    	}
    	else if (vis.m_calibrate)
    	{
    		vis.editColorRange(m);
    		vis.showGreen(m);
    	}
    	else if (!vis.m_calibrate && vis.m_calib_close)
    	{
    		vis.closeGreen();
    		vis.m_calib_open=1;
    		vis.m_calib_close=0;
    	}


    	vis.motionTilt= GetTilt();


//////////////////////////////////////////////////////////////////////////////


    	ball = vis.Find_Ball(m);

    	//updating machine learn array
    	int xIncreasePlace = ((int)ball.center.x)/65;
    	if (BrainBallCounter.x_place[xIncreasePlace] < 20 )
    	{
    		if (ball.radius > 10)
    			BrainBallCounter.x_place[xIncreasePlace]+=2;  //find ball x array place and increase the spot value
    		else if (BrainBallCounter.x_place[xIncreasePlace] > 0)
    			BrainBallCounter.x_place[xIncreasePlace]--;
    	}


    	for (int i=0; i <BrainBallCounter.x_detail;i++)
    	{
    		if (i!=xIncreasePlace && BrainBallCounter.x_place[i]>0)
    			BrainBallCounter.x_place[i]--;
    	}

    	//updating ball location
    	int max = 0;
    	bool present = false;

    	for (int i=0; i <BrainBallCounter.x_detail;i++)
    	{
    		if (BrainBallCounter.x_place[i]>10)
    		{
    			if (present == true && BrainBallCounter.x_place[max] < BrainBallCounter.x_place[i])
    			{
    				max=i;
    			}
    			else if (present==false)
    			{
    				max=i;
    				present=true;
    			}
    		}
    	}

    	/////////////////////////////////////////////
    	if (sem_wait(&ball_sem) != 0)
    		printf("Couldn't sem_wait\n");

    	for (int i=0; i<BrainBall.x_detail ;i++)
    	{
    		BrainBall.x_place[i] = 0;
    	}

    	if (present)
    	{
    		BrainBall.x_place[max] = 1;
    		BrainBall.found= true;
    		BrainBall.radius= ball.radius;
    		BrainBall.x_ball_spot=max;
    		BrainBall.center = ball.center;
    	}
    	else
    	{
    		BrainBall.found= false;
    	}

    	if (sem_post(&ball_sem) != 0)
    		printf("Couldn't sem_post\n");

    	E_State st = Robotstate;
    	//////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////


    	if (ball.found && ball.radius > 0)
    	{
    		cv::circle(m, ball.center, 1, Scalar(255,0,0),4,4);
    		cv::circle(m, ball.center, (int)ball.radius+1, Scalar(255,0,0),4,4);

    		//Elimelech, fix it!
    		Point2f BallDist;
    		if (ball.center.y + ball.radius < FRAME_HEIGHT-1)
    		{
    			BallDist = vis.uv2xy(ball.center.x ,ball.center.y + (int)ball.radius);
    			cv::circle(m, Point(ball.center.x ,ball.center.y + (int)ball.radius), 1, Scalar(0,0,255),4,4);
    		}
    		else
    			BallDist = vis.uv2xy(ball.center.x ,FRAME_HEIGHT-1);


    		char text[70];
    		sprintf(text, "(x,y)=(%d,%d) || ball radius= %d || tilt= %f || (u,v)=(%d,%d)",(int)BallDist.x,(int)BallDist.y,(int)ball.radius, vis.motionTilt,(int)ball.center.x ,(int)ball.center.y);
    		putText(m, text, Point(10,10), Font_Type, FootNoteFont_Scale, Font_Colour,Font_Thickness);
    	}

    	char text[70];
		sprintf(text, "BrainBallCounter [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",BrainBallCounter.x_place[0]
														,BrainBallCounter.x_place[1]
														,BrainBallCounter.x_place[2]
														,BrainBallCounter.x_place[3]
														,BrainBallCounter.x_place[4]
														,BrainBallCounter.x_place[5]
														,BrainBallCounter.x_place[6]
														,BrainBallCounter.x_place[7]
														,BrainBallCounter.x_place[8]
														,BrainBallCounter.x_place[9]    );
		putText(m, text, Point(0,30), Font_Type, FootNoteFont_Scale, Font_Colour,Font_Thickness);

		sprintf(text, "BrainBall [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]",BrainBall.x_place[0]
														,BrainBall.x_place[1]
														,BrainBall.x_place[2]
														,BrainBall.x_place[3]
														,BrainBall.x_place[4]
														,BrainBall.x_place[5]
														,BrainBall.x_place[6]
														,BrainBall.x_place[7]
														,BrainBall.x_place[8]
														,BrainBall.x_place[9]    );
		putText(m, text, Point(0,50), Font_Type, FootNoteFont_Scale, Font_Colour,Font_Thickness);


		sprintf(text, "******************* %s ********************", StateList[st].stateString.c_str() );
		putText(m, text, Point(0,70), Font_Type, FootNoteFont_Scale, Font_Colour,1);

    	vis.ShowImage(m);
    }

	vis.CloseCamera();

	return NULL;
}




/*****************************************************************************************
* Method Name: play
* Description: main state machine
* Arguments: None
* Return Values: none
* Wrote: Roman & Gilad
*****************************************************************************************/
void play()
{
	int headPosCheck = 1;
	E_HeadPosition HeadPosition = LUnknown;
	writeToLog(logFile,"HeadPosition = LUnknown");

	E_State State;
	State = Init;

	while (playing!='q')
	{
    	if (sem_wait(&ball_sem) != 0)
    		printf("Couldn't sem_wait\n");

    	S_BrainBall ball1 = BrainBall;

    	Robotstate = State;

    	if (sem_post(&ball_sem) != 0)
    		printf("Couldn't sem_post\n");

		//////////////////////INIT////////////////////////////////////

		if (State == Init)
		{
			writeToLog(logFile,"State == Init");
			Action::GetInstance()->Start(9);
			while(Action::GetInstance()->IsRunning()) usleep(8*1000);
			SetTilt(0,0);
			HeadPosition = LookingStraight;
			writeToLog(logFile,"HeadPosition = LookingStraight");
			State = LookForBall;
		}

		//////////////////////LOOK_FOR_BALL////////////////////////////////////

		else if (State == LookForBall)
		{
			Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
			writeToLog(logFile,"State == LookForBall");

	    	//sleep(1);
	    	if (ball1.found)
	    	{
	    		State = GoToBall;
	    		writeToLog(logFile,"State == LookForBall - found");
	    		//headPosCheck = 1;
	    	}
	    	else if (headPosCheck == 1)
	    	{
	    		writeToLog(logFile,"State == LookForBall - LookingStraight");
	    		headPosCheck = 2;
	    		SetTilt(0,0);  //straight
				HeadPosition = LookingStraight;
				writeToLog(logFile,"HeadPosition = LookingStraight");
	    		sleep(2);
	    	}
	    	else if (headPosCheck == 2)
	    	{
	    		writeToLog(logFile,"State == LookForBall - LookingStraightDown");
	    		headPosCheck = 3;
	    		SetTilt(0,-25);  //back to center
				HeadPosition = LookingStraightDown;
				writeToLog(logFile,"HeadPosition = LookingStraightDown");
	    		sleep(2);
	    	}
	    	else if (headPosCheck == 3)
	    	{
	    		writeToLog(logFile,"State == LookForBall - LookingLeft");
	    		headPosCheck = 4;
	    		SetTilt(30,-15);  //left
				HeadPosition = LookingLeft;
				writeToLog(logFile,"HeadPosition = LookingLeft");
	    		sleep(2);
	    	}
	    	else if (headPosCheck == 4)
	    	{
	    		writeToLog(logFile,"State == LookForBall - LookingRight");
	    		headPosCheck = 5;
	    		SetTilt(-30,-15);  //right
				HeadPosition = LookingRight;
				writeToLog(logFile,"HeadPosition = LookingRight");
	    		sleep(2);
	    	}
	    	else if (headPosCheck == 5)
	    	{
	    		writeToLog(logFile,"State == LookForBall - go to different spot");
	    		headPosCheck = 1;
	    		State =ChangeSpot; //and move forward
	    	}
		}

		//////////////////////CHANGE_SPOT////////////////////////////////////

		else if (State == ChangeSpot)
		{
			writeToLog(logFile,"State == ChangeSpot");

    		SetTilt(0,-25);  //back to center
			HeadPosition = LookingStraightDown;
			writeToLog(logFile,"HeadPosition = LookingStraightDown");

			GoTo(0,StateList[State].stateString,ball1.x_ball_spot);

			for (int i=0; i<10; i++) // go forward 40 cm
			{
				if (sem_wait(&ball_sem) != 0)
				printf("Couldn't sem_wait\n");

				S_BrainBall ballinwalk = BrainBall;

				if (sem_post(&ball_sem) != 0)
					printf("Couldn't sem_post\n");

				if (ballinwalk.found)
				{
					State = GoToBall;
					writeToLog(logFile,"State == ChangeSpot - found");
					break;
				}

				sleep(1);
			}

			GoStop();

			State = LookForBall;
		}

		//////////////////////GO_TO_BALL////////////////////////////////////

		else if (State == GoToBall)
		{
			writeToLog(logFile,"State == GoToBall");

			int dir=ball1.x_ball_spot;

			if (ball1.found == false)
			{
				State = LookForBall;
			}
			else if (HeadPosition == LookingStraight)
			{
				if(ball1.x_ball_spot > 3 && ball1.x_ball_spot < 7)
				{
					GoTo(0,StateList[State].stateString,ball1.x_ball_spot); //forward
				}
				else if(ball1.x_ball_spot < 4)
				{
					GoTo(dir,StateList[State].stateString,ball1.x_ball_spot);  //left
				}
				else if(ball1.x_ball_spot > 6)
				{
					GoTo(-(dir % 5),StateList[State].stateString,ball1.x_ball_spot);  //Right
				}
				writeToLog(logFile,"GoToBall - LookingStraight");
			}
			else if (HeadPosition == LookingStraightDown)
			{
				if(ball1.center.y < 360)
				{
					if(ball1.x_ball_spot > 3 && ball1.x_ball_spot < 7)
					{
						GoTo(0,StateList[State].stateString,ball1.x_ball_spot); //forward
					}
					else if(ball1.x_ball_spot < 4)
					{
						GoTo(dir,StateList[State].stateString,ball1.x_ball_spot);  //left
					}
					else if(ball1.x_ball_spot > 6)
					{
						GoTo(-(dir % 5),StateList[State].stateString,ball1.x_ball_spot);  //right
					}
					writeToLog(logFile,"GoToBall - LookingStraightDown >360");
				}
				else
				{
					std::string g="Next State: GoForKick y=";
					stringstream p;
					p<<(int)ball1.center.y;
					g+=p.str();
					writeToLog(logFile,g);
					State=GoForKick;
				}
			}
			else if (HeadPosition == LookingRight)
			{
				if(ball1.x_ball_spot < 6)
				{
					GoTo(-(dir+1),StateList[State].stateString,ball1.x_ball_spot);
				}
				else if(ball1.x_ball_spot > 5)
				{
					GoTo(-dir,StateList[State].stateString,ball1.x_ball_spot);
				}

	    		SetTilt(0,-15);
				HeadPosition = LookingStraight;
				writeToLog(logFile,"HeadPosition = LookingStraight");
				sleep(1);
			}
			else if (HeadPosition == LookingLeft)
			{
				if(ball1.x_ball_spot < 6)
				{
					GoTo(8-dir,StateList[State].stateString,ball1.x_ball_spot);
				}
				else if(ball1.x_ball_spot > 5)
				{
					GoTo(5-(dir%5),StateList[State].stateString,ball1.x_ball_spot);
				}

	    		SetTilt(0,-15);
				HeadPosition = LookingStraight;
				writeToLog(logFile,"HeadPosition = LookingStraight");
				sleep(1);
			}

			sleep(1);
		}

		//////////////////////GO FOR KICK////////////////////////////////////

		else if (State == GoForKick)
		{
			if (1)//ball1.center.y>360
			{
				std::string g="State == GoForKick y=";
				stringstream p;
				p<<(int)ball1.center.y;
				g+=p.str();
				writeToLog(logFile,g);

				if(ball1.x_ball_spot > 3 && ball1.x_ball_spot < 7)
				{
					GoTo(0,StateList[State].stateString,ball1.x_ball_spot); //forward
					writeToLog(logFile,"GoForKick forward");
				}
				else if(ball1.x_ball_spot<4)
				{
					GoTo(6,StateList[State].stateString,ball1.x_ball_spot);  //right
					writeToLog(logFile,"GoForKick right");
				}
				else if(ball1.x_ball_spot>6)
				{
					GoTo(-6,StateList[State].stateString,ball1.x_ball_spot);  //left
					writeToLog(logFile,"GoForKick left");
				}

				sleep(4);

				GoTo(0,StateList[State].stateString,ball1.x_ball_spot);

//				if (ball1.center.y<420)
//					sleep(15);
//				else
					sleep(11);


				GoStop();
				writeToLog(logFile,"Kick");
				Action::GetInstance()->Start(2);
				while(Action::GetInstance()->IsRunning()) usleep(8*1000);
			}

			State = GoToBall;
		}

		//////////////////////EXIT////////////////////////////////////

		else if (State == EXIT)
		{
			playing='q';
		}
	}


	playing='j';
	GoStop();
}






/*****************************************************************************************
* Method Name: GoTo
* Description: - right
* 			   + left
* Arguments: None
* Return Values: None
* Wrote: Gilad & Roman
*****************************************************************************************/
void GoTo(const double& dir,std::string state,int x_ball_spot)
{
	if(Walking::GetInstance()->IsRunning() == false )
	{
		if (dir==0)
			writeToLog(logFile,"Going Forward",state,dir,x_ball_spot);
		else if (dir>0)
			writeToLog(logFile,"Turning Lrft",state,dir,x_ball_spot);
		else
			writeToLog(logFile,"Turning Right",state,dir,x_ball_spot);


		Action::GetInstance()->Start(9);
		while(Action::GetInstance()->IsRunning()) usleep(8*1000);

		Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true,true);
		Walking::GetInstance()->X_MOVE_AMPLITUDE = 0;
		Walking::GetInstance()->Y_MOVE_AMPLITUDE = 0;
		Walking::GetInstance()->A_MOVE_AMPLITUDE = dir;
		Walking::GetInstance()->Start();
	}
	else if(Walking::GetInstance()->IsRunning() == true )
	{
		if (dir==0)
			writeToLog(logFile,"Going Forward",state,dir,x_ball_spot);
		else if (dir>0)
			writeToLog(logFile,"Turning Left",state,dir,x_ball_spot);
		else
			writeToLog(logFile,"Turning Rigt",state,dir,x_ball_spot);

		Walking::GetInstance()->A_MOVE_AMPLITUDE = dir;
	}
}


/*****************************************************************************************
* Method Name: GoToUpdateDirection
* Description: + right
* 			   - left
* Arguments: None
* Return Values: None
* Wrote: Gilad & Roman
*****************************************************************************************/
void GoToUpdateDirection(const double& dir)
{
	if(Walking::GetInstance()->IsRunning() == true )
	{
		if (dir>0)
			writeToLog(logFile,"Turning Right");
		else
			writeToLog(logFile,"Turning Left");

		Walking::GetInstance()->A_MOVE_AMPLITUDE = dir;

	}
}


/*****************************************************************************************
* Method Name: GoStop
* Description:
* Arguments: None
* Return Values: None
* Wrote: Gilad & Roman
*****************************************************************************************/
void GoStop()
{
	if(Walking::GetInstance()->IsRunning() == true )
	{
		fprintf(stderr, "STOPPING\n");
		Walking::GetInstance()->Stop();
		while(Walking::GetInstance()->IsRunning() == 1) usleep(8000);
		Walking::GetInstance()->m_Joint.SetEnableBody(false);
		Head::GetInstance()->m_Joint.SetEnableBody(false);
		Action::GetInstance()->m_Joint.SetEnableBody(true);
		MotionManager::GetInstance()->SetEnable(true);
	}
}


