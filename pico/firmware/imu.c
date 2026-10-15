#include "imu.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdint.h>

// MPU6050
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B

#define I2C_TIMEOUT_US 5000

// Valid RP2040 I2C pin pairs: i2c0 -> GPIO 16/17, i2c1 -> GPIO 18/19.
// (GPIO 2-13 are used by the motor drivers.)
static void imu_setup_bus(i2c_inst_t *i2c) {
  uint sda = (i2c == i2c0) ? 16 : 18;
  uint scl = (i2c == i2c0) ? 17 : 19;

  i2c_init(i2c, 400000);
  gpio_set_function(sda, GPIO_FUNC_I2C);
  gpio_set_function(scl, GPIO_FUNC_I2C);
  gpio_pull_up(sda);
  gpio_pull_up(scl);
}

void imu_init(IMU *imu) {
  imu_setup_bus(imu->i2c);   // safe to call once per IMU on the same bus

  // wake up MPU6050
  uint8_t buf[2] = {MPU6050_PWR_MGMT_1, 0x00};
  i2c_write_timeout_us(imu->i2c, imu->i2c_addr, buf, 2, false, I2C_TIMEOUT_US);
}

// If the IMU doesn't answer, the previous values are kept instead of
// storing garbage from an uninitialised buffer.
void imu_read(IMU *imu) {
  uint8_t reg = MPU6050_ACCEL_XOUT_H;
  uint8_t data[14];

  int w = i2c_write_timeout_us(imu->i2c, imu->i2c_addr, &reg, 1, true, I2C_TIMEOUT_US);
  if (w < 0) return;
  int r = i2c_read_timeout_us(imu->i2c, imu->i2c_addr, data, 14, false, I2C_TIMEOUT_US);
  if (r < 0) return;

  // accel
  imu->ax = (int16_t)(data[0] << 8 | data[1]);
  imu->ay = (int16_t)(data[2] << 8 | data[3]);
  imu->az = (int16_t)(data[4] << 8 | data[5]);

  // gyro (data[6..7] is temperature)
  imu->gx = (int16_t)(data[8] << 8 | data[9]);
  imu->gy = (int16_t)(data[10] << 8 | data[11]);
  imu->gz = (int16_t)(data[12] << 8 | data[13]);
}
