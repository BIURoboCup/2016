#ifndef _VISION_ENUMS_H_
#define _VISION_ENUMS_H_

#include "ColorsValues.h"

typedef enum VisionInitializationsStatus_e {
	OPEN_VISION_DEBUG_WINDOWS = 0x01, SAVE_CAPTURED_VIDEO_STREAM = 0x02
} VisionInitializationsStatus_e;

typedef enum GoalPoleSide_e {
	POLE_SIDE_LEFT = 1,
	POLE_SIDE_RIGHT = 2,
	FACE_UP_TO_SEE_POLE_SIDE = 3,
	UNKNOWN_POLE_SIDE = 4
} GoalPoleSide_e;

typedef enum PoleType_e {
	YELLOW_GOAL = YELLOW_AVG_HUE, BLUE_GOAL = BLUE_AVG_HUE, BYB_POLE, YBY_POLE
} PoleType_e;

typedef enum HeadScanForGoal_e {
	NO_POLES,
	ONE_POLE_FACE_UP,
	ONE_POLE_LEFT_SIDE,
	ONE_POLE_RIGHT_SIDE,
	ONE_POLE_UNKNOWN_SIDE,
	TWO_POLES_FACE_UP_LEFT,
	TWO_POLES_FACE_UP_RIGHT,
	TWO_POLES_FACE_UP,
	TWO_POLES_NO_FACE_UP

} HeadScanForGoal_e;

typedef enum HeadTrackingStatus_e {
	BALL_TRACKING,
	GOAL_TRACKING,
	BALL_AND_GOAL_TRACKING,
	KICKING_BALL_TRACKING,
	UNKNOWN_TRACKING_STATUS
} HeadTrackingStatus_e;

#endif /* _VISION_ENUMS_H_ */
