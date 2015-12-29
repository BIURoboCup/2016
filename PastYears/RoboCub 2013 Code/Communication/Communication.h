/*
 * Communication.h
 * Author: Efrat&Naama
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "General/MyThreadClass.h"
#include "IncludesFile.h"
#include "GameControllerData.h"
#include "UDPClient.h"
#include "CommunicationStruct.h"
#include "Enums/PlayerMode.h"

#define GAME_CONTROLLER_ON	(true)
#define GAME_CONTROLLER_OFF	(false)

#define BFF_PACKET_SIZE		(105)
#define GAME_CONTROLLER_SOURCE_ADDRESS "192.168.1.1"

class Localization;
extern int myTeamNumber;
extern bool exitProgram;
class Communication : public MyThreadClass
{

private:

	static Communication* 	_uniqueInstance;
	static bool				m_gameControllerStatus;

	UDPSocket* 				sock;
	UDPSocket* 				RecieveSocket1;
	UDPSocket* 				RecieveSocket2;
	UDPSocket* 				SendSocket1;
	UDPSocket* 				SendSocket2;
	AllLocationData_t*		MyBffLocationData1;
	AllLocationData_t* 		MyBffLocationData2;
	Localization* 			m_localization;
	string sourceAddress1;
	string sourceAddress2;
	unsigned short myPort;
	unsigned short recievePort1;
	unsigned short recievePort2;

	// This is the private c'tor of the singleton
	Communication();

	bool FindAddresses();
	void RecieveFriendData();
	void SetMyData();
	void SetGoalColour();
	void MembersInitialization();
	//string FindMyIPAddress();

	bool _SendFlag1;
	bool _SendFlag2;
	bool _RecieveFlag1;
	bool _RecieveFlag2;
	bool firstTime;
	uint8 goalColour;


public:
	int teamId;
	static string myAddress;
	struct RoboCupGameControlData* gameControlData;
	AllLocationData_t* 		MyLocationData;

	static Communication* GetInstance();
	static void SetGameControllerStatus(bool gameControllerStatus);
	static void SetMyIP(string _myAddress);
	void InternalThreadFunc();
	void GetGameControlData();
	void sendMyData();
	bool GetCommunicationFlag();

	uint8 GetMyState();
	uint8 GetGoalColour();
	uint8 GetkickOffTeam();
	uint8 GetIfFirstHalf();

	AllLocationData_t* GetFriendData(int robotId);
	void SetRobotID(int lineInFile);
	virtual ~Communication();

	UDPSocket* OpenNewUDPReceiveSocket(bool &receiveFlag, unsigned short receivePort);
};

#endif /* COMMUNICATION_H_ */
