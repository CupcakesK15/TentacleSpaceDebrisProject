#include "comms.h"
#include "hardware/uart.h"
#include "motor_control.h"
#include "pico/stdlib.h"
#include "sensor_fusion.h"
#include "tentacle_control.h"
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

void comms_init() {
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
}

int comms_read_line(char *buffer, int max_len) {
  int ind = 0;
  while (ind < max_len - 1) {
    if (uart_is_readable(UART_ID)) {
      char c = uart_getc(UART_ID);
      if (c == '\n') {
        buffer[ind] = '\0';
        return ind;
      }
      buffer[ind] = c;
      ind++;
    }
  }
  buffer[ind] = '\0';
  return ind;
}

void comms_send_ack(const char *cmd) { printf("ACK %s\n", cmd); }

void comms_send_error(const char *code) { printf("ERROR %s\n", code); }

void comms_send_state(float curvature, float tension, const char *flag) {
  printf("STATE %.3f %.3f %s\n", curvature, tension, flag);
}

void parse_command(const char *cmd, Motor *motors, int motor_count,
                   Sensor *sensors, int sensor_count) {
  if (strcmp(cmd, "STATE") == 0) {
    float curvature = fuse_curvature(sensors, sensor_count);
    float tension = fuse_tension(sensors, sensor_count);
    comms_send_state(curvature, tension, "OK");
  }
  else if (strncmp(cmd, "CURL", 5) == 0) {
    float target = atof(cmd + 5);
    curl_to_target(motors, motor_count, sensors, sensor_count, target);
    comms_send_ack("CURL_DONE");
  }
  else if (strcmp(cmd, "STOP") == 0) {
    for (int i = 0; i < motor_count; i++) {
      motor_stop(&motors[i]);
    }
    comms_send_ack("STOP");
  }
  else {
    comms_send_error("INVALID_CMD");
  }
}
