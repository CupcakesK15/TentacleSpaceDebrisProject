#include "motor_control.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"


void motor_init(Motor *m) {
  gpio_init(m->gpio_r_en);
  gpio_init(m->gpio_l_en);
  gpio_set_dir(m->gpio_r_en, GPIO_OUT);
  gpio_set_dir(m->gpio_l_en, GPIO_OUT);

  gpio_init(m->gpio_r_pwm);
  gpio_init(m->gpio_l_pwm);
  gpio_set_function(m->gpio_r_pwm, GPIO_FUNC_PWM);
  gpio_set_function(m->gpio_l_pwm, GPIO_FUNC_PWM);
  m->pwm_slice_r = pwm_gpio_to_slice_num(m->gpio_r_pwm);
  m->pwm_slice_l = pwm_gpio_to_slice_num(m->gpio_l_pwm);
  for (int i = 0; i < 2; i++) {
    uint s = i ? m->pwm_slice_l : m->pwm_slice_r;
    pwm_set_wrap(s, 100);          // level 0-100 = percent duty
    pwm_set_clkdiv(s, 50.0f);      // ~25 kHz
    pwm_set_enabled(s, true);
  }
}

void motor_set_speed(Motor *m, int speed) {
  if (speed > 0) {
    speed = speed < 100 ? speed : 100;
    gpio_put(m->gpio_r_en, 1);
    gpio_put(m->gpio_l_en, 1);
    pwm_set_gpio_level(m->gpio_r_pwm, speed);
    pwm_set_gpio_level(m->gpio_l_pwm, 0);
  } 
  else if (speed < 0) {
    speed = speed > -100 ? speed : -100;
    gpio_put(m->gpio_r_en, 1);
    gpio_put(m->gpio_l_en, 1);
    pwm_set_gpio_level(m->gpio_r_pwm, 0);
    pwm_set_gpio_level(m->gpio_l_pwm, -speed);
  } else {
    motor_stop(m);
  }
}

void motor_stop(Motor *m) {
  gpio_put(m->gpio_r_en, 1);
  gpio_put(m->gpio_l_en, 1);
  pwm_set_gpio_level(m->gpio_r_pwm, 0);
  pwm_set_gpio_level(m->gpio_l_pwm, 0);
}
