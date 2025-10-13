#pragma once

#include <Arduino.h>
#include <limits.h>

// BUZZER

class Buzzer {
public:
    Buzzer(int buzzerPin);
    void buzz(int on_period, int off_period, int total_duration);
    void update(); // Call this from loop()
private:
    int pin;
    int onPeriod, offPeriod;
    unsigned long startTime, lastToggleTime;
    int state; // 0 = idle, 1 = on, 2 = off
    int totalDuration;
};

// FUNCTIONS

void printOnChange(const String& label, const String& currentValue);
void printOnChange(const String& label, int currentValue);
void printOnChange(const String& label);