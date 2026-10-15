"""
Bring-up CLI for one tentacle (one Pico).

Run from the pi/ folder with the venv active:

    python main.py                          # real hardware, auto-detect the Pico
    python main.py /dev/serial/by-id/usb-... # real hardware, explicit port
    PICO_PORT=/dev/serial/by-id/... python main.py
    MOCK=1 python main.py                   # software-only, no Pico needed
"""
import glob
import os
import sys

from tentacle_control.pico_interface import PicoInterface, USE_MOCK

HELP = """Commands:
  state                 read curvature / tension / flag / status
  curl <x>              start curling to target curvature x
  retract               return to baseline shape
  wait                  wait until the current move finishes
  stop                  stop all motors
  baseline              zero the sensors (tentacle must be still and uncurled)
  motor <i> <speed>     run motor i at speed -100..100 (auto-stops after 2 s)
  help                  show this list
  quit                  exit"""


def find_port():
    """Explicit argument > PICO_PORT env var > first /dev/serial/by-id entry."""
    if len(sys.argv) > 1:
        return sys.argv[1]
    if os.environ.get("PICO_PORT"):
        return os.environ["PICO_PORT"]
    found = sorted(glob.glob("/dev/serial/by-id/*"))
    if found:
        return found[0]
    return None


def run_command(pico, cmd):
    parts = cmd.split()
    name = parts[0].lower()

    if name == "state":
        print(pico.request_state())
    elif name == "curl" and len(parts) == 2:
        print(pico.curl(float(parts[1])))
    elif name == "retract":
        print(pico.retract())
    elif name == "wait":
        print(pico.wait_until_done())
    elif name == "stop":
        print(pico.stop())
    elif name == "baseline":
        print(pico.set_baseline())
    elif name == "motor" and len(parts) == 3:
        print(pico.motor(int(parts[1]), int(parts[2])))
    elif name == "help":
        print(HELP)
    else:
        print("Unknown command. Type 'help'.")


def cli():
    if USE_MOCK:
        port = "mock"
    else:
        port = find_port()
        if port is None:
            print("No Pico found in /dev/serial/by-id/. Check the cable and that "
                  "the firmware is flashed, or pass the port as an argument.")
            return 1

    print(f"Tentacle Controller Ready. ({'mock' if USE_MOCK else 'hardware'} mode, {port})")
    print(HELP)

    pico = PicoInterface(port)
    pico.start_heartbeat()  # Pico stops the motors if these stop arriving
    try:
        while True:
            try:
                cmd = input("> ").strip()
            except EOFError:
                break
            if not cmd:
                continue
            if cmd.lower() == "quit":
                break
            try:
                run_command(pico, cmd)
            except Exception as e:  # keep the CLI alive on bad input
                print("Error:", e)
    finally:
        try:
            pico.stop()
        finally:
            pico.close()
    return 0


if __name__ == "__main__":
    sys.exit(cli())
