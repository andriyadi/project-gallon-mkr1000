//
// Created by Andri Yadi on 12/3/15.
//

#ifndef GALLON_LEDBAR_H
#define GALLON_LEDBAR_H

#include "Arduino.h"

#define LED_BAR_BLINK_INTERVAL 1000
#define LED_BAR_ANIMATING_INTERVAL 300

class LedBar {
public:
    LedBar(uint8_t address = 0x38); //atau 0x20
    LedBar(uint8_t* pins, uint8_t pinCount);
    ~LedBar();
    void begin();
    void setBlink(boolean b);
    void loop();
    void setPercentage(float_t percent);
    void setBarLevel(uint8_t barLevel);
    void startAnimation();
    void stopAnimation();
    bool isAnimating();
    int lastError();
    void allOff();

private:
    uint8_t* pins_;
    uint8_t level_ = 0;
    uint8_t barCount_ = 0;
    uint8_t lastError_ = 0;
    uint8_t i2cAddress_ = 0x38;   //atau 0x20

    volatile boolean blink_ = false, allOff_ = false;

    volatile boolean animating_ = false;

    volatile unsigned long lastOn_, animStartTime_;
};


#endif //GALLON_LEDBAR_H
