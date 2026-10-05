import serial
import time

#for testing without hardware
from tentacle_control.pico_interface import PicoInterface
TEST_SOFT = True

#Adjust this to te Pico's serial port
PORT = "/dev/ttyACM0"
BAUD = 115200

def open_serial():
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2) #allows the Pico to reset
    return ser

def send_cmd(ser, cmd):
    ser.write((cmd + "\n").encode())
    ser.flush()

def read_response(ser):
    line = ser.readline().decode().strip()
    return line

def get_state(ser):
    send_cmd(ser, "STATE")
    return read_response(ser)

def curl(ser, target):
    send_cmd(ser, f"CURL {target}")
    return read_response(ser)

def stop(ser):
    send_cmd(ser, "STOP")
    read_response(ser)

def manual_motor(ser, motor, direction, speed):
    send_cmd(ser, f"{motor}_{direction} {speed}")
    return read_response(ser)


def cli_soft(pico: PicoInterface):
    print("Tentacle Controller Ready. (mock mode)")
    print("Commands: state, curl <x>, stop, quit")

    while True:
        try:
            cmd = input(">").strip()

            if cmd == "quit":
                break
            elif cmd == "state":
                print(pico.request_state())
            elif cmd.startswith("curl "):
                target = float(cmd.split()[1])
                print(pico.curl(target))
            elif cmd == "stop":
                print(pico.retract(0))
            else:
                print("Unknown command.")

        except Exception as e:
            print("Error:", e)


def cli_hardware(ser):
    print("Tentacle Controller Ready. (hardware mode)")
    print("Commands: state, curl <x>, stop, a_fwd <x>, a_rev <x>, b_fwd <x>, b_rev <x>, quit")

    while True:
        try:
            cmd = input(">").strip()

            if cmd == "quit":
                break
            elif cmd == "state":
                print(get_state(ser))
            elif cmd.startswith("curl "):
                target = float(cmd.split()[1])
                print(curl(ser, target))
            elif cmd == "stop":
                print(stop(ser))
            elif cmd.startswith("a_fwd"):
                speed = int(cmd.split()[1])
                print(manual_motor(ser, "A", "FORWARD", speed))
            elif cmd.startswith("a_rev"):
                speed = int(cmd.split()[1])
                print(manual_motor(ser, "A", "REVERSE", speed))
            elif cmd.startswith("b_fwd"):
                speed = int(cmd.split()[1])
                print(manual_motor(ser, "B", "FORWARD", speed))
            elif cmd.startswith("b_rev"):
                speed = int(cmd.split()[1])
                print(manual_motor(ser, "B", "REVERSE", speed))
            else:
                print("Unknown command.")

        except Exception as e:
            print("Error:", e)

    ser.close()


def cli():
    if TEST_SOFT:
        cli_soft(PicoInterface(PORT))
    else:
        cli_hardware(open_serial())


if __name__ == "__main__":
    cli()
