import os
import threading
import time

from typing import Optional

import serial

from .enums import TentacleError, TentacleFlag, TentacleStatus

# Real hardware by default. Run `MOCK=1 python main.py` for software-only testing.
USE_MOCK = os.environ.get("MOCK") == "1"

BAUD = 115200            # ignored by USB serial, but pyserial requires a value
READ_TIMEOUT = 0.5       # seconds; SET_BASELINE takes ~0.2 s to reply
MAX_JUNK_LINES = 5       # non-protocol lines to skip before giving up on a reply
VALID_REPLIES = ("ACK", "ERROR", "STATE")


class PicoInterface:
    def __init__(self, port):
        self.port = port
        self.serial: Optional[serial.Serial] = None
        self._lock = threading.Lock()
        self._hb_stop = threading.Event()
        self._hb_thread = None

        # mock-mode state
        self._mock_line = ""
        self._mock_target = 0.0
        self._mock_curv = 0.0
        self._mock_done_at = 0.0
        self._mock_status = "IDLE"

        if not USE_MOCK:
            try:
                ser = serial.Serial(port, BAUD, timeout=READ_TIMEOUT,
                                    write_timeout=1.0)
            except serial.SerialException as e:
                raise RuntimeError(
                    f"Could not open {port}: {e}\n"
                    "Check the path with `ls /dev/serial/by-id/`, that the Pico is "
                    "flashed, and that your user is in the 'dialout' group."
                ) from e
            self.serial = ser
            time.sleep(0.1)
            ser.reset_input_buffer()

    # ------------------------------------------------------------------
    # Low level
    # ------------------------------------------------------------------
    def _require_serial(self) -> serial.Serial:
        if self.serial is None:
            raise RuntimeError("Serial port is not open (mock mode or already closed).")
        return self.serial

    def _readline(self):
        if USE_MOCK:
            line, self._mock_line = self._mock_line, ""
            return line
        raw = self._require_serial().readline()
        return raw.decode("ascii", errors="ignore").strip()

    def _transact(self, command):
        """Send one command and return the parsed reply. Thread-safe."""
        with self._lock:
            if USE_MOCK:
                self._mock_line = self._mock_reply(command)
                return self.read_response()

            try:
                ser = self._require_serial()
                ser.reset_input_buffer()  # drop stale bytes
                ser.write((command + "\n").encode("ascii"))
                ser.flush()
            except serial.SerialException:
                return "ERROR", TentacleError.TIMEOUT
            return self.read_response()

    def read_response(self):
        """
        Read and parse one reply line.
        Returns (type, data):
          ("ACK", command_name)
          ("ERROR", TentacleError)
          ("STATE", (curvature, tension, TentacleFlag, TentacleStatus))
          (None, None) if nothing valid arrived before the timeout
        Lines that don't start with ACK/ERROR/STATE (e.g. stray debug prints
        from the firmware) are skipped.
        """
        line = ""
        for _ in range(MAX_JUNK_LINES):
            try:
                line = self._readline()
            except serial.SerialException:
                return None, None
            if not line:
                return None, None  # timeout, no data
            if line.split()[0] in VALID_REPLIES:
                break
        else:
            return None, None

        parts = line.split()

        if parts[0] == "ACK":
            return "ACK", parts[1] if len(parts) > 1 else ""

        if parts[0] == "ERROR":
            code = parts[1] if len(parts) > 1 else "INVALID_CMD"
            err = TentacleError.__members__.get(code, TentacleError.INVALID_CMD)
            return "ERROR", err

        # STATE <curvature> <tension> <flag> [status]
        if parts[0] == "STATE":
            try:
                curvature = float(parts[1])
                tension = float(parts[2])
            except (IndexError, ValueError):
                return None, line
            flag = TentacleFlag.__members__.get(
                parts[3] if len(parts) > 3 else "", TentacleFlag.ERR)
            status = TentacleStatus.__members__.get(
                parts[4] if len(parts) > 4 else "", TentacleStatus.IDLE)
            return "STATE", (curvature, tension, flag, status)

        return None, line

    # ------------------------------------------------------------------
    # Commands
    # ------------------------------------------------------------------
    def curl(self, target):
        """Start curling to a target curvature. Returns immediately (ACK)."""
        return self._transact(f"CURL {target}")

    def retract(self):
        """Return to the baseline shape (curvature 0)."""
        return self._transact("RETRACT")

    def stop(self):
        return self._transact("STOP")

    def set_baseline(self):
        """Tentacle must be still and uncurled when this is called."""
        return self._transact("SET_BASELINE")

    def request_state(self):
        return self._transact("REQUEST_STATE")

    def heartbeat(self):
        return self._transact("HEARTBEAT")

    def motor(self, index, speed):
        """Bring-up command: run one motor (speed -100..100); auto-stops after 2 s."""
        return self._transact(f"MOTOR {index} {int(speed)}")

    # ------------------------------------------------------------------
    # Helpers
    # ------------------------------------------------------------------
    def wait_until_done(self, timeout=6.0, poll=0.1):
        """Poll REQUEST_STATE until the move finishes. Returns a TentacleStatus."""
        end = time.monotonic() + timeout
        while time.monotonic() < end:
            kind, data = self.request_state()
            if kind == "STATE" and isinstance(data, tuple):
                status = data[3]
                if status != TentacleStatus.MOVING:
                    return status
            time.sleep(poll)
        return TentacleStatus.TIMEOUT

    def start_heartbeat(self, period=0.25):
        """Ping the Pico regularly. If pings stop, the Pico stops the motors."""
        if self._hb_thread and self._hb_thread.is_alive():
            return
        self._hb_stop.clear()

        def loop():
            while not self._hb_stop.is_set():
                self.heartbeat()
                self._hb_stop.wait(period)

        self._hb_thread = threading.Thread(target=loop, daemon=True)
        self._hb_thread.start()

    def close(self):
        self._hb_stop.set()
        if self._hb_thread:
            self._hb_thread.join(timeout=1.0)
        ser = self.serial
        if ser is not None and ser.is_open:
            ser.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    # ------------------------------------------------------------------
    # Mock mode (software testing only; mirrors the real protocol)
    # ------------------------------------------------------------------
    def _mock_reply(self, command):
        parts = command.split()
        name = parts[0] if parts else ""
        now = time.monotonic()

        if self._mock_status == "MOVING" and now >= self._mock_done_at:
            self._mock_status = "DONE"
            self._mock_curv = self._mock_target

        if name == "CURL" and len(parts) > 1:
            self._mock_target = float(parts[1])
            self._mock_done_at = now + 0.5
            self._mock_status = "MOVING"
            return "ACK CURL"
        if name == "RETRACT":
            self._mock_target = 0.0
            self._mock_done_at = now + 0.5
            self._mock_status = "MOVING"
            return "ACK RETRACT"
        if name == "STOP":
            self._mock_status = "IDLE"
            return "ACK STOP"
        if name == "SET_BASELINE":
            self._mock_curv = 0.0
            return "ACK SET_BASELINE"
        if name == "HEARTBEAT":
            return "ACK HEARTBEAT"
        if name == "MOTOR":
            return "ACK MOTOR"
        if name == "REQUEST_STATE":
            return f"STATE {self._mock_curv:.3f} 0.100 OK {self._mock_status}"
        return "ERROR INVALID_CMD"
