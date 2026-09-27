#ifndef IMU_H
#define IMU_H

#include <stdint.h>

typedef struct {
  uint8_t i2c_addr;
  //raw MPU6050 readings stored in these
  float ax, ay, az;
  float gx, gy, gz;
} IMU;

void imu_init(IMU *imu);
void imu_read(IMU *imu);

#endif
