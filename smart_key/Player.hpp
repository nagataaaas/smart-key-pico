#ifndef PLAYER_SRC_PLAYER_H_
#define PLAYER_SRC_PLAYER_H_

#include <vector>

#include "pico/stdlib.h"

struct Sound {
    uint16_t frequency;
    uint16_t duration;

    Sound(uint16_t f, uint16_t d) : frequency(f), duration(d) {}
};

class Player {
   public:
    Player(uint8_t pin);
    virtual ~Player();

    void play(std::vector<Sound> sounds);
    void off();

   private:
    uint8_t _pin = 0;
    uint slice = 0;
    uint channel = 0;
    void setFrequency(uint16_t freq);
};

int duration(std::vector<Sound> sounds);

#endif /* PLAYER_SRC_PLAYER_H_ */