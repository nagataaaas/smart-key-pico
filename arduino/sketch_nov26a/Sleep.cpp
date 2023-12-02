#include <stdio.h>

#include <chrono>

#include "hardware/clocks.h"
#include "pico/stdlib.h"

class Sleep {
   public:
    Sleep(uint32_t duration_ms) {
        _duration_ms = duration_ms;
        startAt = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
    };

    void wait() {
        uint32_t now = std::chrono::duration_cast<std::chrono::microseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
        uint32_t diff = now - startAt;
        if (diff < _duration_ms) {
            sleep_ms(_duration_ms - diff);
        }
    };

   private:
    uint32_t startAt;
    uint32_t _duration_ms;
};
