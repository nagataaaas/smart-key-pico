#include <stdio.h>

#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <SerialBT.h>

#include "hardware/clocks.h"
#include "pico/stdlib.h"
#define NOW_MILLI std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

using nullFunc = void (*)();

struct Recipe {
    nullFunc callback;
    int duration_msec;
    std::string tag;
    bool is_interval;
};

class Interval {
   public:
    Interval(){

    };
    void setInterval(int duration_msec, nullFunc callback, std::string tag = "", bool immediate = false) {
        int64_t next = NOW_MILLI + duration_msec;
        callbacks[next] = Recipe{callback, duration_msec, tag, true};
        if (immediate) {
            callback();
        }
    };
    void setTimeout(int duration_msec, nullFunc callback, std::string tag = "", bool immediate = false) {
        int64_t next = NOW_MILLI + duration_msec;
        callbacks[next] = Recipe{callback, duration_msec, tag, false};
        if (immediate) {
            callback();
        }
    };

    void tick() {
        int64_t now = NOW_MILLI;
        std::vector<Recipe> toAdd;
        std::vector<int64_t> toRemove;
        for (auto pair : callbacks) {
          const auto key = pair.first;
          const auto value = pair.second;
            if (key < now) {
                SerialBT.printf("[Interval.tick] tag='%s' is now running. current=%d, key=%d\n", value.tag.c_str(), now, key);
                value.callback();
                toRemove.push_back(key);
                if (value.is_interval) toAdd.push_back(value);
            }
        }
        for (int64_t key : toRemove) {
            callbacks.erase(key);
        }
        now = NOW_MILLI;
        for (Recipe recipe : toAdd) {
            int64_t next = now + recipe.duration_msec;
            callbacks[next] = recipe;
        }
    };

    void eraseByTag(std::string tag) {
        std::vector<int64_t> toRemove;
        for (auto pair : callbacks) {
          if (pair.second.tag == tag) toRemove.push_back(pair.first);
        }
        if (toRemove.size()){
                SerialBT.printf("[Interval.eraseByTag] new callback size = %d = (%d - %d)\n", callbacks.size() - toRemove.size(), callbacks.size(), toRemove.size());
        }
        for (int64_t key : toRemove) {
            callbacks.erase(key);
        }
    };

    bool hasTag(std::string tag) {
        for (auto pair : callbacks) {
          if (pair.second.tag == tag) return true;
        }
        return false;
    };

   private:
    std::unordered_map<int64_t, Recipe> callbacks;
};
