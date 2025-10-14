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

void phoneWarning(String phoneSensorState, 
                  String bedTimeState, 
                  String& prev_bedTimeState, 
                  const int GREEN_LIGHT_PIN, 
                  const int RED_LIGHT_PIN, 
                  const unsigned long time_treshold, 
                  bool BedState,
                  bool prev_BedState
                 ) {
    // The current time
    unsigned long now = millis(); // you have to resolve the problem with the variable now being resetted (wraparound) to zero every 49.7 days.
    
    // If there is a rising edge response of bed time
    if (bedTime == "ON" && prev_bedTime == "OFF") {
        firstStartTime_state = true;
    // If there is a constat response of bed time
    } else if (bedTime == prev_bedTime) {
        firstStartTime_state = false;
    // If there is a falling edge response of bed time
    } else if (bedTime == "OFF" && prev_bedTime == "ON")
        firstStartTime_state = true;
    }
    prev_bedTimeState = bedTimeState;

    // If there is a rising edge response of the bed state
    if (bedState == true && prev_bedState == false) {
        midStartTime_state = false;
    // If there is a falling edge response of the bed state
    } else if (bedState == false && prev_bedState == true) {
        midStartTime_state = true;
    }
    prev_bedState = bedState;

    // If it is bed time
    if (bedTimeState == "ON") {
        // If I am laying in bed and I am on my phone, and it is the first time of the evening
        if (bedState && phoneSensorState == "ON" && firstStartTime_state) {
            unsigned long firstStartTime = now;
            firstStartTime_state = false;
        // If I am laying in bed and I am on my phone, or I am not on my phonenot laying in my bed and I am using 
        } else if (bedState || not bedState && not firstStartTime_state) {
            if (phoneSensorState == "ON") {
                // content
                unusedTime;
            } else if (phoneSensorState == "OFF" && unusedTime < time_treshold) {
                // content
                unuseTime;
            }
        } else if (
    // It is not bed time anymore2
    } else {
        useTime = 0;
        unusedTime = 0;
    }
}

