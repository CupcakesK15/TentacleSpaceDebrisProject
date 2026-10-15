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
    sensor_fusion.c
    imu.c
    tentacle_control.c
)

target_link_libraries(tentacle_firmware
    pico_stdlib
    hardware_pwm
    hardware_i2c
)

# printf/getchar go over the USB cable (shows up as /dev/ttyACM* on the Pi)
pico_enable_stdio_usb(tentacle_firmware 1)
pico_enable_stdio_uart(tentacle_firmware 0)
pico_add_extra_outputs(tentacle_firmware)
