/*
 * Goalie.cpp
 *
 *  Created on: 17 5 2012
 *      Author: Amir&Sarit
 */

#include "Goalie.h"

Goalie* Goalie::m_uniqueInstance = 0;

Goalie::Goalie()
{
	cout<<"Goalie::Goalie()"<<endl;
	m_localization = Localization::GetInstance();
	cout<<"Goalie::Goalie()1"<<endl;
	m_motion = WalkingAndActions::GetInstance();
	cout<<"Goalie::Goalie()2"<<endl;
	m_communication = Communication::GetInstance();
	cout<<"Goalie::Goalie()3"<<endl;
	m_goTo = GoTo::GetInstance();
	cout<<"Goalie::Goalie()4"<<endl;
}

Goalie::~Goalie() {}

Goalie* Goalie::GetInstance()
{
	if (m_uniqueInstance == 0)

	{
		m_uniqueInstance = new Goalie();
	}
	return m_uniqueInstance;
}

void Goalie::InternalThreadFunc()
{
	while(1)
	{
		cout<<"DEBUG:: Goalie::internal thread function"<<endl;
//		switch(m_communication->GetMyState())
//		{
//		case STATE_INITIAL:
//			DoStateGameControllerInitial();
//			break;
//		case STATE_SET:
//			DoStateGameControllerSet();
//			break;
//		case STATE_READY:
//			DoStateGameControllerReady();
//			break;
//		case STATE_PLAYING:
			GoalieMain(); // -->for debug
//			break;
//		case STATE_FINISHED:
//			DoStateGameControllerFinished();
//			break;
//		default:
//			printf("ERROR IN GOALIE STATE MACHINE!!!\n\n");
//			break;
//		}
	}
}

void Goalie::DoStateGameControllerReady()
{
	XYLocation_s positionLocation;
	positionLocation.x = GOALIE_POSITION_X;
	positionLocation.y = GOALIE_POSITION_Y;
	m_goTo->Stop();
	m_goTo->StartGoToXY(positionLocation);
}




void Goalie::GoalieMain()
{
	printf("Goalie::GoalieMain()\n");

	usleep(1000000);

	_OLD_X=0;
	//_my_pos=m_localization->GetMyLocation();
	//usleep(500000);
	m_motion->StartAction(STAND_UP);
	while(m_communication->GetMyState() == STATE_PLAYING)
	{

		do
		{

			usleep(50000);
			if(m_communication->GetMyState() != STATE_PLAYING)
			{
				return;
			}
			if (m_localization->GetBallLocation().isInSight)
			{
				if ((m_localization->GetBallLocation().y)-(m_localization->GetMyLocation().y)<150)
				{
					_mylist.push_back(m_localization->GetBallLocation());
					_myPos.push_back(m_localization->GetMyLocation());
				}
			}

			//cout << "MyX= "<<m_localization->GetMyLocation().x<<" MyY= "<<m_localization->GetMyLocation().y << " Ballx= "<<m_localization->GetBallLocation().x<<" Bally= "<<m_localization->GetBallLocation().y<<"\n";
		}while(_mylist.size()<=0);
/*		_itMy=_myPos.begin();
		for (_it=_mylist.begin(); _it!=_mylist.end(); ++_it) {
								printf("x=%f y=%f  dis=%f\n",(*_it).x-(*_itMy).x,(*_it).y-(*_itMy).y,(*_it).distanceFromMe);
					_itMy++;}
*/


//		_it=_mylist.end();
//		_it--;
//		_itMy = _myPos.end();
//		_itMy--;
//		cout << "MyX= "<<(*_itMy).x<<" MyY= "<<(*_itMy).y << " Ballx= "<<(*_it).x<<" Bally= "<<(*_it).y<<"\n";
		//for (_it=_mylist.begin(); _it!=_mylist.end(); ++_it) {
		//printf("x=%f y=%f\n",(*_it).x,(*_it).y);}
		//printf("\n");
		if(IncomeKickToGoal() == true)//		if(IncomeKickToGoal() == true)
		{
			//write(1,"IncomeKickToGoal()\n",sizeof("IncomeKickToGoal()\n"));
			JumpToSave();
		}
		else
		{
			if(CheckDistanceFromBall())
			{
				//write(1,"CheckDistanceFromBall()\n",sizeof("CheckDistanceFromBall()\n"));
				KickTheBall();
			}
			else
			{
				//write(1,"MoveToNewRange()\n",sizeof("MoveToNewRange()\n"));
				MoveToNewRange();// for now return to the middle of the goal.
			}

		}

		}

}


bool Goalie::IncomeKickToGoal()
{
	//cout<<"IncomeKickToGoal\n";
	if(_mylist.size()==1) return false;
	_it=_mylist.end();
	_it--;
	_itMy=_myPos.end();
	_itMy--;
	float b_old_y,b_old_x,b_new_y,b_new_x,b_old2_y,b_old2_x;

	b_new_y=(*_it).y-(*_itMy).y;
	b_new_x=(*_it).x-(*_itMy).x;
	//cout << "\n NEW: MyX= "<<(*_itMy).x<<" MyY= "<<(*_itMy).y << " Ballx= "<<(*_it).x<<" Bally= "<<(*_it).y<<"\n";
	_it--;
	_itMy--;
	b_old_y=(*_it).y-(*_itMy).y;

	b_old_x=(*_it).x-(*_itMy).x;
	//cout << "\n OLD: MyX= "<<(*_itMy).x<<" MyY= "<<(*_itMy).y << " Ballx= "<<(*_it).x<<" Bally= "<<(*_it).y<<"\n";

	_it--;
	_itMy--;
	b_old2_y=(*_it).y-(*_itMy).y;

	b_old2_x=(*_it).x-(*_itMy).x;
	_it++;
	_itMy++;

	//cout << "Ball old y = " << b_old_y << " Ball new y = " << b_new_y << " Dis:" << (*_it).distanceFromMe<< endl;
	//cout << "Ball old x = " << b_old_x << " Ball new x = " << b_new_x << " Dis:" << (*_it).distanceFromMe<< endl;

	if((b_new_y<0))
	{

		_mylist.clear();
		_myPos.clear();
		//cout<<"Debug:  b_new<0.\n";
		return false;
	}



	if(b_new_y<40)
	{
		if((b_new_y+7)<b_old_y)
		{
			cout<<"DEBUG: jump 40 33\n";
			_it++;
			if(((b_new_x-b_old_x)<3)||((b_new_x-b_old_x)>-3))
			{_new_X=((*_it).x-(*_itMy).x);}
			else
			{
				cout<<"\nDebug: X more than 3------------.\n";
				cout<<"\ndiff x-x:"<< (b_new_x-b_old_x)<<"\n"<<endl;

				float M=(b_new_y-b_old_y)/(b_new_x-b_old_x);
				float N=b_new_y-M*b_new_x;
				_new_X=-N/M;
			}
			return true;
		}
	}

	if(b_new_y<60)
	{
		if((b_new_y+10)<b_old_y)
		{
			cout<<"DEBUG: jump 60 50";
			_it++;
			if(((b_new_x-b_old_x)<3)||((b_new_x-b_old_x)>-3))
			{_new_X=((*_it).x-(*_itMy).x);}
			else
			{
				cout<<"\nDebug: X more than 3------------.\n";
				float M=(b_new_y-b_old_y)/(b_new_x-b_old_x);
				float N=b_new_y-M*b_new_x;
				_new_X=-N/M;
			}
			return true;
		}
	}

	if(b_new_y<100)
		{
			if((b_new_y+15)<b_old_y)
				//if((b_old_y+10)<b_old2_y)
			{
					cout<<"DEBUG: jump 100 75 65";
				_it++;
				if(((b_new_x-b_old_x)<3)||((b_new_x-b_old_x)>-3))
				{_new_X=((*_it).x-(*_itMy).x);}
				else
				{
					cout<<"\nDebug: X more than 3------------.\n";
					float M=(b_new_y-b_old_y)/(b_new_x-b_old_x);
					float N=b_new_y-M*b_new_x;
					_new_X=-N/M;
				}
				return true;
			}
		}
	/*float Ts,Ts2,Ts3,Ts4,Ycm,TsYcm,Ts2Ycm,N,time;
	N=(float)_mylist.size();
	_it=_mylist.begin();
	myTimeval first_time=(*_it).time;
*/
	/*if (!ballmoved())
	{
		//cout<<"ball didnt move:"<<endl;
		//_mylist.clear();
		//_myPos.clear();
		_mylist.pop_front();
		_myPos.pop_front();
		return false;
	}*/
/*	if(_mylist.size()==2) return false;
	_itMy=_myPos.begin();
	for (_it=_mylist.begin(); _it!=_mylist.end(); ++_it)
	{
		time=(float)((*_it).time-first_time)/1000000;// start from 0
		Ts+=time;
		Ts2+=time*time;
		Ts3+=time*time*time;
		Ts4+=time*time*time*time;
		Ycm+=(*_it).y-(*_itMy).y;
		TsYcm+=time*(((*_it).y)-(*_itMy).y);
		Ts2Ycm+=time*time*(((*_it).y)-(*_itMy).y);
		_itMy++;
	}
	float a11=Ts4,a12=Ts3,a13=Ts2,a21=Ts3,a22=Ts2,a23=Ts,a31=Ts2,a32=Ts,a33=N;
	float b1=Ts2Ycm,b2=TsYcm,b3=Ycm,A_eq,B_eq,C_eq;
	//R2-(a21/a11)*R1
	a22 = a22-(a21/a11)*a12;
	a23 = a23-(a21/a11)*a13;
	b2 = b2-(a21/a11)*b1;
	a21 = 0;
	//R3-(a31/a11)*R1
	a32 = a32-(a31/a11)*a12;
	a33 = a33-(a31/a11)*a13;
	b3 = b3-(a31/a11)*b1;

	a31=0;
	//again for a 2X2 matrix R3-(a32/a22)*R2
	a33 = a33-(a32/a22)*a23;
	b3 = b3-(a32/a22)*b2;
	a32 = 0;
	//we have now diagonal matrix


	C_eq = b3/a33;
	B_eq = (b2-C_eq*a23)/a22;
	A_eq = (b1-B_eq*a12-C_eq*a13)/a11;

	//we get: y = f (x) = Ax^2 + Bx + C

	//cout<<"f (t) = "<<A_eq<<"t^2 + "<<B_eq<<"t + "<<C_eq<<endl;
	float t_stop=-B_eq/(2*A_eq);
	_it=_mylist.end();
	_it--;
	//float last_t=(float)((*_it).time-first_time)/1000000;
	//if (t_stop<last_t) return false;
	float y_stop=A_eq*t_stop*t_stop+B_eq*t_stop+C_eq;
	if(y_stop<=0)
	{
		printf("y stop= %f t stop= %f\n",y_stop,t_stop);
		a11=a21=a12=b1=b2=0;
		a22=N;
		float X_temp,Y_temp;
		_itMy=_myPos.begin();
		for (_it=_mylist.begin(); _it!=_mylist.end(); ++_it)
		{
			X_temp=((*_it).x)-(*_itMy).x;
			a11+=X_temp*X_temp;
			a12+=X_temp;
			Y_temp=((*_it).y)-(*_itMy).y;
			b1+=Y_temp*X_temp;
			b2+=Y_temp;
			_itMy++;
		}
		a21=a12;
		a22=a22-(a21/a11)*a12;
		b2=b2-(a21/a11)*b1;
		a21=0;
		B_eq=b2/a22;
		A_eq=(b1-B_eq*a12)/a11;
		_new_X=-B_eq/A_eq;// x at goal line
		return true;
	}  */
	_mylist.pop_front();
	_myPos.pop_front();
	return false;
}


void Goalie::JumpToSave()
{
	DirectionOfTheKick();

}


void Goalie::DirectionOfTheKick()
{

	if (_new_X<-1)//if (_new_X<(_my_pos.x-10))
	{
		//m_motion->StartAction(JUMP_LEFT);
		sleep(2);
		_mylist.clear();
		_myPos.clear();
		printf("\n\nJump left \n");
		printf("\n\n_new_X = %f \n",_new_X);
		_itMy=_myPos.begin();
		for (_it=_mylist.begin(); _it!=_mylist.end(); ++_it) {
					printf("x=%f y=%f\n",(*_it).x-(*_itMy).x,(*_it).y-(*_itMy).y);
		_itMy++;}

		_mylist.clear();
		_myPos.clear();
		sleep(4);

	}
	else
	{
		if (_new_X>1) //if (_new_X>_my_pos.x+10)
		{
			//m_motion->StartAction(JUMP_RIGHT);
			sleep(2);
			_mylist.clear();
			_myPos.clear();
			printf("\n\nJump Right\n");
			printf("\n\n_new_X = %f \n",_new_X);
			_itMy=_myPos.begin();
			for (_it=_mylist.begin(); _it!=_mylist.end(); ++_it) {
						printf("x=%f y=%f\n",(*_it).x-(*_itMy).x,(*_it).y-(*_itMy).y);

	_itMy++;}
			_mylist.clear();
			_myPos.clear();
			sleep(4);
		}
		else
		{
			//m_motion->StartAction(JUMP_FORWARD);
			printf("\n\nJump Middle\n");

			printf("\n\n_new_X = %f \n",_new_X);
			_itMy=_myPos.begin();
			for (_it=_mylist.begin(); _it!=_mylist.end(); ++_it) {
						printf("x=%f y=%f\n",(*_it).x-(*_itMy).x,(*_it).y-(*_itMy).y);
			_itMy++;}
			_mylist.clear();
			_myPos.clear();
			sleep(4);
		}
	}
}





bool Goalie::CheckDistanceFromBall()
{
	//cout<<"CheckDistanceFromBall\n";
	if (m_localization->GetBallLocation().isInSight)
	{
		if(_mylist.size()==0) return false;
		_it=_mylist.end();
		_it--;
		_itMy=_myPos.end();
		_itMy--;
		//cout<<"X: "<<(*_itMy).x << " Y: " << (*_it).y << "\n";
		if(((*_itMy).x<-50)||((*_itMy).x>50)) return false;
		//if(_OLD_X==((*_it).x)-((*itMy).x)) return false;
		//_OLD_X=(*_it).x-((*itMy).x));
		if((*_it).y>100)
		{
			//cout << "Angle from me = " << ((*_it).x-(*_itMy).x) << "\n";
			if(((*_it).x-(*_itMy).x)>20)
			{
				m_motion->StartAction(STEP_RIGHT);
				cout <<"STEP LEFT: ";
				//cout << "MyX= "<<(*_itMy).x<<" MyY= "<<(*_itMy).y << " Ballx= "<<(*_it).x<<" Bally= "<<(*_it).y<<"\n";
			}
			if(((*_it).x-(*_itMy).x)<-20)
			{
				m_motion->StartAction(STEP_LEFT);
				cout <<"STEP RIGHT: ";
				//cout << "MyX= "<<(*_itMy).x<<" MyY= "<<(*_itMy).y << " Ballx= "<<(*_it).x<<" Bally= "<<(*_it).y<<"\n";
			}
		}
	}
	/*
	if (BallMoved()) return false;// the ball didnt stop yet
	if((_new_ball_pos.x<REHAVA_LEFT)||(_new_ball_pos.x>REHAVA_RIGHT)) return false;
	if(_new_ball_pos.y>REHAVA_FRONT) return false;
	return true;*/
	return false;
}

bool Goalie::ballmoved()
{
	_it=_mylist.end();
	_it--;
	float newX,oldX,newY,oldY;
	newX=(*(_it)).x;
	newY=(*(_it)).y;
	_it--;
	oldX=(*(_it)).x;
	oldY=(*(_it)).y;
	if(((newY-oldY)<2)&&((newY-oldY)>-2))
	{
		return false;
	}
	else return true;
}
void Goalie::KickTheBall()//
{
	//m_motion->StartAction(KICK_LEFT);
}


void Goalie::MoveToNewRange()
{

}

