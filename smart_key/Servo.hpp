#ifndef SERVO_SRC_SERVO_H_
#define SERVO_SRC_SERVO_H_

#include "pico/stdlib.h"

class Servo {
   public:
    Servo(uint8_t pin);
    virtual ~Servo();

    void rotate(float degree);

   private:
    uint8_t _pin = 0;
};

#endif /* SERVO_SRC_SERVO_H_ */