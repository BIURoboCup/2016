/*
 *   MotionStatus.h
 *
 *   Author: ROBOTIS
 *
 */

#ifndef _MOTION_STATUS_H_
#define _MOTION_STATUS_H_

#include "JointData.h"


namespace Robot
{
    enum {
        BACKWARD    = -1,
        STANDUP     = 0,
        FORWARD     = 1,
        RIGHT		= 2,
        LEFT		= -2
    };

	class MotionStatus
	{
	private:

	public:
	    static const int FALLEN_F_LIMIT     = 400;
	    static const int FALLEN_B_LIMIT     = 600;
	    static const int FALLEN_R_LIMIT     = 412;
	    static const int FALLEN_L_LIMIT     = 612;
	    static const int FALLEN_MAX_COUNT   = 30;


		static JointData m_CurrentJoints;
		static int FB_GYRO;
		static int RL_GYRO;
		static int FB_ACCEL;
		static int RL_ACCEL;

		static int BUTTON;
		static int FALLEN;
	};
}

#endif
