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
                  String& prev_phoneSensorState,
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
    now = millis();
    
    // If there is a rising edge response of bed time
    if (bedTimeState == "ON" && prev_bedTimeState == "OFF") {
        firstStartTime_state = true;
    } prev_bedTimeState = bedTimeState;
    // If there is a rising edge response of the phone state
    if (phoneSensorState == "ON" && prev_phoneSensorState == "OFF") {
        if (!firstStartTime_state) { // Phone was picked up during bed time
            unusedTime += now - midStartTime; // Add the time it was unused
        } 
    }
    // If there is a falling edge response of the phone state
    else if (phoneSensorState == "OFF" && prev_phoneSensorState == "ON") {
        midStartTime = now;
    }
    // If there is a constant "OFF" response of the phone state
    else if (phoneSensorState == "ON" && prev_phoneSensorState == "ON") {
        prev_midStartTime = midStartTime; // remove?
        midStartTime_state = false;
    } prev_phoneSensorState = phoneSensorState;

    // If it is bed time
    if (bedTimeState == "ON") { 
        // If I am laying in bed and I am on my phone, and it is the first time of my bed time
        if (bedState && phoneSensorState == "ON" && firstStartTime_state) {
            unusedTime = 0; // Is this necessary?
            firstStartTime = now;
            firstStartTime_state = false;
        } 
        // If bed time has started and we are tracking time
        if (!firstStartTime_state) {
            // Calculate total time on phone
            elapsedTime = now - firstStartTime;
            if (elapsedTime >= unusedTime) {
                useTime = elapsedTime - unusedTime;
            } else {
                // This case handles the millis() wraparound
                useTime = (ULONG_MAX - firstStartTime + now) - unusedTime;
            }
        // If i am on my phone, but it is not the first time of my bed time
        // Maybe have some debounce for when I am not in bed? Like, I am going to game all night but I triggered the state and still use the phone???
        if (!firstStartTime_state && phoneSensorState == "ON") {
            useTime = now - firstStarttime - unusedTime;
        } 
        else if (!firstStartTime_state && !midStartTime_state) { // happens in only one scan
            timeAdjust += unusedTime;
        }
        // If i am not on my phone (to get accurate usage time)
        else if (!firstStartTime_state && midStartTime_state && unusedTime < time_treshold) {
            unusedTime = now - midStartTime
        } 
        // None of the qualifiactions above has been met, i.e. unusedTime is bigger than time_threshold 
        else {
            useTime = 0;
            unusedTime = 0;
            timeAdjust = 0;
        }
    // It is not bed time anymore
    } else { // Remember to implement so that this else gets triggered after a certian amount of unused time (time_treshold)
        useTime = 0;
        unusedTime = 0;
        timeAdjust = 0;
    }
}




