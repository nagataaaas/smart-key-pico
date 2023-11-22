
project(smart_key)

add_executable(smart_key
    main.cpp
    Servo.cpp
    Player.cpp
    Sleep.cpp
    Interval.cpp
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(smart_key pico_stdlib hardware_irq hardware_pwm)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(smart_key)