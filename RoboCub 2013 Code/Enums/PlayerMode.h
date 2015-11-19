/*
 * PlayerMode.h
 *
 *  Created on: Apr 9, 2012
 *      Author: robocup
 */

#ifndef PLAYERMODE_H_
#define PLAYERMODE_H_


typedef enum PlayerMode_e
{
	STRIKER_MODE,
	GOALIE_MODE,
	DEBUG_MODE
} PlayerMode_t;

typedef enum RobotID_e
{
	ID_STRIKER1,
	ID_STRIKER2,
	ID_GOALIE,
	ID_UNKNOWN
} RobotID_e;

#endif /* PLAYERMODE_H_ */
