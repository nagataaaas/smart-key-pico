#include <stdio.h>

#include <chrono>
#include <functional>
#include <map>

#include "hardware/clocks.h"
#include "pico/stdlib.h"

using nullFunc = void (*)();

struct Recipe {
    nullFunc callback;
    int duration_msec;
};

class Interval {
   public:
    Interval(){

    };
    void setInterval(int duration_msec, nullFunc callback, bool immediate = false) {
        int64_t now = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
        int64_t duration = duration_msec * 1'000;
        int64_t next = now + duration;
        callbacks[next] = Recipe{callback, duration_msec};
        if (immediate) {
            callback();
        }
    };

    void tick() {
        int64_t now = std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
        std::vector<Recipe> toAdd;
        std::vector<int64_t> toRemove;
        for (auto const& [key, val] : callbacks) {
            if (key < now) {
                val.callback();
                toRemove.push_back(key);
                toAdd.push_back(val);
            }
        }
        for (int64_t key : toRemove) {
            callbacks.erase(key);
        }
        now = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
        for (Recipe recipe : toAdd) {
            int64_t next = now + recipe.duration_msec * 1'000;
            callbacks[next] = recipe;
        }
    };

   private:
    std::unordered_map<int64_t, Recipe> callbacks;
};
