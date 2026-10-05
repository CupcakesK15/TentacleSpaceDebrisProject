import random
import time

def fake_state():
    curvature = random.uniform(0.0, 0.5)
    tension = random.uniform(0.0, 1.0)
    return f"STATE {curvature:.3f} {tension:.3f} OK"

def fake_curl(target):
    time.sleep(0.5)
    return "ACK CURL"

def fake_retract():
    return "ACK RETRACT"

def fake_set_baseline():
    return "ACK SET_BASELINE"

def fake_heartbeat():
    return "ACK HEARTBEAT"

def fake_send_command():
    return "ACK COMMAND"
