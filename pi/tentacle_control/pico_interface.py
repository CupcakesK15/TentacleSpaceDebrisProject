from typing import Optional, Union
import serial
from .enums import TentacleFlag, TentacleError

import utils.mock_data as mock

USE_MOCK = True

StateData = tuple[float, float, TentacleFlag]
ResponseData = Union[None, str, StateData, TentacleError]
Response = tuple[Optional[str], ResponseData]

class PicoInterface:
    def __init__(self, port):
        self.port = port
        # currently waiting 100ms for timeout --> MIGHT CHANGE
        self.serial: Optional[serial.Serial] = None
        if not USE_MOCK:
            self.serial = serial.Serial(port, 115200, timeout=0.1)

    def send_command(self, command: str) -> None:
        """
        Send a command string to the Pico.
        ie. "CURL 50"
        """
        if USE_MOCK:
            mock.fake_send_command()
            return
        assert self.serial is not None, "send_command called while USE_MOCK is True"
        msg = (command + "\n").encode("ascii")
        self.serial.write(msg)
        self.serial.flush()  # making sure pico gets the command

    def _parse_line(self, line: str) -> Response:
        """
        Shared parser so mock and real responses are handled identically.
        """
        if not line:
            return None, None

        parts = line.split()

        # ACK <command>
        if parts[0] == "ACK":
            return "ACK", parts[1]

        # ERROR <code>
        if parts[0] == "ERROR":
            code = parts[1]
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

    def read_response(self) -> Response:
        assert self.serial is not None, "read_response called while USE_MOCK is True"
        line = self.serial.readline().decode("ascii").strip()
        return self._parse_line(line)

    def curl(self, speed) -> Response:
        if USE_MOCK:
            return self._parse_line(mock.fake_curl(speed))
        self.send_command(f"CURL {speed}")
        return self.read_response()

    def retract(self, speed) -> Response:
        if USE_MOCK:
            return self._parse_line(mock.fake_retract())
        self.send_command(f"RETRACT {speed}")
        return self.read_response()

    def set_baseline(self) -> Response:
        if USE_MOCK:
            return self._parse_line(mock.fake_set_baseline())
        self.send_command("SET_BASELINE")
        return self.read_response()

    def request_state(self) -> Response:
        if USE_MOCK:
            return self._parse_line(mock.fake_state())
        self.send_command("REQUEST_STATE")
        return self.read_response()

    def heartbeat(self) -> Response:
        if USE_MOCK:
            return self._parse_line(mock.fake_heartbeat())
        self.send_command("HEARTBEAT")
        return self.read_response()
