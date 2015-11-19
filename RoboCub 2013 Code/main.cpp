#include "MyRobot.h"

using namespace std;

/********************************************************************
 * 							Global Variables
 ********************************************************************/

const double pixel2angleHorizon  = Camera::VIEW_V_ANGLE / ((double)240);
const double pixel2angleVertical = Camera::VIEW_H_ANGLE / ((double)320);

int myTeamNumber;
bool exitProgram;

myTimeval processStartTime;

/********************************************************************
 * 							Global Functions
 ********************************************************************/
PlayerMode_e GetPlayerMode(int argc, char *argv[]);
int ProgramInitialization();
void SetIPStatic(char* const myAddress);

bool IsBallDataRelevant(BallLocation *ballLocation)
{
	myTimeval currentTime;
	currentTime.update();

	return ( currentTime - ballLocation->time < MAX_VALID_TIME_BALL_LOCATION) && (ballLocation->time.tv_sec != 0);
}

bool IsBallDataFromNow(BallLocation *ballLocation)
{
	myTimeval currentTime;
	currentTime.update();

	return ( currentTime - ballLocation->time < 500 * 1000) && (ballLocation->time.tv_sec != 0);
}


/********************************************************************
 *
 *
 * 								Main
 *
 *
 ********************************************************************/

int main(int argc, char *argv[])
{
	char userInput;
	string myIP;

	exitProgram = false;
	processStartTime.update();
	signal(SIGINT,MyRobot::SigIntHandler);
	const time_t programStartTemp = time(NULL);
	struct tm* programStartTimestamp = localtime(&programStartTemp);
	Vision::SetProgramStartTimestamp(programStartTimestamp);

	if(argc < 3)
	{
		cout << "My IP: ";
		cin >> myIP;
		Communication::SetMyIP(myIP);
	}
	else
	{
		Communication::SetMyIP(argv[2]);
	}

	if (argc == 4)
	{
		myTeamNumber = atoi(argv[3]);
	}
	else
	{
		myTeamNumber = TEAM_NUMBER;
	}

	cout << "main thread id: " << (long int)syscall(224) << "\n";
	cout << "Would you like to open up communication with game controller (y/n)? ";
	userInput = getchar();
	if ((userInput == 'Y') || (userInput == 'y'))
	{
		Communication::SetGameControllerStatus(GAME_CONTROLLER_ON);
	}
	else
	{
		Communication::SetGameControllerStatus(GAME_CONTROLLER_OFF);
	}
	userInput = getchar();

	try
	{
		PlayerMode_e playerMode;

		WalkingAndActions *walkingAndActions = WalkingAndActions::GetInstance();
		walkingAndActions->StartAction(SIT_DOWN);

		// In order to start from a known position - the Robot will sit down at first.
		// WARNING: IF DELETED, THE ROBOT WOULD STRETCH TO AN UNDESIRED POSITION!
		//WalkingAndActions *walkingAndActions = WalkingAndActions::GetInstance();
		//walkingAndActions->StartAction(STAND_UP);
		playerMode = GetPlayerMode(argc, argv);

		int visionFlag = ProgramInitialization();
		MyRobot::SetVisionFlag(visionFlag);

		MyRobot robot(playerMode);
		robot.Process();

		cout << "end program" << endl;

	}

	catch (char* str)
	{
		cout << str << endl;
		return 1;
	}
	catch (const char* str)
	{
		cout << str << endl;
		return 1;
	}
	catch (...) {
		cout<<"something bad happened. sorry."<<endl;
		return 0;
	}

	return 0;
}

int ProgramInitialization()
{
	char userInput;

	int visionFlag = 0;

	cout << "\n***************************************";
	cout << "\n***************************************\n\n ";

	cout << "Would you like to open up the vision debugging windows (Y/N)? ";

	userInput = getchar();
	if ((userInput == 'Y') || (userInput == 'y'))
	{
		visionFlag = visionFlag | OPEN_VISION_DEBUG_WINDOWS;
	}
	userInput = getchar();


	cout << "Would you like to record the vision video of the robot (Y/N)? ";

	userInput = getchar();
	if ((userInput == 'Y') || (userInput == 'y'))
	{
		visionFlag = visionFlag | SAVE_CAPTURED_VIDEO_STREAM;
	}
	userInput = getchar();

	return visionFlag;
}

void SetIPStatic(char* const myAddress)
{
	char command[50];
	strcpy(command, "wlan0 ");
	strcat(command, myAddress);
	strcat(command, " up");
	cout<<"In SetIPStatic, Command: "<<command<<"\n";
	pid_t 	pid;
	int 	waitStatus;
	 /* fork a child process */
	if ((pid = fork()) < 0)
	{
		printf("*** ERROR: forking child process failed\n");
		exit(1);
	}
	else if (pid == 0) /*CHILD*/
	{
		char* argv[] = {"ifconfig", command};
		if(execvp("ifconfig", argv) < 0)
		{
			throw "Networking error, in SetIPStatic";
		}
	}
	else
	{
		while (wait(&waitStatus) != pid);
	}
	cout<<"Finish SetIPStatic***************************************************************8\n";
}

PlayerMode_e GetPlayerMode(int argc, char *argv[])
{
	string userInput;
	PlayerMode_e playerMode;
	bool validInput = true;

	while(true)
	{
		if(argc == 1 || validInput == false)
		{
			cout << "Please choose which mode to run:" << endl;
			cout << "[debug] for debug mode" << endl;
			cout << "[striker] for striker mode" << endl;
			cout << "[goalie] for goal keeper mode" << endl;
			cin >> userInput;
		}
		else
		{
			userInput = argv[1];
		}
		//check which role to execute

		if(userInput.compare("debug") == 0 )
		{
			cout << "starting debug mode" << endl;
			playerMode = DEBUG_MODE;
			break;
		}
		else if(userInput.compare("striker") == 0)
		{
			cout << "starting striker mode" << endl;
			playerMode = STRIKER_MODE;
			break;
		}
		else if(userInput.compare("goalie") == 0)
		{
			cout << "starting goalie mode" << endl;
			playerMode = GOALIE_MODE;
			break;
		}
		else
		{
			cout << "Wrong input - invalid player mode" << endl;
			validInput = false;
		}
	}

	return playerMode;
}

/************************************************************************************************************
 *

                                                     `       `.-.`
                                                `-+oyo   `.:+ossso-
                                             `-+ssssoo-:/syyyyyysss/
                                           .:+oosssssyyyyyyyyyyyysso`
                                         ./osyyyhhdddmmmddhhhyyyyyo+-
                                       `:oyhddmmmmmmmmmmmmmmdhyyysoo:
                                      .+yhh+///ymmmmmmmmmmmmmdhyysso.
                                     -shmmh/--/sydmddmmmmmmmmdyyysso`
                                    `+oshd+//++oooooooshmmmmmhyyyyso
                                    -/sy+o+oooso/ohmdy++smmmdhyyyys+
                                    :+dNd/yhmdhs/hNNNNd/+dmmdyyyyys+
                                    -/hmm/hhmddy/+hmmmy/+ddmdhhyyyy/
                                    `/+o++dhhhhhyo/+++/oyhhddhyyyys.
                                     -ooshdddddddhhyyyhddhhdddhyys-
                                      -shddddddddddddddddmmmmmmh+.
                                       .+hmNNmmmNmmmmmNNNNNNmds-
                                         `-/odmmNmmmmmmmmdmm+.
                                       `````-sddmdhhsyhdddmmhyyyyyo//+oss+:`
                                    :/ossssshdmmmmdddddmmNNNNmmmmdyyhdmNNmmy:`
                                 `.+ydddmmmmmmNNNNNNNmNNNNNNNmmmmmmNmmNNNmhhmh+/.
                             `-:osdmdmmmmmmmmmmmmmmmmmmmmmmmmmmddNNmhdNNNNmdmdmmds.
                           .:shhhhNdmmmmmmmmmmmmmmmmmmmmmmmmmmmsmmmmhdNNNNNmdmddddd/`
                          +hhhmdhhdmmmmmmmmmmmmmmmmmmmmmmmmdhmsyNmmdmddmNmmmmmddhdds+-`
                         .dyyhdmdhhmmmmmmmmmmmmmmdddhhyysssoym/dNNmmNNmdddddhhmmdhh+oh+.
                         :d++ohhhddmmmmmdddhyssssoo++++++++odd+mmNNmmmmdyhyyhdhyhhdddddy:`
                         odho/ohmmhmmmmmy+//////////+++++ossmhymmmNNNNdhso+++ddhyydmmmddho.`
                         ydddddmNmdmmmmds+++++++++++ooooysosmysmmmmNNNNmdysshddddhshmmmdddys.
                         hdddddmmddmmdmho++ooooooooooooyssoymyhmmmmNNNNNmNNNmmmddddyydmmmdmmd/`
                        `hmdmmmmmmddmmdyooooooooosossssssshdmydmmmmmmNNNddmNmmmmmmmdhydmmmmyymy.
                       `+mNNNNNNmmdmmmdysoosssssssssssssdmdhdsdmmmmmmmmmdhhdmNNNNNNNNmmmsosdhhms
                      :yNNNNMMNNNNdmmmdyssssssssssyyyyyyhyhdmhdmmmmmmmdhdhyy/+yNNNNNNNNo+++ohmd:
                   `.:dNMNNNNNNNNmsymddhhdddddddddddddddddmmmdddmmmmddhhhhy:  sNNNNNNNNysyyyhh-
                 .+yhhdmNMmmNNNNsso+mmddddddhhhhhhhhhhhhhddddhdmmmmmmdhhhy-   /NNNNddNNNmdhhyh+
              `:oyhhhdmNNNNNNNNNydh+sNmddhhhhhhhhhhhhhhhhddhhdhmmmmmdhhhhy    -hNNNmdNNNNNdd/:h+
            `:syhhhhhmNNmmNNNmNdsh/`-mmddhhhhhhhhhhhhhhhhdddhhdmmmmmdhhso/     -mmhNNNNmmmmmmhdy`
          `/syhhhhhhmNNNssNNydmhoo sddmmddhhhhhhhhhhhdhyydmmddmmmddy:--`        :mhmmmmmmmmmmmdds
        ./sydmmmmdNNNNMNNNNNydmdo` yd/smsyhyyhmddmhyydssshhhhhhhssds            `+ymmdmmdmmmddmdd.
      `/yhdNNNNNNNNNNMMMNNNy+so.   ymmmmmmmooyNdhNdoodhsyhhhddhhdmds``.`          :dmmmmdmmmmdmmh`
     .hmmNNNmmNNNNNNNd+++o+`       hmdmmNNNNmNNmmNNNNNNNmhhmhdhdhdmmmhys.        :dmmmmddmmmmmmm/
    `odNNNmNNNNNNNNNs             `ymhdmsmNNmmsmNNNNNNNdmhhyhhhmNmdmmhod/       smmmmmmddmmmmmmm/
    .hNNNNdymNNNNNNNo            `ymsssyhmNNd+/NNNNNMNmdmyhooyyhNmNmmdodo      .mNmmmmddNmmmmmmd.
    .dNNNNdymNNNNms-             sNNNddmNNNhs+hNNNNNNNmNmh++/:/dNmNmmdoss      +Nmmddmhdmmmmmmmy
    -dNNNmhyhs///.              :NNNNNh+yNo/+yNNNNNNNNmNmNmdddNNNNmmmdosh      hmmdhdmymmmmmmmm:
    .yNmh+++`                  `dNNMNNhoyy::/dNNNNNNNNNNNNNNsoomNNNmmdo+h      .smyymdymmmmmmmd.
     `.`                       +mNNNNNmNd/::smNNNmmNysmNNNNN+-:mNNNmmdoys       shyhmhdmmmmmmms
                              .dmmmmmdydo///dmmNy.--``dmdmmmmydmNmmNNmo/`       /hydddmmmmmmmm:
                              ommmmmmsdd+yhhmmmm.    `hy+ooooodmmhhmNmo:        :dyddmmmmmmmmh`
                             -dmNmmmyhmy+/+dmmms      yyoo++oymmmhdNNmo:        -mhhmmmmmmmmm+
                             smNmmmdsmmdhhdmmmm:     `yyoooohdmmmhdNNNo/       -ymdhmmmmmmmmd.
                           `-mNNNNmyhmmddmmmmmm-      yyooohdmmmmhdNNNo+       `.+ddmmmmmmmmo
                         `sshNNNNNNNNNmmmmmmmmm:      yhooydmmmmmdhNNNoo.         ./shddmmmo`
                         -dmmNmmmmmmdddmmmmmmmy`      syydmmhhhNNmmNNNs+.           ``:+so-
                         :mmmmmmdmmdyyymNmmmmh.     `:hyhNNNmmmNNNmNNmo+.
                         +mmNmmmdmmdhyhmmdmmd-      `hhhdddmmmmmmmdmmmo+`
                         smmmmmmdddddddmmmmd:        yhhhhhddddmmmdmmmy+`
                         yddddmdhhdhyhhNmmm+         sdhhhhhhhhddmmmNNd+.
                         yddddddhhdddhdNNNNd-        /dhhhhdddhhhdddmmm+/`
                        `hdddddhhhdhhhhmNNNNm/`      -dhhhhhddddhhdhmmms+:
                        `hdddddhhhhyyyhmNNNNNNy-`    `ddhhhhhhhhhddhdddy++:`
                        `hhddddhhhhyyyhmNNNNNNNNo     hdhhhhdddddhdhdddhoo+/
                        .hhhdhdhhdhyyhhmNNNNNNNNm-    sdhhdddddddhhhhddds+o+`
                        -dddddddhdhhhhhmNNNNNNNNNy    +ddddddddddddhhddhy+oo.
                        -NNNmdddddhhhhhdNNNNNNNNNy    -mdddddddddddhhddhhooo:
                        -MNNm/:hNdhhhhhdNNNNNNNNNs    `ddddddddddddhhdhhhsoo+
                        .NNNNhymNddhhhhdNNNNNNNNN+     yddddddddmmmmdddhhyooo`
                        `dddmNNNNdmdddmdNNNNNNNNN/     +NNNNNNyssmNNNddhhhoos`
                         :/::sNNNmdhhmmmNNNNNNNNm-     -mNNNNN/--mNNNdmdmmyo+
                         od+oodNNNmhhmmmmmmdyo/:.      `dNmmdNmmdNNdNddddhh+.
                    ``.:/hhhddNNNdmdddmmmNdys+//`       sNmNms++odNNNNddhhy:
                `-/osyyhhmmmmdNNNNNNNNNNNNNNNNNN-       /NhNo+::--sNNNhddho```
              `+yhhhyyyyhmmNNmNNNNNNNNNNNNNNNmmh.       .yydyds+ysdNNdNdNdoo+++//.
              `yddddhhhhhhmmmmmmNNmmmmmddys+/-.            oddhyddmMNNNNNNdyososo:
               -/oyhddddddddddddhhso/:-`                ``/hmmNNmmmNNNNNNNmsososs-
                   `.-/oosyso/:.`                      /dmmNmmNNmNNNNNNNNNmhyoyo.
                                                      .dmmmmmmmmmmmNNmmmNmmhss:
                                                      .yddddddddmmmmmmmmmddy+`
                                                        .-://+oosyyhhdddhyy-
                                                                     ````

 * 								RoboCup - 2012 Mexico City
 * 								Bar Ilan University team
 * 								Israel
 *************************************************************************************************************/
