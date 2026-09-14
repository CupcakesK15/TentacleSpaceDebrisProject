from .pico_interface import PicoInterface

class TentacleManager: 
    def __init__(self, ports):
        # ports is a list of all serial port strings
        self.tentacles = [PicoInterface(port) for port in ports]

    def curl_all(self, speed):
        pass

    def retract_all(self, speed):
        pass

    def set_all_baselines(self):
        pass

    def get_all_states(self):
        pass