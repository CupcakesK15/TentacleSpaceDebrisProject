#ifndef IMU_H
#define IMU_H

#include <stdint.h>
#include "hardware/i2c.h"

typedef struct {
  i2c_inst_t *i2c;
  uint8_t i2c_addr;
  //raw MPU6050 readings stored in these
  float ax, ay, az;
  float gx, gy, gz;
} IMU;

void imu_init(IMU *imu);
void imu_read(IMU *imu);

#endif
