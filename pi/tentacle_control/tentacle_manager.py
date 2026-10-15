from .pico_interface import PicoInterface
from .enums import TentacleFlag

TENSION_GRASP_THRESHOLD = 0.5  # placeholder — tune once hardware exists


class TentacleManager:
    def __init__(self, ports):
        # ports is a list of all serial port strings
        self.tentacles = [PicoInterface(port) for port in ports]

    # ---- lifecycle ----------------------------------------------------
    def start_heartbeats(self):
        for t in self.tentacles:
            t.start_heartbeat()

    def close(self):
        for t in self.tentacles:
            t.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    # ---- commands (all Picos run independently, so these start together)
    def curl_all(self, target):
        return [t.curl(target) for t in self.tentacles]

    def retract_all(self):
        return [t.retract() for t in self.tentacles]

    def stop_all(self):
        return [t.stop() for t in self.tentacles]

    def wait_all(self, timeout=6.0):
        """Returns a list of TentacleStatus, one per tentacle."""
        return [t.wait_until_done(timeout) for t in self.tentacles]

    def set_all_baselines(self):
        return [t.set_baseline() for t in self.tentacles]

    def get_all_states(self):
        return [t.request_state() for t in self.tentacles]

    # ---- grasp helpers ------------------------------------------------
    def align_to_object(self, obj) -> bool:
        """No-op placeholder: tentacles currently have no pan/tilt, so there's
        nothing to physically aim yet. Just confirms a real detection came through."""
        return obj is not None

    def verify_grasp(self) -> bool:
        states = self.get_all_states()
        for type_, data in states:
            if type_ != "STATE" or not isinstance(data, tuple):
                return False
            _curvature, tension, flag, _status = data
            if flag != TentacleFlag.OK or tension < TENSION_GRASP_THRESHOLD:
                return False
        return True
