#include "motor_control.h"
#include "sensor_read.h"

#ifndef COMMS_H
#define COMMS_H

void comms_init();
int comms_read_line(char *buffer, int max_len);

void comms_send_ack(const char *cmd);
void comms_send_error(const char *code);
void comms_send_state(float curvature, float tension, const char *flag, const char *status);
void parse_command(const char *cmd, Motor *motors, int motor_control,
                   Sensor *sensors, int sensor_count);

#endif
