from .pico_interface import PicoInterface
from .enums import TentacleFlag

TENSION_GRASP_THRESHOLD = 0.5  # placeholder — tune once hardware exists

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

    def align_to_object(self, obj) -> bool:
        """No-op placeholder: tentacles currently have no pan/tilt, so there's
        nothing to physically aim yet. Just confirms a real detection came through."""
        return obj is not None

    def verify_grasp(self) -> bool:
        states = self.get_all_states()
        for type_, data in states:
            if type_ != "STATE" or not isinstance(data, tuple):
                return False
            curvature, tension, flag = data
            if flag != TentacleFlag.OK or tension < TENSION_GRASP_THRESHOLD:
                return False
        return True
