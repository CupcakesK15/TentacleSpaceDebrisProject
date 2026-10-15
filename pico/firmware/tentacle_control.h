#include "motor_control.h"
#include "sensor_read.h"

#ifndef TENTACLE_CONTROL_H
#define TENTACLE_CONTROL_H

typedef enum { TC_IDLE, TC_MOVING, TC_DONE, TC_TIMEOUT } TcStatus;

void tc_start_move(float target, uint32_t timeout_ms);
void tc_manual(Motor *motors, int motor_count, int index, int speed);
void tc_stop(Motor *motors, int motor_count);
void tc_heartbeat(void);
void tc_update(Motor *motors, int motor_count, Sensor *sensors, int sensor_count);
TcStatus tc_status(void);
const char *tc_status_str(void);

#endif
