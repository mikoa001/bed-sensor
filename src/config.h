#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <limits.h>
#include "secrets.h"

// Flag to track if discovery has been published
bool discovery_published = false;

// MQTT Topics
const char* state_topic = "homeassistant/binary_sensor/bed_sensor/state";
const char* config_topic = "homeassistant/binary_sensor/bed_sensor/config";
const char* availability_topic = "homeassistant/binary_sensor/bed_sensor/availability";
const char* rssi_state_topic = "homeassistant/sensor/bed_sensor_rssi/state";
const char* ip_state_topic = "homeassistant/sensor/bed_sensor_ip/state";
const char* pressure_state_topic = "homeassistant/sensor/bed_sensor_pressure/state";
const char* phone_usage_state_topic = "homeassistant/sensor/bed_sensor_phone_usage/state";

// Topic for monitoring HA entity state
const char* phone_sensor_state_topic = "homeassistant/binary_sensor/mikael_sin_mobil_interactive/state";
const char* bed_time_state_topic = "homeassistant/binary_sensor/bedTimeState";
const char* home_state_topic = "homeassistant/device_tracker/mikael_sin_mobil";
const char* use_time_treshold_topic = "homeassistant/input_number/bed_sensor_phone_use_time_treshold";
const char* unused_time_treshold_topic = "homeassistant/input_number/bed_sensor_phone_unused_time_treshold";

// Initialize the state of the monitored entities
String phoneSensorState = "OFF";
String bedTimeState = "OFF";
String homeState = "NOT_HOME";
int useTime_treshold = 0;
int unusedTime_treshold = 0;


// Defining pins
const int SENSOR_PIN = A0;
const int BUTTON_PIN = 15;
const int GREEN_LIGHT_PIN = 14;
const int RED_LIGHT_PIN = 12;
const int BUZZER_PIN = 5; // I had 16 before, but changed to 5
const int LED_BUILTIN_1 = 2; // Define top-built-in LED pin for ESP8266
const int LED_BUILTIN_2 = 16; // Define bottom-built-in LED pin for ESP8266

// Sensor threshold - adjust based on your sensor
const int OCCUPIED_THRESHOLD = 250;  // Value when bed is occupied (0-1023): 0 means alot of pressure, 1023 means absolutely no pressure at all.

// Debounce settings
const unsigned long DEBOUNCE_DELAY = 0;  // The time of how long the criteria must be held before the state of the sensor changes
unsigned long lastDebounceTime = 0;
bool lastBedState = false;
bool currentBedState = false;

WiFiClient espClient;
PubSubClient client(espClient);

// Defining phoneWarning related variables
String prev_bedTimeState = "OFF";
String prev_phoneSensorState = "OFF";
bool firstStartTime_state = true;  // Start in a ready state
unsigned long now = 0;
unsigned long firstStartTime = 0;
unsigned long midStartTime = 0;
unsigned long useTime = 0;
unsigned long unusedTime = 0;
unsigned long elapsedTime = 0;

// Timer for publishing sensor data
unsigned long lastDataPublishTime = 0;
const unsigned long DATA_PUBLISH_INTERVAL = 30000; // Publish every 30 seconds

// Defining debugging related variables
bool prev_bedOccupied = false;
String prev_homeState = "NOT_HOME";

// Buzzer sequence configuration (non-blocking using millis)
const int BUZZS_BEEPS_PER_BURST = 3;     // beeps in one burst
const unsigned long BUZZS_BEEP_ON_MS = 80;   // beep on duration
const unsigned long BUZZS_BEEP_OFF_MS = 180;  // gap between beeps
const unsigned long BUZZS_BURST_PAUSE_MS = 1500; // gap between bursts

// Buzzer runtime state
bool buzzerRequested = false; // whether logic currently requests the buzzer
bool buzzerActive = false;    // whether a sequence is running
int buzzerBeepIndex = 0;      // which beep within the current burst
int buzzerPhase = 0;          // 0 = ON, 1 = BETWEEN-BEEPS (OFF), 2 = BURST-PAUSE
unsigned long buzzerLastToggle = 0;

