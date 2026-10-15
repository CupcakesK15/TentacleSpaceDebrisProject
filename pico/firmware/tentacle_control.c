#include "tentacle_control.h"
#include "sensor_fusion.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdbool.h>

#define GAIN 200.0f
#define TOLERANCE 0.01f
#define MAX_SPEED 100
#define MANUAL_TIMEOUT_MS 2000
#define HEARTBEAT_TIMEOUT_MS 1000

typedef enum { MODE_IDLE, MODE_MOVE, MODE_MANUAL } Mode;

static Mode mode = MODE_IDLE;
static TcStatus status = TC_IDLE;
static float target = 0.0f;
static absolute_time_t deadline, next_step, hb_deadline;
static bool hb_armed = false;

static int clamp_speed(float s) {
  if (s > MAX_SPEED) return MAX_SPEED;
  if (s < -MAX_SPEED) return -MAX_SPEED;
  return (int)s;
}

static void stop_all(Motor *motors, int n) {
  for (int i = 0; i < n; i++) motor_stop(&motors[i]);
}

void tc_start_move(float t, uint32_t timeout_ms) {
  target = t;
  deadline = make_timeout_time_ms(timeout_ms);
  next_step = get_absolute_time();
  mode = MODE_MOVE;
  status = TC_MOVING;
}

void tc_manual(Motor *motors, int n, int index, int speed) {
  if (index < 0 || index >= n) return;
  mode = MODE_MANUAL;
  status = TC_IDLE;
  deadline = make_timeout_time_ms(MANUAL_TIMEOUT_MS);
  motor_set_speed(&motors[index], clamp_speed(speed));
}

void tc_stop(Motor *motors, int n) {
  stop_all(motors, n);
  mode = MODE_IDLE;
  status = TC_IDLE;
}

void tc_heartbeat(void) {
  hb_armed = true;  // watchdog only starts after the first heartbeat
  hb_deadline = make_timeout_time_ms(HEARTBEAT_TIMEOUT_MS);
}

void tc_update(Motor *motors, int n, Sensor *sensors, int sensor_count) {
  if (mode == MODE_IDLE) return;

  // Pi went quiet: stop and flag it
  if (hb_armed && time_reached(hb_deadline)) {
    tc_stop(motors, n);
    status = TC_TIMEOUT;
    return;
  }

  if (mode == MODE_MANUAL) {
    if (time_reached(deadline)) tc_stop(motors, n);
    return;
  }

  // MODE_MOVE
  if (time_reached(deadline)) {
    stop_all(motors, n);
    mode = MODE_IDLE;
    status = TC_TIMEOUT;
    return;
  }
  if (!time_reached(next_step)) return;
  next_step = make_timeout_time_ms(10);

  float error = target - fuse_curvature(sensors, sensor_count);
  if (fabsf(error) < TOLERANCE) {
    stop_all(motors, n);
    mode = MODE_IDLE;
    status = TC_DONE;
    return;
  }
  int speed = clamp_speed(error * GAIN);
  for (int i = 0; i < n; i++) motor_set_speed(&motors[i], speed);
}

TcStatus tc_status(void) { return status; }

const char *tc_status_str(void) {
  switch (status) {
    case TC_MOVING:  return "MOVING";
    case TC_DONE:    return "DONE";
    case TC_TIMEOUT: return "TIMEOUT";
    default:         return "IDLE";
  }
}
