#include <stdint.h>

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

typedef struct {
  uint8_t gpio_r_en;
  uint8_t gpio_l_en;
  uint8_t gpio_r_pwm;
  uint8_t gpio_l_pwm;
  uint8_t pwm_slice_r;
  uint8_t pwm_slice_l;
} Motor;

void motor_init(Motor *m);
void motor_set_speed(Motor *m, int speed); //speed: -100 to 100
void motor_stop(Motor *m);

#endif
