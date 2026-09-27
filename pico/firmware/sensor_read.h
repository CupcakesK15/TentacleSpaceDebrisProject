#include <stdint.h>
#include "imu.h"

#ifndef SENSOR_READ_H
#define SENSOR_READ_H

typedef enum {
    SENSOR_IMU
} SensorType;

typedef struct {
    SensorType type;
    void *data; //points to sensor specific struct
} Sensor;


void sensor_init(Sensor *sensor);
float read_curvature(Sensor *sensor);
float read_tension(Sensor *sensor);

#endif