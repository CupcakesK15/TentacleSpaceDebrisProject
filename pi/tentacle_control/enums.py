from enum import Enum

class TentacleError(Enum):
    NONE = 0
    MOTOR_FAULT = 1
    SENSOR_FAULT = 2
    INVALID_CMD = 3
    TIMEOUT = 4

class TentacleFlag(Enum): 
    OK = 0
    WARN = 1
    ERR = 2