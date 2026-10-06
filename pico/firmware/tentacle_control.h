#include "motor_control.h"
#include "sensor_read.h"

#ifndef TENTACLE_CONTROL_H
#define TENTACLE_CONTROL_H

void curl_to_target(Motor *motors, int motor_count, Sensor *sensors, int sensor_count, float target);

#endif
