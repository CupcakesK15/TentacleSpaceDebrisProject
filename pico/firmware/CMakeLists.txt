cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(tentacle_firmware C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_executable(tentacle_firmware
    main.c
    comms.c
    motor_control.c
    sensor_read.c
    imu.c
)

target_link_libraries(tentacle_firmware
    pico_stdlib
    hardware_uart
    hardware_pwm
    hardware_i2c
)

pico_add_extra_outputs(tentacle_firmware)
