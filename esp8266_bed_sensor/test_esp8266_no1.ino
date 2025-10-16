/*
 *  HA-SleepGuard Bed Sensor
 *  ESP8266 Bed Occupancy Sensor for Home Assistant
 * 
 *  This sketch reads a pressure/weight sensor to detect bed occupancy
 *  and publishes the state to Home Assistant via MQTT
 * 
 *  Extended: Now also subscribes to the state of binary_sensor.mikael_sin_mobil_interactive
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "code.h"

// WiFi credentials
const char* ssid = "Telia-1D84C8";
const char* password = "aPKkYr3d3PPGHWmJ";

// MQTT Broker settings
const char* mqtt_server = "192.168.1.118";             // IP of your Home Assistant device, found in the network settings in the HA UI
const int mqtt_port = 1883;                            // Default MQTT port, change it if you have manually changed the port in the MQTT add-on (Mosquitto)
const char* mqtt_user = "mqtt";                        // The HA user (not person, there is a difference) where the mqtt comms will happen on, for limiting the access in case of an cyberattack
const char* mqtt_password = "Hjju7ytfv4%%43ibpplpwf";  // A strong password for the mqtt user so that it is hard to hack
const char* mqtt_client_id = "bed_sensor";             // the id_of this single client (the microcontroller)

// MQTT Topics
const char* state_topic = "homeassistant/binary_sensor/bed_sensor/state";
const char* config_topic = "homeassistant/binary_sensor/bed_sensor/config";
const char* availability_topic = "homeassistant/binary_sensor/bed_sensor/availability";

// Topic for monitoring HA entity state
const char* phone_sensor_state_topic = "homeassistant/binary_sensor/mikael_sin_mobil_interactive/state";
const char* bed_time_state_topic = "homeassistant/binary_sensor/bedTimeState";
const char* home_state_topic = "homeassistant/device_tracker/mikael_sin_mobil";
// Store the state of the monitored entity
String phoneSensorState = "OFF";
String bedTimeState = "OFF";
String homeState = "NOT_HOME";



// Defining pins
const int SENSOR_PIN = A0;
const int BUTTON_PIN = 15;
const int GREEN_LIGHT_PIN = 14;
const int RED_LIGHT_PIN = 12;
const int BUZZER_PIN = 16; // 5?

// Sensor threshold - adjust based on your sensor
const int OCCUPIED_THRESHOLD = 250;  // Value when bed is occupied (0-1023): 0 means alot of pressure, 1023 means absolutely no pressure at all.

// Debounce settings
const unsigned long DEBOUNCE_DELAY = 0;  // The time of how long the criteria must be held before the state of the sensor changes
unsigned long lastDebounceTime = 0;
bool lastBedState = false;
bool currentBedState = false;

WiFiClient espClient;
PubSubClient client(espClient);

// Initializing the buzzer struct with the BUZZER_PIN
// Buzzer buzzer(BUZZER_PIN);

// Defining code.h relevant variables
String prev_bedTimeState = "OFF";
String prev_phoneSensorState = "OFF";
bool firstStartTime_state = true;  // Start in a ready state
unsigned long now = 0;
unsigned long firstStartTime = 0;
unsigned long midStartTime = 0;
unsigned long useTime = 0;
unsigned long unusedTime = 0;
unsigned long elapsedTime = 0;

// MQTT callback for subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Handle phone sensor state updates
  if (topicStr == phone_sensor_state_topic) {
    phoneSensorState = String(message);
    // Serial.print("Received state for mikael_sin_mobil_interactive: "); // for debugging
    // Serial.println(phoneSensorState); // for debugging
  } 
  else if (topicStr == bed_time_state_topic) {
    bedTimeState = String(message);
  }
  else if (topicStr == home_state_topic) {
    homeState = String(message);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connect to MQTT and publish discovery config
  reconnect();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_password, availability_topic, 0, true, "offline")) {
      Serial.println("connected");
      client.publish(availability_topic, "online", true);
      delay(1000);
      publishDiscoveryConfig();
      // Subscribe to the monitored entity's state topic
      client.subscribe(bed_time_state_topic);
      Serial.print("Subscribed to phone sensor topic: ");
      Serial.println(bed_time_state_topic);
      // ------------------------------------------------
      client.subscribe(home_state_topic);
      Serial.print("Subscribed to phone sensor topic: ");
      Serial.println(home_state_topic);
      // ------------------------------------------------
      client.subscribe(phone_sensor_state_topic);
      Serial.print("Subscribed to phone sensor topic: ");
      Serial.println(phone_sensor_state_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishDiscoveryConfig() {
  String config = "{";
  config += "\"name\":\"Bed Sensor\",";
  config += "\"state_topic\":\"" + String(state_topic) + "\",";
  config += "\"availability_topic\":\"" + String(availability_topic) + "\",";
  config += "\"payload_on\":\"ON\",";
  config += "\"payload_off\":\"OFF\",";
  config += "\"device_class\":\"occupancy\",";
  config += "\"unique_id\":\"bed_sensor_001\",";
  config += "\"device\":{";
  config += "\"identifiers\":[\"bed_sensor_001\"],";
  config += "\"name\":\"Bed Sensor\",";
  config += "\"model\":\"ESP8266 Bed Sensor\",";
  config += "\"manufacturer\":\"HA-SleepGuard\"";
  config += "}";
  config += "}";

  Serial.println("Discovery config to be published:");
  Serial.println(config);

  client.publish(config_topic, config.c_str(), true);
  Serial.println("Published discovery config");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Read sensor value
  int sensorValue = analogRead(SENSOR_PIN);

  // Determine bed state
  bool bedOccupied = sensorValue <= OCCUPIED_THRESHOLD;

  // printOnChange("Bed sensor state: ", bedOccupied); // for debugging
  printOnChange("Phone interactive: ", phoneSensorState); // for debugging
  printOnChange("bed time: ", bedTimeState); // for debugging
  printOnChange("home state: ", homeState); // for debugging
  // Debounce logic
  if (bedOccupied != currentBedState) {
    lastDebounceTime = millis();
    currentBedState = bedOccupied;
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (currentBedState != lastBedState) {
      lastBedState = currentBedState;

      // Publish state to MQTT
      if (currentBedState) {
        client.publish(state_topic, "ON", true);
      } else {
        client.publish(state_topic, "OFF", true);
      }
    }
  }

  if (phoneSensorState == "ON") {
    printOnChange("Turning on green (D5)"); // for debugging
    digitalWrite(GREEN_LIGHT_PIN, HIGH);
  } else {
    printOnChange("Turning off green (D5)"); // for debugging
    digitalWrite(GREEN_LIGHT_PIN, LOW);
  }

  // The buzzer is wrong, fix it!
  if (phoneWarning(phoneSensorState, bedTimeState, homeState, currentBedState, 36000000, 5000)) {
    // Make the buzzer sequence start, as long as the if-sentence is true
    digitalWrite(RED_LIGHT_PIN, HIGH);
  } else {
    // Reset the buzzer settings so that it can start correctly next time it is being called
    digitalWrite(RED_LIGHT_PIN, LOW);
  }

  delay(100);  // determines the frequency of the whole code
}
