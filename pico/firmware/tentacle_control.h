#include "motor_control.h"
#include "sensor_read.h"

#ifndef TENTACLE_CONTROL_H
#define TENTACLE_CONTROL_H

void curl_to_target(Motor *motorA, Motor *motorB, Sensor *sensors, int count, float target);

#endif
