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

// PHONE WARNING

void phoneWarning(String phoneSensorState, // Remember to fix so that our phoneSensorState only goes high if I am home (geolocation)
                  String bedTimeState, 
                  String& prev_bedTimeState,
                  bool BedState,
                  bool prev_BedState
                  const int GREEN_LIGHT_PIN, 
                  const int RED_LIGHT_PIN, 
                  const unsigned long time_treshold, 
                 ) {
    // The current time
    // you have to resolve the problem with the variable now being resetted (wraparound) to zero every 49.7 days.
    // Might resolve it by using timesteps and iterate the other time-varaiables with this timestep, may need to think more about it.
    // In that case, you have to create that functionality in the main code and only run the code every time step, and for each time it is being ran, the time varaiables will increase.
    unsigned long now = millis();
    
    // If there is a rising edge response of bed time
    if (bedTime == "ON" && prev_bedTime == "OFF") {
        firstStartTime_state = true;
    // If there is a falling edge response of bed time
    } /* else if (bedTime == "OFF" && prev_bedTime == "ON") // this may not be necessary
        firstStartTime_state = false;
    } */
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
        // If I am laying in bed and I am on my phone, and it is the first time of my bed time
        if (bedState && phoneSensorState == "ON" && firstStartTime_state) {
            unsigned long firstStartTime = now;
            firstStartTime_state = false;
        } 
        // If i am on my phone, but it is not the first time of my bed time
        // Maybe have some debounce for when I am not in bed? Like, I am going to game all night but I triggered the state and still use the phone???
        else if (!firstStartTime_state && phoneSensorState == "ON") {
            // Write your code here
        } 
        // If i am not on my phone (to get accurate usage time)
        else if (!firstStartTime_state && phoneSensorState == "OFF" && unusedTime < time_treshold) {
        if (!firstStartTime_state && phoneSensorState == "ON") {
            if (phoneSensorState == "ON") {
                // content
                unusedTime;
            } else if (phoneSensorState == "OFF" && unusedTime < time_treshold) {
                // content
                unuseTime;
            }
        } else if (
    // It is not bed time anymore
    } else {
        useTime = 0;
        unusedTime = 0;
    }
}


