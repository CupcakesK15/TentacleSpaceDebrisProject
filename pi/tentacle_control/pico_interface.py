import serial
from .enums import TentacleFlag, TentacleError

class PicoInterface: 
    def __init__(self, port): 
        self.port = port
        #currently waiting 100ms for timeout --> MIGHT CHANGE
        self.serial = serial.Serial(port, 115200, timeout = 0.1) 
    def send_command(self, command):
        pass

    def read_response(self):
        pass

    def curl(self, speed):
        pass

    def retract(self, speed):
        pass

    def set_baseline(self):
        pass

    def request_state(self):
        pass

    def heartbeat(self):
        pass