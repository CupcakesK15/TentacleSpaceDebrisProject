from .pico_interface import PicoInterface

class TentacleManager: 
    def __init__(self, ports):
        # ports is a list of all serial port strings
        self.tentacles = [PicoInterface(port) for port in ports]

    def curl_all(self, speed):
        return [t.curl(speed) for t in self.tentacles]


    def retract_all(self, speed):
        return [t.retract(speed) for t in self.tentacles]


    def set_all_baselines(self):
        return [t.set_baseline() for t in self.tentacles]


    def get_all_states(self):
        return [t.request_state() for t in self.tentacles]