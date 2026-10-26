#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>

// ---- A4988 / NEMA 17 settings -------------------------------------------
// MOTOR_MICROSTEP must match how MS1/MS2/MS3 are wired on the driver:
//   all unconnected = 1, all tied to 3V3 = 16 (1/2 = 2, 1/4 = 4, 1/8 = 8).
// The firmware can't see those pins, so if you rewire them, change this and reflash.
#define MOTOR_STEPS_PER_REV 200   // standard NEMA 17 (1.8 deg per full step)
#define MOTOR_MICROSTEP     16
#define MOTOR_MAX_RPS       2     // shaft speed when speed = +/-100 (revolutions/second)

#define MOTOR_MAX_STEP_HZ (MOTOR_STEPS_PER_REV * MOTOR_MICROSTEP * MOTOR_MAX_RPS)

// 0 = keep the coils energised after a stop (holds position, driver gets warm)
// 1 = release the motor on stop (cool, but the tendon can unwind)
#define MOTOR_DISABLE_ON_STOP 0

typedef struct {
  uint8_t gpio_step;   // A4988 STEP (one pulse = one microstep)
  uint8_t gpio_dir;    // A4988 DIR
  uint8_t gpio_en;     // A4988 ENABLE (active low)
  uint8_t invert;      // set to 1 to flip this motor's direction in software
  // filled in by motor_init / motor_set_speed
  uint8_t pwm_slice;
  int last_speed;
} Motor;

void motor_init(Motor *m);
void motor_set_speed(Motor *m, int speed); // speed: -100 to 100 (sign = direction, magnitude = step rate)
void motor_stop(Motor *m);

#endif
