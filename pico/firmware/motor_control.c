#include "motor_control.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

// Step pulses come from the PWM hardware at 50% duty, so the CPU never has to
// bit-bang them. Each STEP pin must be on its own PWM slice (the pin table in
// main.c does that: GPIO 2, 5, 8 -> slices 1, 2, 4).
//
// The PWM clock is divided by a fixed 64, giving a ~1.95 MHz tick. The step
// frequency is then set through the 16-bit wrap value:
//   step_hz = tick_hz / (wrap + 1)
// That covers roughly 30 Hz to 1 MHz, plenty for the A4988.
#define PWM_CLKDIV 64.0f

void motor_init(Motor *m) {
  // Driver disabled first (ENABLE is active low), so nothing moves during setup.
  gpio_init(m->gpio_en);
  gpio_set_dir(m->gpio_en, GPIO_OUT);
  gpio_put(m->gpio_en, 1);

  gpio_init(m->gpio_dir);
  gpio_set_dir(m->gpio_dir, GPIO_OUT);
  gpio_put(m->gpio_dir, 0);

  gpio_set_function(m->gpio_step, GPIO_FUNC_PWM);
  m->pwm_slice = pwm_gpio_to_slice_num(m->gpio_step);
  pwm_set_clkdiv(m->pwm_slice, PWM_CLKDIV);
  pwm_set_wrap(m->pwm_slice, 1000);
  pwm_set_gpio_level(m->gpio_step, 0);   // level 0 = output stays low = no steps
  pwm_set_enabled(m->pwm_slice, true);

  m->last_speed = 0;
}

void motor_set_speed(Motor *m, int speed) {
  if (speed > 100) speed = 100;
  if (speed < -100) speed = -100;

  if (speed == 0) {
    motor_stop(m);
    return;
  }
  if (speed == m->last_speed) return;   // nothing to change; avoids glitching the pulse train

  int forward = (speed > 0) != (m->invert != 0);
  int mag = speed > 0 ? speed : -speed;

  gpio_put(m->gpio_dir, forward ? 1 : 0);
  gpio_put(m->gpio_en, 0);              // energise the driver

  uint32_t step_hz = (uint32_t)MOTOR_MAX_STEP_HZ * (uint32_t)mag / 100u;
  if (step_hz < 1) step_hz = 1;

  uint32_t tick_hz = (uint32_t)(clock_get_hz(clk_sys) / PWM_CLKDIV);
  uint32_t period = tick_hz / step_hz;  // = wrap + 1
  if (period > 65536u) period = 65536u;
  if (period < 4u) period = 4u;

  uint16_t wrap = (uint16_t)(period - 1);
  pwm_set_wrap(m->pwm_slice, wrap);
  pwm_set_gpio_level(m->gpio_step, period / 2);   // 50% duty

  // If the counter is already past the new wrap it would run up to 65535
  // before rolling over (a long gap with no steps), so restart it.
  if (pwm_get_counter(m->pwm_slice) >= wrap) pwm_set_counter(m->pwm_slice, 0);

  m->last_speed = speed;
}

void motor_stop(Motor *m) {
  pwm_set_gpio_level(m->gpio_step, 0);   // no more pulses
  m->last_speed = 0;
#if MOTOR_DISABLE_ON_STOP
  gpio_put(m->gpio_en, 1);
#endif
}
