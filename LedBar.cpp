//
// Created by Andri Yadi on 12/3/15.
//

#include "LedBar.h"
#include <Wire.h>

LedBar::~LedBar() {
}

LedBar::LedBar(uint8_t *_pins, uint8_t _pinCount) : pins_(_pins) {
    //barCount_ = sizeof(_pins) / sizeof(uint8_t) + 1;
    Serial.println("LEDBAR Control using GPIO");
    barCount_ = _pinCount;
    lastOn_ = millis();
}

LedBar::LedBar(uint8_t _address): i2cAddress_(_address) {
    //barCount_ = sizeof(_pins)/ sizeof(uint8_t) + 1;
    //Serial.printf("LEDBAR Control using PCF8574. Address = %d\r\n", _address);
	Serial.print("LEDBAR Control using PCF8574. Address = ");Serial.println(_address);
    barCount_ = 10;
    lastOn_ = millis();
    pins_ = 0;
}

void LedBar::begin() {

    //Serial.printf("Pins count = %d\r\n", barCount_);
    if (pins_) {
        for (uint8_t i = 0; i < barCount_; i++) {
            uint8_t pin = pins_[i];
            pinMode(pin, OUTPUT);
            digitalWrite(pin, HIGH);
        }
    }
    else {
        //Serial.println("Here....");
        Wire.beginTransmission(i2cAddress_);
        //Wire.write((uint8_t)0b00000000);          //All lighted on first
        Wire.write((uint8_t)0b11111111);            //All lighted off first
        lastError_ = Wire.endTransmission();
        //Serial.println(lastError_);
    }
}

void LedBar::setBarLevel(uint8_t barLevel) {
    if (pins_) {
        for (uint8_t i = 0; i < barCount_; i++) {
            uint8_t pin = pins_[i];

            if (i < barLevel) {
                digitalWrite(pin, LOW);
            }
            else {
                digitalWrite(pin, HIGH);
            }
        }
    }
    else if (barLevel == 0){
        Wire.beginTransmission(i2cAddress_);
        Wire.write(0b11111111);
        lastError_ = Wire.endTransmission();
    }
    else {
        Wire.beginTransmission(i2cAddress_);
        Wire.write(0b11111111 << (barLevel - 1));
        lastError_ = Wire.endTransmission();
    }
}

void LedBar::setPercentage(float_t percent) {
    if (allOff_) {
        return;
    }

    level_ = (percent / 100) * barCount_;
    setBarLevel(level_ + 1);
}

void LedBar::setBlink(boolean b) {
    blink_ = b;
    //Serial.printf("set blink %d\r\n", b);
}

void LedBar::loop() {

    if (blink_) {
        if ((millis() - lastOn_) > LED_BAR_BLINK_INTERVAL) {

            lastOn_ = millis();
            //Serial.printf("All off %d\r\n", allOff_);

            if (allOff_) {
                allOff();
            }
            else {
                if (level_ == 0) {
                    setPercentage(20);
                }
                else {
                    setPercentage((level_ / barCount_) * 100);
                }
            }

            allOff_ = !allOff_;
        }
    }
    else if (animating_) {

        if ((millis() - lastOn_) > LED_BAR_ANIMATING_INTERVAL) {

            int t = ((millis()- animStartTime_) % 3000) * barCount_/3000;
            //Serial.printf("t = %d\r\n", t + 1);
            setBarLevel(t + 1);

            lastOn_ = millis();
        }
    }
    else {
        allOff_ = false;
    }
}


void LedBar::allOff() {

    if (pins_) {
        for (uint8_t i = 0; i < barCount_; i++) {
            uint8_t pin = pins_[i];
            digitalWrite(pin, HIGH);
        }
    }
    else {
        Wire.beginTransmission(i2cAddress_);
        Wire.write(0b11111111);
        lastError_ = Wire.endTransmission();
    }
}

int LedBar::lastError() {
    int e = lastError_;
    lastError_ = 0;
    return e;
}

void LedBar::startAnimation() {
    animating_ = true;
    animStartTime_ = millis();
    level_ = 0;
}

void LedBar::stopAnimation() {
    animating_ = false;
    animStartTime_ = 0;
    level_ = 0;
}

bool LedBar::isAnimating() {
    return animating_;
}
