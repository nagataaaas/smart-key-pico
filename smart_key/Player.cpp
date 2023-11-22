#include "Player.hpp"

#include <stdio.h>

#include <vector>

#include "hardware/clocks.h"
#include "hardware/pwm.h"

#define SILENCE_FREQ 1'000'000
#define DEFAULT_CLK 125'000'000

uint32_t clock;

Player::Player(uint8_t pin) {
    _pin = pin;
    clock = clock_get_hz(clk_sys);
    slice = pwm_gpio_to_slice_num(pin);
    channel = pwm_gpio_to_channel(pin);

    gpio_init(pin);

    // Setup up PWM t
    gpio_set_function(pin, GPIO_FUNC_PWM);
    pwm_set_chan_level(slice, channel, 2048);
}
Player::~Player() {
}
void Player::off() {
    pwm_set_enabled(slice, false);
}
void Player::play(std::vector<Sound> sounds) {
    for (Sound sound : sounds) {
        pwm_set_enabled(slice, false);
        if (sound.frequency > 0) {
            setFrequency(sound.frequency);
            pwm_set_enabled(slice, true);
        }
        sleep_ms(sound.duration);
    }
    off();
}

void Player::setFrequency(uint16_t freq) {
    float divider = (float)clock / (freq * 10000.0);
    pwm_set_clkdiv(slice, divider);
    pwm_set_wrap(slice, 10000);
    pwm_set_gpio_level(_pin, 5000);
}

int duration(std::vector<Sound> sounds) {
    int duration = 0;
    for (Sound sound : sounds) {
        duration += sound.duration;
    }
    return duration;
}