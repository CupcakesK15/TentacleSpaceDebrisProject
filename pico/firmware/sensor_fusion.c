#include "sensor_fusion.h"
#include "pico/stdlib.h"
#include <math.h>

static float curv_offset = 0.0f;
static float tension_offset = 0.0f;

static float raw_curvature(Sensor *s, int n) {
  float sum = 0.0f;
  for (int i = 0; i < n; i++) sum += read_curvature(&s[i]);
  return sum / n;
}

static float raw_tension(Sensor *s, int n) {
  float sum = 0.0f;
  for (int i = 0; i < n; i++) sum += read_tension(&s[i]);
  return sum / n;
}

float fuse_curvature(Sensor *s, int n) { return raw_curvature(s, n) - curv_offset; }
float fuse_tension(Sensor *s, int n)   { return raw_tension(s, n) - tension_offset; }

void fusion_set_baseline(Sensor *s, int n) {
  const int SAMPLES = 20;
  float c = 0.0f, t = 0.0f;
  for (int i = 0; i < SAMPLES; i++) {
    c += raw_curvature(s, n);
    t += raw_tension(s, n);
    sleep_ms(10);
  }
  curv_offset = c / SAMPLES;
  tension_offset = t / SAMPLES;
}
