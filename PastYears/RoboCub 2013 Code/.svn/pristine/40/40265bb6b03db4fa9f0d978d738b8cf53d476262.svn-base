/*
 * Communication.cpp
 */

#include "Communication.h"
#include "UDPClient.h"
#include "Localization/Localization.h"
#include "MyRobot.h"
#include <string.h>

using namespace std;

//#define DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
//#define DEBUG_COMM_SEND_MY_DATA
//#define DEBUG_COMM_GAME_CONTROLLER
//#define DEBUG_COMM_MY_DATA

Communication* Communication::_uniqueInstance = 0;
bool Communication::m_gameControllerStatus = false;
string Communication::myAddress;

Communication::Communication()
{
	firstTime = true;
	//cout<<"Communication::Communication()\n";
	MembersInitialization();
	if(!FindAddresses()) return;

	_RecieveFlag1 = false;
	_RecieveFlag2 = false;

	RecieveSocket1 = OpenNewUDPReceiveSocket(_RecieveFlag1, recievePort1);
	RecieveSocket2 = OpenNewUDPReceiveSocket(_RecieveFlag2, recievePort2);

	SendSocket1 = new UDPSocket();
	SendSocket2 = new UDPSocket();
	_SendFlag1 = true;
	_SendFlag2 = true;
}

void Communication::SetMyIP(string _myAddress)
{
	myAddress = _myAddress;
}

void Communication::SetGameControllerStatus(bool gameControllerStatus)
{
	m_gameControllerStatus = gameControllerStatus;
	cout << "gameControllerStatus = " << m_gameControllerStatus << endl;
}


void Communication::MembersInitialization()
{
	sock = new UDPSocket((unsigned short)GAMECONTROLLER_PORT);
	gameControlData = (RoboCupGameControlData*) (malloc(sizeof (RoboCupGameControlData)));
	gameControlData->state = STATE_INITIAL;
	m_localization = Localization::GetInstance();

	MyLocationData = new AllLocationData_t();
	MyBffLocationData1 = new AllLocationData_t();
	MyBffLocationData2 = new AllLocationData_t();

	MyBffLocationData1->ballLocation.time = 0;
	MyBffLocationData2->ballLocation.time = 0;

	sourceAddress1 = "1.1.1.1";
	sourceAddress2 = "1.1.1.2";
}

Communication* Communication::GetInstance()
{
	if (_uniqueInstance == 0)
	{
		_uniqueInstance = new Communication();
	}
	return _uniqueInstance;
}

bool Communication::GetCommunicationFlag()
{
	if ((!_SendFlag1 || !_RecieveFlag1 )  && (!_SendFlag2 || !_RecieveFlag2))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void Communication::GetGameControlData()
{
    int bytesRcvd;
    char recvString[GAMECONTROLLER_PACKET_SIZE + 1]; // Buffer for echo string + \0
    string sourceAddress; // Address of datagram source
    unsigned short sourcePort; // Port of datagram source
	#ifdef DEBUG_COMM_GAME_CONTROLLER
    	cout << "GetGameControlData Before receive\n";
	#endif
    try
    {
		bytesRcvd = sock->recvFrom((void *) recvString, GAMECONTROLLER_PACKET_SIZE, sourceAddress, sourcePort, false);
		#ifdef DEBUG_COMM_GAME_CONTROLLER
			cout << "GetGameControlData After recieve\n";
	    #endif
		if(sourceAddress == GAME_CONTROLLER_SOURCE_ADDRESS)
		{
			recvString[bytesRcvd] = '\0'; // Terminate string
			memcpy((void *) gameControlData, (const void *) recvString, sizeof (RoboCupGameControlData));
			if(firstTime)
			{
				#ifdef DEBUG_COMM_GAME_CONTROLLER
					cout << "GetGameControlData - first Time\n";
				#endif
				if(gameControlData->teams[0].teamNumber == myTeamNumber)
				{
					teamId = 0;
				}
				else
				{
					teamId = 1;
				}

				if (m_gameControllerStatus == GAME_CONTROLLER_ON)
				{
					SetGoalColour();
				}
				firstTime = false;
			}

			if (m_gameControllerStatus == GAME_CONTROLLER_ON)
			{
				if (gameControlData->firstHalf == 0)
				{
					if(goalColour != gameControlData->teams[teamId].goalColour)
					{
						SetGoalColour();
					}
				}
			}
		}

		//cout << "************** game state = " << gameControlData->state << "\n";

		#ifdef DEBUG_COMM_GAME_CONTROLLER
			cout<< "[Communication]: State: "<<(unsigned int)gameControlData->state << "\n";
		#endif
    }
    catch(...)
    {
		#ifdef DEBUG_COMM_GAME_CONTROLLER
    		cout<<"errno: "<<errno<<" In GameControllerData\n";
		#endif
    	return;
    }
}

void Communication::SetGoalColour()
{
	goalColour = gameControlData->teams[teamId].goalColour;
	m_localization->SetDirection(goalColour);
}

void Communication::RecieveFriendData()
{
	int errno;
	char buff1[BFF_PACKET_SIZE+1];
	char buff2[BFF_PACKET_SIZE+1];

	string Address1;
	string Address2;
	unsigned short sourcePort1;
	unsigned short sourcePort2;

	if(_RecieveFlag1 == false)
	{
		RecieveSocket1 = OpenNewUDPReceiveSocket(_RecieveFlag1, recievePort1);
	}

	if(_RecieveFlag2 == false)
	{
		RecieveSocket2 = OpenNewUDPReceiveSocket(_RecieveFlag2, recievePort2);
	}

	errno = 0;
	if (_RecieveFlag1 == true)
	{
		try
		{
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Communication: Before ReceiveFrom BFF 1\n";
			cout<< "sourcePort1:" << recievePort1 << "\n";
			#endif
			int len1 = RecieveSocket1->recvFrom((void *) buff1, BFF_PACKET_SIZE, Address1, sourcePort1, true);
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "After RecieveSocket1->recvFrom\n";
			#endif
			buff1[len1]='\0';
			memcpy((void *) MyBffLocationData1, (const void *) buff1, sizeof (AllLocationData_t));
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Friend Data has been copied\n";
			#endif

		}
		catch (const char* str)
		{
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Communication Error: " << str << "\n";
			#endif
			_RecieveFlag1 = false;
			free(RecieveSocket1);
		}
		catch (...)
		{
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Unknown Communication Error, in receive from, errno = " << errno << "\n";
			#endif
			_RecieveFlag1 = false;
			free(RecieveSocket1);
		}
	}

	errno = 0;
	if (_RecieveFlag2 == true)
	{
		try
		{
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Communication: Before ReceiveFrom BFF 2\n";
			cout<< "sourcePort2:" << recievePort2 << "\n";
			#endif
			int len2 = RecieveSocket2->recvFrom((void *) buff2, BFF_PACKET_SIZE, Address2, sourcePort2, true);
			buff1[len2]='\0';
			memcpy((void *) MyBffLocationData2, (const void *) buff2, sizeof (AllLocationData_t));
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Friend Data has been copied\n";
			#endif
		}
		catch (const char* str)
		{
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Communication Error: " << str << "\n";
			cout << "Unknown Communication Error, in receive from, errno = " << errno << "\n";
			#endif
			_RecieveFlag2 = false;
			free(RecieveSocket2);
		}
		catch (...)
		{
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout << "Unknown Communication Error, in receive from, errno = " << errno << "\n";
			#endif
			_RecieveFlag2 = false;
			free(RecieveSocket2);
		}
	}
}

UDPSocket* Communication::OpenNewUDPReceiveSocket(bool &receiveFlag, unsigned short receivePort)
{
	UDPSocket* RecieveSocket;

	try
	{
		RecieveSocket = new UDPSocket((unsigned short)receivePort);
		receiveFlag = true;
	}
	catch (const char* str)
	{
		#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
		cout << "Communication Error: " << str << "\n";
		#endif

	}
	catch (...)
	{
		#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
		cout << "Communication Error, creating UDP socket \n";
		#endif
	}

	return RecieveSocket;
}

AllLocationData_t* Communication::GetFriendData(int robotId)
{
	switch(robotId)
	{
	case(1):
			return MyBffLocationData1;
		break;
	case(2):
			return MyBffLocationData2;
		break;
	default:
		return NULL;
	}
}

void Communication::sendMyData()
{
	#ifdef DEBUG_COMM_SEND_MY_DATA
	cout << "Start to Send My Data, flag: "<< _SendFlag1 << "\n";
	#endif

	if (_SendFlag1 == false)
	{
		#ifdef DEBUG_COMM_SEND_MY_DATA
		cout << "Send My Data, Open new Socket with friend1 \n";
		#endif
		SendSocket1 = new UDPSocket();
		_SendFlag1 = true;
	}
	if (_SendFlag2 == false)
	{
		#ifdef DEBUG_COMM_SEND_MY_DATA
		cout << "Send My Data, Open new Socket with friend2 \n";
		#endif
		SendSocket2 = new UDPSocket();
		_SendFlag2 = true;
	}

	try
	{
		#ifdef DEBUG_COMM_SEND_MY_DATA
			cout << "Try to Send My Data\n" << "MyPort: "<< myPort << " SourceAddress1: " <<sourceAddress1 <<"\n";
		#endif
		SendSocket1->sendTo((void*)MyLocationData, sizeof(AllLocationData_t)+1,sourceAddress1, myPort, false);
	}
	catch(...)
	{
		#ifdef DEBUG_COMM_SEND_MY_DATA
			cout << "Send My Data to source address 1 error\n";
			cout<< "errno: " << errno <<" TimeOut\n";
		#endif
		_SendFlag1 = false;
		free(SendSocket1);
	}

	try
	{
		#ifdef DEBUG_COMM_SEND_MY_DATA
			cout << "Try to Send My Data\n" << "MyPort: "<< myPort << " SourceAddress2: " <<sourceAddress2 <<"\n";
		#endif
		SendSocket2->sendTo((void*)MyLocationData, sizeof(AllLocationData_t)+1,sourceAddress2, myPort, false);
	}
	catch(...)
	{
		#ifdef DEBUG_COMM_SEND_MY_DATA
			cout << "Send My Data to source address 2 error\n";
			cout<< "errno: " << errno <<" TimeOut\n";
		#endif
		_SendFlag2 = false;
		free(SendSocket2);
	}
}

bool Communication::FindAddresses()
{
	 //cout<<"In FindAddresses function\n";
	 ifstream myReadFile;
	 int i = 0;
	 myReadFile.open("IPAddresses");
	 if(myReadFile.fail())
		 return false;
	 char output[100];
	 string addresses[2];
	 string ports[2];
	 if (myReadFile.is_open())
	 {
		 for(int k=0; k<3; k++)
		 {
			 myReadFile >> output;
			 if(output != myAddress)
			 {
				 if(i==2)
				 {
				   cout<<"My IP is not in IPAddresses!!!\n";
				   exit(1);
				 }
				 //cout<<"output1: "<<output<<"\n";
				 addresses[i] = output;
				 myReadFile >> output;
				 //cout<<"output2: Port: "<<output<<"\n";
				 ports[i] = output;
				 i++;
			 }
			 else
			 {
				 myReadFile >> output;
				 myPort = (unsigned short)atoi(output);

				 SetRobotID(k);
			 }
		 }
		 if(addresses[0].empty() || addresses[1].empty())
			 return false;
	 }
	sourceAddress1 = addresses[0];
	sourceAddress2 = addresses[1];
	cout<<"[IPAddresses]: First Friend IP: 0: "<<addresses[0] <<" Second Friend IP: 1: "<<addresses[1]<<"\n";
	recievePort1 = (unsigned short)atoi(ports[0].c_str());
	recievePort2 = (unsigned short)atoi(ports[1].c_str());

	myReadFile.close();
	return true;
}

void Communication::SetMyData()
{
	myTimeval currentTime;
	currentTime.update();

	MyLocationData->robotlocation = m_localization->GetMyLocation();
	MyLocationData->ballLocation = m_localization->GetBallLocation();

	MyLocationData->locationnDiff = currentTime - MyLocationData->robotlocation.time;
	MyLocationData->ballDiff = currentTime - MyLocationData->ballLocation.time;

	#ifdef DEBUG_COMM_MY_DATA
		cout << "[comm My Data: ]" << MyLocationData->robotlocation.x << "\n";
	#endif
	#ifdef DEBUG_COMM_GAME_CONTROLLER
		cout << "Communication: MyLocationData->ballLocation.distanceFromMe: " << MyLocationData->ballLocation.distanceFromMe << "\n";
		cout << "Communication: m_localization->GetBallLocation().distanceFromMe: " << m_localization->GetBallLocation().distanceFromMe << "\n";
	#endif
}

uint8 Communication::GetMyState()
{
	if (m_gameControllerStatus == GAME_CONTROLLER_ON)
	{
		return gameControlData->state;
	}
	else
	{
		return STATE_PLAYING;
	}
}
uint8 Communication::GetGoalColour()
{
	if (m_gameControllerStatus == GAME_CONTROLLER_ON)
	{
		return gameControlData->teams[teamId].goalColour;
	}
	else
	{
		return 0;
	}
}
uint8 Communication::GetkickOffTeam()
{
	if (m_gameControllerStatus == GAME_CONTROLLER_ON)
	{
		 return gameControlData->kickOffTeam;
	}
	else
	{
		return 0;
	}
}

uint8 Communication::GetIfFirstHalf()
{
	if (m_gameControllerStatus == GAME_CONTROLLER_ON)
	{
		 return gameControlData->firstHalf;
	}
	else
	{
		return 1;
	}
}

void Communication::InternalThreadFunc()
{

	while(exitProgram == false)
	{
		try
		{
			GetGameControlData();
			#ifdef DEBUG_COMM_GAME_CONTROLLER
				cout << "GetGameControlData()\n";
			#endif
			SetMyData();
			#ifdef DEBUG_COMM_SEND_MY_DATA
				cout << "SetMyData()\n";
			#endif
			sendMyData();
			RecieveFriendData();
			#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
				cout << "RecieveFriendData()\n";
			#endif
			usleep(50 * 1000);
		}

		catch(...)
		{
#ifdef DEBUG_COMM_RECEIVE_DATA_FROM_FRIENDS
			cout<<"error: "<<errno<<" --> No communication \n";
#endif
		}
	}

	cout << "Communication Thread exited\n";
}


Communication::~Communication()
{
	free(gameControlData);
}

void Communication::SetRobotID(int lineInFile)
{
	cout << "SetRobotID%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% = "<< lineInFile << "\n";
	switch (lineInFile)
	{
	case 0:
		MyRobot::SetPlayerID(ID_STRIKER1);
		break;
	case 1:
		MyRobot::SetPlayerID(ID_STRIKER2);
		break;
	case 2:
		MyRobot::SetPlayerID(ID_GOALIE);
		break;
	default:
		MyRobot::SetPlayerID(ID_UNKNOWN);
		break;
	}
}
