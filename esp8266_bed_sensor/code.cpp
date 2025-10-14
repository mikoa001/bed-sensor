#include "code.h"
#include <Arduino.h>

// BUZZER

// Defining constructer
Buzzer::Buzzer(int buzzerPin): pin(buzzerPin), state(0) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);  // Buzzer off
}

void Buzzer::buzz(int on_period, int off_period, int total_duration) {
  onPeriod = on_period;
  offPeriod = off_period;
  totalDuration = total_duration;
  startTime = millis();
  lastToggleTime = millis();
  state = 1;               // Start ON
  digitalWrite(pin, LOW);  // Buzzer ON
}

void Buzzer::update() {
  if (state == 0) return;  // idle
  unsigned long now = millis();
  if (now - startTime >= (unsigned long)totalDuration) {
    digitalWrite(pin, HIGH);  // Ensure buzzer off
    state = 0;
    return;
  }
  if (state == 1 && now - lastToggleTime >= (unsigned long)onPeriod) {
    digitalWrite(pin, HIGH);  // Buzzer OFF
    state = 2;
    lastToggleTime = now;
  } else if (state == 2 && now - lastToggleTime >= (unsigned long)offPeriod) {
    digitalWrite(pin, LOW);  // Buzzer ON
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

// PHONE WARNING

void phoneWarning(String phoneSensorState, String bedTimeState, String homeState, bool bedState, const int GREEN_LIGHT_PIN, const int RED_LIGHT_PIN, const unsigned long time_treshold) {
  // The current time
  now = millis();

  // If there is a rising edge response of bed time
  if (bedTimeState == "ON" && prev_bedTimeState == "OFF") {
    firstStartTime_state = true;
  }
  prev_bedTimeState = bedTimeState;

  // If it is bed time
  if (bedTimeState == "ON" && homeState == "HOME") {
    // If I am laying in bed and I am on my phone, and it is the first time of my bed time
    if (bedState && phoneSensorState == "ON" && firstStartTime_state) {
      // Initializing
      unusedTime = 0;
      useTime = 0;
      firstStartTime = now;
      firstStartTime_state = false;
    }
    // If the initialization have previously runned
    if (!firstStartTime_state) {
      // ===================================PHONE==============================================
      // If there is a rising edge response of the phone state
      if (phoneSensorState == "ON" && prev_phoneSensorState == "OFF") {
        if (midStartTime != 0) {
          unusedTime += now - midStartTime;  // Add the time it was unused
        }
      }
      // If there is a falling edge response of the phone state
      else if (phoneSensorState == "OFF" && prev_phoneSensorState == "ON") {
        midStartTime = now;
      }
      prev_phoneSensorState = phoneSensorState;
      // ===================================PHONE==============================================

      // If i am on my phone, but it is not the first time of my bed time
      // Maybe have some debounce for when I am not in bed? Like, I am going to game all night but I triggered the state and still use the phone???
      if (phoneSensorState == "ON") {
        useTime = elapsedTime - unusedTime;
      }
      // None of the qualifiactions above has been met, i.e. the phone have not been used for a long enough period
      if (phoneSensorState == "OFF" && (now - midStartTime >= time_treshold)) {
        firstStartTime_state = true;
        useTime = 0;
        unusedTime = 0;
      }
      // Calculate total time on phone
      elapsedTime = now - firstStartTime;
      useTime = elapsedTime - unusedTime;
    }
  }
  // It is not bed time anymore or I am not home
  else {
    firstStartTime_state = true;
    useTime = 0;
    unusedTime = 0;
    return;
  }
}