#include <functional>
#include <iostream>
#include <vector>

#include "Interval.cpp"
#include "Player.hpp"
#include "Servo.hpp"
#include "Sleep.cpp"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"

#define PIN_GREEN 0
#define PIN_BLUE 1
#define PIN_RED 2
#define PIN_SERVO 28
#define PIN_BUTTON_OUT 26
#define PIN_BUTTON_IN 22
#define PIN_BUZZER 11

auto_init_mutex(task_mutex);

void tryLockTask() {
    uint32_t owner_out;
    while (true) {
        if (mutex_try_enter(&task_mutex, &owner_out)) break;
        sleep_ms(10);
    }
}
void releaseTask() {
    mutex_exit(&task_mutex);
}

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

std::vector<Sound> open;
std::vector<Sound> close;

enum class Task {
    PLAY_OPEN,
    PLAY_CLOSE,
};
std::vector<Task> tasks;

void waitTask() {
    while (true) {
        tryLockTask();
        if (tasks.size() > 0) {
            Task task = tasks[0];
            tasks.erase(tasks.begin());

            releaseTask();
            if (task == Task::PLAY_OPEN) {
                player.play(open);
            } else if (task == Task::PLAY_CLOSE) {
                player.play(close);
            }
        } else {
            releaseTask();
        }
        sleep_ms(10);
    }
}

void pushTask(Task task) {
    tryLockTask();
    tasks.push_back(task);
    releaseTask();
}

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
    multicore_launch_core1(waitTask);

    LockState::NEUTRAL.setColor();
    LockState currentState = LockState::CLOSE;
    currentState.operate();
    currentState.setColor();
    LockState::NEUTRAL.operate();

    bool before = gpio_get(PIN_BUTTON_IN);
    Interval interval = Interval();

    // interval.setInterval(3000, []() { player.play(open); });

    while (true) {
        bool current = gpio_get(PIN_BUTTON_IN);
        if (!before && current) {
            currentState = currentState == LockState::CLOSE ? LockState::OPEN : LockState::CLOSE;
            std::cout << 2;
            if (currentState == LockState::OPEN) {
                pushTask(Task::PLAY_OPEN);
            } else {
                pushTask(Task::PLAY_CLOSE);
            }
            LockState::NEUTRAL.setColor();
            currentState.operate();
            currentState.setColor();
            LockState::NEUTRAL.operateWithoutWait();
        }
        before = current;
        std::cout << "sleeping!\n";
        sleep_ms(100);
        // interval.tick();
        // std::cout << get_totp_token("dff72519fcb676918638104ef4e92771b4847fa97c341494d411c59704bbd825a1d6abb23433a4d9b66e49f0dc6b8c85245705efa9a68afa6bda461c333728aad8f665daa277c4d6826a1fb6b45e30ee16929de421ae6c42470521e9676979c7e468e726eb1487431e9aba6bc8936e832d88cf405134d17eb5cdb84926eba1bc8a96ec73b413d67dd0f9617b1a3ff8886ab2d1e6001bb5e7b4572ad15f22bbf0efd6c003e81ec3c9e66ac3121c1fb43499be1d7d78f42834f1390a0c8ac06c9ca24daff69259ed68e88573cafb1a65a143af701d067d72367bffa54bd75cd66f7f1adbf6774b0c8b289196aea1a4e9c4a77b48f3854423a4a7e785efc391feac452eccb0dcc64e2cd12947860d21e97cb4539cd29afa92ee16287ca49ab7f8054b79ba94caf55f16345ad6c946c5950469515d275e18ec224094308c43a323d705d0aa9dc316a37edd01ae68d22d45a7995fe096c2cf38494040abd149ed0402f6f711458646864e66bc640e0e0d10a66c3dbcfb2d50114ffe971d458fff4ef25c469a5c636fda524390d170bb6f26cdde144228cd705f9a4e5f8f58c8fd32139fadeea5e68271aceaf5c00407da85fa4b4ce94c70223df17ce567aac5c267c186effba730610ffd9d0cbadf05c5ec187dfba80d88f50c62ed9e2755979893733ab9f5445ae2e63ad7b793d93e129ff216b109938b0fd21b06a870057ecbfe5fb6dc633ca1518040e99566426166a7aac26a899543754b39026166d8f5e9b3e5f7febbe9c76d4318a36088cb5a42a483f8f0df8ed463033f3f7df26517a059e8383868ea4bc0a796974e7919633c16b8a80f9e952bc020b0166bb6204cbfaf6b6f12dbb9d53a1d8c0087c68f5a6eaa46241241d93b6aac28e7f755fa1844910d4c11bc3a6d04e9f603db848ec9e6cee2b8562e38a9f793cfee6f80d0d996407436072576c4013eac4e47fa2613d4128e7fc6e7a8208910bd8247033a94bb0f8e5b1d934e3e611efbbd5a01463da0636fafd5fa10a22564e5d5821e6148f6bb7e123950a39c5c15ab67eea1392a701222fcc9cbbe9a699d14d3f7da4255776b35c4c7aa8310afb87280bc1d8a4b3a3d44d14978e926079f725bc354c55f4e11f7da126e1b01fd91bb637fa0484576eb4528137f1d692e9400e14a70e6c689150da1c076954723e3cdecce703257c4835251306aa64f0ce04deb8cefd5fa07e1e3d28f632b54de8e6ff4f9de29356f05140d8ac51b6c7887f34d1a49d815a951ccf1783c31253c9d50521a345dbe503624967fad7efa1675c32c847bbf5cf4bc590334aba32bd693e4dc43fef8c9f4966a8a136f4d79d2cd9b7b48d680f67132c136c5379f3340395b430b712827120891e928b619ccdf816ead45dc37129ae915932b39d9a93ebc690e1ef63678ce42e462a632003b9aab7d560588fdf71952ac") << "\n";
        std::cout << "totp!\n";
    }

    return 0;
}
