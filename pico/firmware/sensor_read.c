#include "sensor_read.h"
#include "imu.h"
#include <math.h>

void sensor_init(Sensor *sensor) {
  switch (sensor->type) {
    case SENSOR_IMU:
      imu_init((IMU *)sensor->data);
  }
}

float read_curvature(Sensor *sensor) {
  IMU *imu = (IMU *)(sensor->data);
  imu_read(imu);

  //compute curvature from IMU orientation
  return imu->gx * 0.001f; //placeholder
}

float read_tension(Sensor *sensor) {
  IMU *imu = (IMU *)(sensor->data);
  imu_read(imu);

  float mag =
      sqrtf((imu->ax * imu->ax) + (imu->ay * imu->ay) + (imu->az * imu->az));
  return mag * 0.001f;
}
