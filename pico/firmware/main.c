#include "comms.h"
#include "motor_control.h"
#include "pico/stdlib.h"
#include "sensor_read.h"
#include "tentacle_control.h"

// IMUs (GY-521 / MPU-6050)
//   i2c0 -> GPIO 16 (SDA) / 17 (SCL): imu1 (AD0 low, 0x68) and imu2 (AD0 tied to 3V3, 0x69)
//   i2c1 -> GPIO 18 (SDA) / 19 (SCL): imu3 (0x68)
IMU imu1 = {.i2c = i2c0, .i2c_addr = 0x68};
IMU imu2 = {.i2c = i2c0, .i2c_addr = 0x69};
IMU imu3 = {.i2c = i2c1, .i2c_addr = 0x68};

Sensor sensors[] = {
  {SENSOR_IMU, &imu1},
  {SENSOR_IMU, &imu2},
  {SENSOR_IMU, &imu3}
};

// A4988 drivers. Each STEP pin must be on a different PWM slice
// (GPIO 2 -> slice 1, GPIO 5 -> slice 2, GPIO 8 -> slice 4).
// If you wire different GPIOs, change the numbers here and reflash.
// Set .invert = 1 on a motor to reverse its direction without rewiring.
Motor motors[] = {
  {.gpio_step = 2, .gpio_dir = 3, .gpio_en = 4},
  {.gpio_step = 5, .gpio_dir = 6, .gpio_en = 7},
  {.gpio_step = 8, .gpio_dir = 9, .gpio_en = 10}
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
