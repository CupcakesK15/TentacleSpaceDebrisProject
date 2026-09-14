Transport of Data: 
    RasPi5 <--> RasPiPico communicate over UART
    Baud rate: 115200
    Messages sent in ASCII text, ending in newline character '\n'
    Each message processed as single command

Commands RasPi5 Sends to RasPiPico: 
    CURL <int: speed>: curl inwards at <speed>
    RETRACT <int: speed>: uncurl out at <speed>
    SET_BASELINE: records current position as "relaxed" pose (used for correcting positional drift)
    REQUEST_STATE: returns current sensor state
    HEARTBEAT: checks if RasPiPico is alive via a ping

Responses RasPiPico Sends to RasPi5 to Commands: 
    STATE <float: curvature> <float: tension> <enum: flags>
        Valid Flags: 
            OK
            WARN
            ERR
    ACK <string: command>
    ERROR <enum: code>
        Valid Error Codes: 
            MOTOR_FAULT
            SENSOR_FAULT
            INVALID_CMD
            TIMEOUT

Message Format: 
    Commands: space-separated tokens
    Numbers: ints or floats
    One command per line and RasPiPico responds immediately

Tentacle IDs: 
    Messages don't include tentacle IDs since each RasPiPico should connect to its own serial port on the RasPi5

Timing: --> NEEDS TO BE EDITED FOR THE ACTUAL TIMES
    RasPiPico should respond within xxms of receiving a command
    RasPi considers no response for more than xxms as a TIMEOUT error.