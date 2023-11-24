
project(smart_key)

add_executable(smart_key
    main.cpp
    Servo.cpp
    Player.cpp
    Sleep.cpp
    Interval.cpp
    OTP.cpp
    crypto.cpp
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(smart_key pico_stdlib hardware_irq hardware_pwm pico_multicore)
pico_enable_stdio_usb(smart_key 1)
pico_enable_stdio_uart(smart_key 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(smart_key)