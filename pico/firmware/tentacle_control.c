#include "tentacle_control.h"
#include "sensor_fusion.h"
#include <math.h>
#include <stdbool.h>
#include "pico/stdlib.h"

void curl_to_target(Motor *motorA, Motor *motorB, Sensor *sensors, int count, float target) {
  while (true) {
    float curvature = fuse_curvature(sensors, count);
    float error = target - curvature;

    float speed = error * 200.0f; //gain tuning

    motor_set_speed(motorA, speed);
    motor_set_speed(motorB, speed);

    if (fabs(error) < 0.01f) {
      motor_stop(motorA);
      motor_stop(motorB);
      return;
    }

    sleep_ms(10);
  }
}
