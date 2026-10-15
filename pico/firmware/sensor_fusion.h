#include "sensor_read.h"

#ifndef SENSOR_FUSION_H
#define SENSOR_FUSION_H

float fuse_curvature(Sensor *sensors, int sensor_count);
float fuse_tension(Sensor *sensors, int sensor_count);
void fusion_set_baseline(Sensor *sensors, int sensor_count);

#endif 
