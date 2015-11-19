/*
 * GyroData.h
 *
 *  Created on: Dec 25, 2014
 *      Author: darwini5
 */

#ifndef GYRODATA_H_
#define GYRODATA_H_

#include "MX28.h"
#include "JointData.h"
#include "FSR.h"
#include "CM730.h"
#include "MotionStatus.h"

int compass_Gyro(Robot :: CM730 *cm730, int id);

#endif /* GYRODATA_H_ */
