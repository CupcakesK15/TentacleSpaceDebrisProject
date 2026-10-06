#include "tentacle_control.h"
#include "sensor_fusion.h"
#include <math.h>
#include <stdbool.h>
#include "pico/stdlib.h"

void curl_to_target(Motor *motors, int motor_count, Sensor *sensors, int sensor_count, float target) {
  while (true) {
    float curvature = fuse_curvature(sensors, sensor_count);
    float error = target - curvature;

    float speed = error * 200.0f; //gain tuning

    for (int i = 0; i < motor_count; i++) {
      motor_set_speed(&motors[i], speed);
    }


    if (fabs(error) < 0.01f) {
      for (int i = 0; i < motor_count; i++) {
        motor_stop(&motors[i]);
      }
      return;
    }

    sleep_ms(10);
  }
}
