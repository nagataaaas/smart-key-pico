#include <functional>
#include <vector>

#include "Interval.cpp"
#include "Player.hpp"
#include "Servo.hpp"
#include "Sleep.cpp"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#define PIN_GREEN 0
#define PIN_BLUE 1
#define PIN_RED 2
#define PIN_SERVO 28
#define PIN_BUTTON_OUT 26
#define PIN_BUTTON_IN 22
#define PIN_BUZZER 11

void init_io() {
    gpio_init(PIN_RED);
    gpio_set_dir(PIN_RED, GPIO_OUT);

    gpio_init(PIN_GREEN);
    gpio_set_dir(PIN_GREEN, GPIO_OUT);

    gpio_init(PIN_BLUE);
    gpio_set_dir(PIN_BLUE, GPIO_OUT);

    gpio_init(PIN_BUTTON_IN);
    gpio_set_dir(PIN_BUTTON_IN, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_IN);

    gpio_init(PIN_BUTTON_OUT);
    gpio_set_dir(PIN_BUTTON_OUT, GPIO_OUT);
    gpio_put(PIN_BUTTON_OUT, 1);

    gpio_init(PIN_SERVO);
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
}

struct Color {
    bool red;
    bool green;
    bool blue;

    void set() const {
        gpio_put(PIN_RED, red);
        gpio_put(PIN_GREEN, green);
        gpio_put(PIN_BLUE, blue);
    }
};

Servo servo = Servo(PIN_SERVO);
Player player = Player(PIN_BUZZER);

class LockState {
   public:
    static const uint DIG_CLOSE = 180;
    static const uint DIG_NEUTRAL = 90;
    static const uint DIG_OPEN = 0;

    double value;
    Color color;

    LockState(double val, const Color &c) : value(val), color(c) {}

    void operate() const {
        servo.rotate(value);
        sleep_ms(750);
    }
    void operateWithoutWait() const {
        servo.rotate(value);
    }

    void setColor() const { color.set(); }

    bool operator==(const LockState &other) const { return value == other.value; }

    static const LockState CLOSE;
    static const LockState NEUTRAL;
    static const LockState OPEN;
};

const Color RED_COLOR = {true, false, false};
const Color GREEN_COLOR = {false, true, false};
const Color BLUE_COLOR = {false, false, true};
const Color YELLOW_COLOR = {true, true, false};

const LockState LockState::CLOSE(LockState::DIG_CLOSE, BLUE_COLOR);
const LockState LockState::NEUTRAL(LockState::DIG_NEUTRAL, YELLOW_COLOR);
const LockState LockState::OPEN(LockState::DIG_OPEN, RED_COLOR);

const int c4 = 262;
const int cs4 = 277;
const int d4 = 294;
const int ds4 = 311;
const int e4 = 330;
const int f4 = 349;
const int fs4 = 370;
const int g4 = 392;
const int gs4 = 415;
const int a4 = 440;
const int as4 = 466;
const int b4 = 494;
const int c5 = 523;
const int cs5 = 554;
const int d5 = 587;
const int space = 0;

// c4, c4, g4, g4, a4, a4, g4, space,
//     f4, f4, e4, e4, d4, d4, c4, space,

void *play(void *arg) {
    std::vector<Sound> sounds = *(std::vector<Sound> *)arg;
    player.play(sounds);
    return NULL;
}
std::vector<Sound> open;
std::vector<Sound> close;

int main() {
    for (int freq : (std::vector<int>){d4, g4, f4, d5}) {
        open.push_back(Sound(freq, 150));
        open.push_back(Sound(space, 50));
    }
    open.end()[-2].duration = 250;
    for (int freq : (std::vector<int>){d5, c5, g4, b4}) {
        close.push_back(Sound(freq, 150));
        close.push_back(Sound(space, 50));
    }
    close.end()[-2].duration = 250;

    stdio_init_all();
    init_io();

    LockState::NEUTRAL.setColor();
    LockState currentState = LockState::CLOSE;
    currentState.operate();
    currentState.setColor();
    LockState::NEUTRAL.operate();

    bool before = gpio_get(PIN_BUTTON_IN);
    Interval interval = Interval();

    interval.setInterval(3000, []() { player.play(open); });

    while (true) {
        bool current = gpio_get(PIN_BUTTON_IN);
        if (!before && current) {
            currentState = currentState == LockState::CLOSE ? LockState::OPEN : LockState::CLOSE;
            LockState::NEUTRAL.setColor();
            currentState.operate();
            currentState.setColor();
            Sleep sleep = Sleep(0);
            LockState::NEUTRAL.operateWithoutWait();
            if (currentState == LockState::OPEN) {
                sleep = Sleep(duration(open));
                player.play(open);
            } else {
                sleep = Sleep(duration(close));
                player.play(close);
            }
            sleep.wait();
        }
        before = current;
        sleep_ms(100);
        interval.tick();
    }

    return 0;
}
