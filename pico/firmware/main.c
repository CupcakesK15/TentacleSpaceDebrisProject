#include "comms.h"
#include "motor_control.h"
#include "pico/stdlib.h"
#include "sensor_read.h"

// ALL VALUES IN THESE INITIALIZATIONS ARE PLACEHOLDERS

Motor motorA = {
  .gpio_r_en = 2, 
  .gpio_l_en = 3, 
  .gpio_r_pwm = 4, 
  .gpio_l_pwm = 5
};

Motor motorB = {
  .gpio_r_en = 6, 
  .gpio_l_en = 7, 
  .gpio_r_pwm = 8, 
  .gpio_l_pwm = 9
};

IMU imu1 = {.i2c_addr = 0x68};
IMU imu2 = {.i2c_addr = 0x69};
IMU imu3 = {.i2c_addr = 0x68}; // if on a second I2C bus or a mux

Sensor sensors[] = {
  {SENSOR_IMU, &imu1}, 
  {SENSOR_IMU, &imu2}, 
  {SENSOR_IMU, &imu3}
};

int sensor_count = sizeof(sensors) / sizeof(Sensor);

int main() {
  stdio_init_all();

  //initializing subsystems
  motor_init(&motorA);
  motor_init(&motorB);
  for (int i = 0; i < sensor_count; i++) {
    sensor_init(&sensors[i]);
  }
  comms_init();

  while (1) { // main loop
    char buffer[64];

    int len = comms_read_line(buffer, sizeof(buffer));
    if (len > 0) {
      parse_command(buffer, &motorA, &motorB, sensors, sensor_count);
    }
  }
}
