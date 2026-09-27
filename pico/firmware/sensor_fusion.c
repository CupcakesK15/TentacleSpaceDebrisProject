#include "sensor_fusion.h"
#include <math.h>

float fuse_curvature(Sensor *sensors, int count) {
  float sum = 0.0f;

  for (int i = 0; i < count; i++) {
    sum += read_curvature(&sensors[i]);
  }

  return sum / count;
}

float fuse_tension(Sensor *sensors, int count) {
  float sum = 0.0f;

  for (int i = 0; i < count; i++) {
    sum += read_tension(&sensors[i]);
  }

  return sum / count;
}
