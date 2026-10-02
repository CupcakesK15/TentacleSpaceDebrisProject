import random
import time

def fake_state(): 
    curvature = random.uniform(0.0, 0.5)
    tension = random.uniform(0.0, 1.0)
    return f"STATE {curvature:.3f} {tension:.3f} OK"

def fake_curl(target):
    time.sleep(0.5)
    return "CURL_DONE"

def fake_stop():
    return "STOP"

def fake_retract():
    return "RETRACT"

def fake_heartbeat():
    return "HEARTBEAT"

def fake_set_baseline():
    return "SET BASELINE"
