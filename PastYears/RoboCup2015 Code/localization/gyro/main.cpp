#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "GyroData.h"
#include "LinuxCM730.h"

#define PROGRAM_VERSION		"v1.00"
#define MAXNUM_INPUTCHAR	(128)

using namespace Robot;

LinuxCM730 linux_cm730("/dev/ttyUSB0");
CM730 cm730(&linux_cm730);


int gID = CM730::ID_CM;

void sighandler(int sig)
{
	exit(0);
}

int main()
{
	signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
	signal(SIGQUIT, &sighandler);
	signal(SIGINT, &sighandler);

	char input[MAXNUM_INPUTCHAR];
	char *token;
	int input_len;
	char cmd[80];
	char param[20][30];
	int num_param;



	printf( "\n[Dynamixel Monitor for DARwIn %s]\n", PROGRAM_VERSION);

	if(cm730.Connect() == true)
	{
		while(1)
		{
			gets(input);
			fflush(stdin);
			input_len = strlen(input);
			if(input_len == 0)
				continue;
			token = strtok( input, " " );
			if(token == 0)
				continue;
			strcpy( cmd, token );
			token = strtok( 0, " " );
			num_param = 0;
			while(token != 0)
			{
				strcpy( param[num_param++], token );
				token = strtok( 0, " " );
			}
			if(strcmp(cmd, "d") == 0)
				compass_Gyro(&cm730, gID);
		}
	}
	printf("\nTerminated DXL Manager.\n");
	return 0;
}

