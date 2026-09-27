#include "imu.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <hardware/structs/io_bank0.h>
#include <stdint.h>

//MPU6050
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B

void imu_init(IMU *imu) {
  //initialize I2C
  i2c_init(i2c0, 400000);
  gpio_set_function(4, GPIO_FUNC_I2C); // SDA
  gpio_set_function(5, GPIO_FUNC_I2C); // SCL
  gpio_pull_up(4);
  gpio_pull_up(5);

  //wake up MPU6050
  uint8_t buf[2];
  buf[0] = MPU6050_PWR_MGMT_1;
  buf[1] = 0x00;

  i2c_write_blocking(i2c0, imu->i2c_addr, buf, 2, false);
}

void imu_read(IMU *imu) {
  uint8_t reg = MPU6050_ACCEL_XOUT_H;
  uint8_t data[14];

  //read accel + gyro registers
  i2c_write_blocking(i2c0, imu->i2c_addr, &reg, 1, true);
  i2c_read_blocking(i2c0, imu->i2c_addr, data, 14, false);

  //unpack accel
  imu->ax = (int16_t)(data[0] << 8 | data[1]);
  imu->ay = (int16_t)(data[2] << 8 | data[3]);
  imu->az = (int16_t)(data[4] << 8 | data[5]);

  //unpack gyro
  imu->gx = (int16_t)(data[8] << 8 | data[9]);
  imu->gy = (int16_t)(data[10] << 8 | data[11]);
  imu->gz = (int16_t)(data[12] << 8 | data[13]);
}
