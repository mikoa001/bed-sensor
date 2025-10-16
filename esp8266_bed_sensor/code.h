#pragma once
#include <Arduino.h>
#include <limits.h>

extern String prev_bedTimeState;
extern String prev_phoneSensorState;
extern bool firstStartTime_state;
extern unsigned long now;
extern unsigned long firstStartTime;
extern unsigned long midStartTime;
extern unsigned long useTime;
extern unsigned long unusedTime;
extern unsigned long elapsedTime;

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

bool phoneWarning(String phoneSensorState, String bedTimeState, String homeState, bool bedState, const unsigned long unused_time_treshold, const unsigned long use_time_treshold);

