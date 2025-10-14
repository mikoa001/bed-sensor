#pragma once
#include <Arduino.h>
#include <limits.h>

String bedTimeState = "OFF";
String prev_bedTimeState =="OFF";
String prev_phoneSensorState = "OFF";
bool firstStartTime_state = false;
bool midStartTime_state = false;
unsigned long firstStartTime = 0;
unsigned long midStartTime = 0;
unsigned long prev_midStartTime = 0;
unsigned long useTime = 0;
unsigned long unusedTime = 0;
unsigned long now = 0;
unsigned long prev_now = 0;
unsigned long interval = 100;
unsigned long elapsedTime = 0;


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

void phoneWarning(String phoneSensorState, int LIGHT_PIN);


