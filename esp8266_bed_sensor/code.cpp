#include "code.h"
#include <Arduino.h>

// BUZZER

Buzzer::Buzzer(int buzzerPin) : pin(buzzerPin), state(0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH); // Buzzer off
}

void Buzzer::buzz(int on_period, int off_period, int total_duration) {
    onPeriod = on_period;
    offPeriod = off_period;
    totalDuration = total_duration;
    startTime = millis();
    lastToggleTime = millis();
    state = 1; // Start ON
    digitalWrite(pin, LOW); // Buzzer ON
}

void Buzzer::update() {
    if (state == 0) return; // idle
    unsigned long now = millis();
    if (now - startTime >= (unsigned long)totalDuration) {
        digitalWrite(pin, HIGH); // Ensure buzzer off
        state = 0;
        return;
    }
    if (state == 1 && now - lastToggleTime >= (unsigned long)onPeriod) {
        digitalWrite(pin, HIGH); // Buzzer OFF
        state = 2;
        lastToggleTime = now;
    } else if (state == 2 && now - lastToggleTime >= (unsigned long)offPeriod) {
        digitalWrite(pin, LOW); // Buzzer ON
        state = 1;
        lastToggleTime = now;
    }
}

// FUNCTIONS

// For String or const char* values
void printOnChange(const String& label, const String& currentValue) {
  static String lastValue = "";
  if (currentValue != lastValue) {
    Serial.print(label);
    Serial.println(currentValue);
    lastValue = currentValue;
  }
}

// For integer values
void printOnChange(const String& label, int currentValue) {
  static int lastValue = INT_MIN;
  if (currentValue != lastValue) {
    Serial.print(label);
    Serial.println(currentValue);
    lastValue = currentValue;
  }
}

// For printing only labels
void printOnChange(const String& label) {
  static String lastLabel = "";
  if (label != lastLabel) {
    Serial.println(label);
    lastLabel = label;
  }
}