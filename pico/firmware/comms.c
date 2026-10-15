#include "comms.h"
#include "motor_control.h"
#include "pico/stdlib.h"
#include "sensor_fusion.h"
#include "tentacle_control.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LINE_BUF_SIZE 64

// USB serial is set up by stdio_init_all() in main(), so nothing to do here.
// Requires pico_enable_stdio_usb(<target> 1) in CMakeLists.txt.
void comms_init(void) {}

// Non-blocking: returns the length of a complete line (copied into `out`),
// or 0 if no full line has arrived yet. The partial line is kept internally,
// so the caller's buffer doesn't need to persist between calls.
int comms_read_line(char *out, int max_len) {
  static char line[LINE_BUF_SIZE];
  static int len = 0;
  int c;

  while ((c = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
    if (c == '\r') continue;
    if (c == '\n') {
      if (len == 0) continue;               // ignore blank lines
      int n = len < max_len - 1 ? len : max_len - 1;
      memcpy(out, line, n);
      out[n] = '\0';
      len = 0;
      return n;
    }
    if (len < LINE_BUF_SIZE - 1) {
      line[len++] = (char)c;
    } else {
      len = 0;                              // overflow: drop the line
    }
  }
  return 0;
}

void comms_send_ack(const char *cmd) { printf("ACK %s\n", cmd); }

void comms_send_error(const char *code) { printf("ERROR %s\n", code); }

void comms_send_state(float curvature, float tension, const char *flag, const char *status) {
  printf("STATE %.3f %.3f %s %s\n", curvature, tension, flag, status);
}

void parse_command(const char *cmd, Motor *motors, int motor_count,
                   Sensor *sensors, int sensor_count) {
  char line[LINE_BUF_SIZE];
  strncpy(line, cmd, sizeof(line) - 1);
  line[sizeof(line) - 1] = '\0';
  char *name = strtok(line, " ");
  char *a1 = strtok(NULL, " ");
  char *a2 = strtok(NULL, " ");

  if (!name) { comms_send_error("INVALID_CMD"); return; }

  if (strcmp(name, "REQUEST_STATE") == 0 || strcmp(name, "STATE") == 0) {
    float c = fuse_curvature(sensors, sensor_count);
    float t = fuse_tension(sensors, sensor_count);
    const char *flag = (tc_status() == TC_TIMEOUT) ? "ERR" : "OK";
    comms_send_state(c, t, flag, tc_status_str());
  }
  else if (strcmp(name, "CURL") == 0 && a1) {
    tc_start_move(strtof(a1, NULL), 5000);
    comms_send_ack("CURL");
  }
  else if (strcmp(name, "RETRACT") == 0) {
    tc_start_move(0.0f, 5000);
    comms_send_ack("RETRACT");
  }
  else if (strcmp(name, "SET_BASELINE") == 0) {
    tc_stop(motors, motor_count);
    fusion_set_baseline(sensors, sensor_count);
    comms_send_ack("SET_BASELINE");
  }
  else if (strcmp(name, "HEARTBEAT") == 0) {
    tc_heartbeat();
    comms_send_ack("HEARTBEAT");
  }
  else if (strcmp(name, "MOTOR") == 0 && a1 && a2) {
    int idx = atoi(a1);
    if (idx < 0 || idx >= motor_count) { comms_send_error("INVALID_CMD"); return; }
    tc_manual(motors, motor_count, idx, atoi(a2));
    comms_send_ack("MOTOR");
  }
  else if (strcmp(name, "STOP") == 0) {
    tc_stop(motors, motor_count);
    comms_send_ack("STOP");
  }
  else {
    comms_send_error("INVALID_CMD");
  }
}
