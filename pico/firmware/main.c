#include "comms.h"
#include "motor_control.h"
#include "pico/stdlib.h"
#include "sensor_read.h"
#include "tentacle_control.h"

// ALL VALUES IN THESE INITIALIZATIONS ARE PLACEHOLDERS

IMU imu1 = {.i2c = i2c0, .i2c_addr = 0x68};
IMU imu2 = {.i2c = i2c0, .i2c_addr = 0x69};
IMU imu3 = {.i2c = i2c1, .i2c_addr = 0x68}; // if on a second I2C bus or a mux

Sensor sensors[] = {
  {SENSOR_IMU, &imu1}, 
  {SENSOR_IMU, &imu2}, 
  {SENSOR_IMU, &imu3}
};

Motor motors[] = {
  {.gpio_r_en = 2, .gpio_l_en = 3, .gpio_r_pwm = 4, .gpio_l_pwm = 5}, 
  {.gpio_r_en = 6, .gpio_l_en = 7, .gpio_r_pwm = 8, .gpio_l_pwm = 9}, 
  {.gpio_r_en = 10, .gpio_l_en = 11, .gpio_r_pwm = 12, .gpio_l_pwm = 13}
};

int sensor_count = sizeof(sensors) / sizeof(Sensor);
int motor_count = sizeof(motors) / sizeof(Motor);

int main() {
  stdio_init_all();

  //initializing subsystems
  for (int i = 0; i < motor_count; i++) {
    motor_init(&motors[i]);
  }
  for (int i = 0; i < sensor_count; i++) {
    sensor_init(&sensors[i]);
  }
  comms_init();

  while (1) { // main loop
    char buffer[64];

    int len = comms_read_line(buffer, sizeof(buffer));
    if (len > 0) {
      parse_command(buffer, motors, motor_count, sensors, sensor_count);
    }

    tc_update(motors, motor_count, sensors, sensor_count);
  }
}
