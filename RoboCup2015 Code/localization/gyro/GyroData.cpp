/*
 * GyroData.cpp
 *
 *  Created on: Dec 25, 2014
 *      Author: darwini5
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "GyroData.h"

using namespace Robot;

int compass_Gyro(CM730 *cm730, int id)
{
	unsigned char table[128];
	int addr;
	int value_Gyro_X; //,value_Gyro_Y,value_Gyro_Z,value_ACC_X,value_ACC_Y,value_ACC_Z;
	int gyroValue = 0;
    int offset = 512;
    double angle = 0;
	double gyroRate;

	while(1)
	{
		if(id == CM730::ID_CM) // Sub board
		{
			if(cm730 -> ReadWord(id,CM730::P_GYRO_X_L,&value_Gyro_X,0) != CM730::SUCCESS)
			//if(cm730 -> ReadTable(id, CM730::P_MODEL_NUMBER_L, CM730::P_RIGHT_MIC_H, &table[CM730::P_MODEL_NUMBER_L], 0) != CM730::SUCCESS)
			{
				printf(" Can not read table!\n");
				return(-1);
			}

			printf( "\n" );
			printf( " [EEPROM AREA]\n" );

			//addr = CM730::P_GYRO_Z_L; value_Gyro_Z = CM730::MakeWord(table[addr], table[addr+1]);
			//printf( " GYRO_Z                  (R) [%.3d]:%5d (L:0x%.2X H:0x%.2X)\n", addr, value_Gyro_Z, table[addr], table[addr+1]);
			//addr = CM730::P_GYRO_Y_L; value_Gyro_Y = CM730::MakeWord(table[addr], table[addr+1]);
			//printf( " GYRO_Y                  (R) [%.3d]:%5d (L:0x%.2X H:0x%.2X)\n", addr, value_Gyro_Y, table[addr], table[addr+1]);
			//addr = CM730::P_GYRO_X_L; value_Gyro_X = CM730::MakeWord(table[addr], table[addr+1]);
			//printf( " GYRO_X                  (R) [%.3d]:%5d (L:0x%.2X H:0x%.2X)\n", addr, value_Gyro_X, table[addr], table[addr+1]);
			//addr = CM730::P_ACCEL_X_L; value_ACC_X = CM730::MakeWord(table[addr], table[addr+1]);
			//printf( " ACCEL_X                 (R) [%.3d]:%5d (L:0x%.2X H:0x%.2X)\n", addr, value_ACC_X, table[addr], table[addr+1]);
			//addr = CM730::P_ACCEL_Y_L; value_ACC_Y = CM730::MakeWord(table[addr], table[addr+1]);
			//printf( " ACCEL_Y                 (R) [%.3d]:%5d (L:0x%.2X H:0x%.2X)\n", addr, value_ACC_Y, table[addr], table[addr+1]);
			//addr = CM730::P_ACCEL_Z_L; value_ACC_Z = CM730::MakeWord(table[addr], table[addr+1]);
			//printf( " ACCEL_Z                 (R) [%.3d]:%5d (L:0x%.2X H:0x%.2X)\n", addr, value_ACC_Z, table[addr], table[addr+1]);
		    //printf( "\n" );

		    gyroValue = value_Gyro_X;
		    printf("gyroValue: %d\n",gyroValue);

		    gyroRate = (gyroValue - offset) / 3.2; // Sensitivity from data-sheet
		    printf("gyroRate:  %f\n",gyroRate);
		    angle = angle + (gyroRate / 100.0);
		    printf("angle:     %f\n",angle);
		    printf("%5f\n",angle);
		    printf("\n");

		    sleep(0.001);
		}
	}
	return(0);

}

