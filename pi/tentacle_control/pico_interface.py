import serial
from .enums import TentacleFlag, TentacleError

import utils.mock_data as mock

USE_MOCK = True 


class PicoInterface:
    def __init__(self, port):
        self.port = port
        # currently waiting 100ms for timeout --> MIGHT CHANGE
        self.serial = serial.Serial(port, 115200, timeout=0.1)

    def send_command(self, command):
        """
        Send a command string to the Pico.
        ie. "CURL 50"
        """
        msg = (command + "\n").encode("ascii")
        self.serial.write(msg)
        self.serial.flush()  # making sure pico gets the command

    def read_response(self):
        """
        Read one response line from the Pico.
        Returns a tuple (type, data)
        type: "ACK", "ERROR", "STATE", None
        data: parsed payload dependent on type
        """
        line = self.serial.readline().decode("ascii").strip()

        if not line:
            return None, None

        parts = line.split()

        # ACK <command>
        if parts[0] == "ACK":
            return "ACK", parts[1]

        # ERROR <code>
        if parts[0] == "ERROR":
            code = parts[1]
            # string to enum
            try:
                err_enum = TentacleError[code]
            except KeyError:
                err_enum = TentacleError.INVALID_CMD
            return "ERROR", err_enum

        # STATE <curvature> <tension> <flags>
        if parts[0] == "STATE":
            curvature = float(parts[1])
            tension = float(parts[2])
            flag_str = parts[3]
            try:
                flag_enum = TentacleFlag[flag_str]
            except KeyError:
                flag_enum = TentacleFlag.ERR
            return "STATE", (curvature, tension, flag_enum)

        # unknown message
        return None, line

    def curl(self, speed):
        if USE_MOCK: 
            return mock.fake_curl(speed)
        self.send_command(f"CURL {speed}")
        return self.read_response()

    def retract(self, speed):
        if USE_MOCK: 
            return mock.fake_retract()
        self.send_command(f"RETRACT {speed}")
        return self.read_response()

    def set_baseline(self):
        if USE_MOCK: 
            return mock.fake_set_baseline()
        self.send_command("SET_BASELINE")
        return self.read_response()

    def request_state(self):
        if USE_MOCK: 
            return mock.fake_state()
        self.send_command("REQUEST_STATE")
        return self.read_response()

    def heartbeat(self):
        if USE_MOCK: 
            return mock.fake_heartbeat()
        self.send_command("HEARTBEAT")
        return self.read_response()
