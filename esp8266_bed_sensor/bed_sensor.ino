/*
 * HA-SleepGuard Bed Sensor
 * ESP8266 Bed Occupancy Sensor for Home Assistant
 * 
 * This sketch reads a pressure/weight sensor to detect bed occupancy
 * and publishes the state to Home Assistant via MQTT
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "YOUR_INTERNETT_SSID";
const char* password = "YOUR_INTERNET_PASSWORD";

// MQTT Broker settings
const char* mqtt_server = "LOCAL_HA_SERVER_IP";     // IP of your Home Assistant device, found in the network settings in the HA UI
const int mqtt_port = 1883;                         // Default MQTT port, change it if you have manually changed the port in the MQTT add-on (Mosquitto)
const char* mqtt_user = "MQTT_USER";                // The HA user (not person, there is a difference) where the mqtt comms will happen on, for limiting the access in case of an cyberattack
const char* mqtt_password = "MQTT_USER_PASSWORD";   // A strong password for the mqtt user so that it is hard to hack
const char* mqtt_client_id = "bed_sensor";          // the id_of this single client (the microcontroller)

// MQTT Topics
const char* state_topic = "homeassistant/binary_sensor/bed_sensor/state";
const char* config_topic = "homeassistant/binary_sensor/bed_sensor/config";
const char* availability_topic = "homeassistant/binary_sensor/bed_sensor/availability";

// Defining pins
const int SENSOR_PIN = A0;
const int BUTTON_PIN = 15;
const int GREEN_LIGHT_PIN = 14;
const int RED_LIGHT_PIN = 12;
const int BUZZER_PIN = 5;

// Sensor threshold - adjust based on your sensor
const int OCCUPIED_THRESHOLD = 250;  // Value when bed is occupied (0-1023): 0 means alot of pressure, 1023 means absolutely no pressure at all.

// Debounce settings
const unsigned long DEBOUNCE_DELAY = 0;  // The time of how long the criteria must be held before the state of the sensor changes
unsigned long lastDebounceTime = 0;
bool lastBedState = false;
bool currentBedState = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  
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
      delay(1000); // Add a small delay
      publishDiscoveryConfig();
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
  Serial.println(config); // Print out the full JSON for debugging

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
  
  // Serial.print("Sensor value: "); // for debugging
  // Serial.println(sensorValue);    // for debugging

  // Determine bed state
  bool bedOccupied = sensorValue <= OCCUPIED_THRESHOLD;

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
        // Serial.println("Bed: OCCUPIED"); // for debugging
      } else {
        client.publish(state_topic, "OFF", true);
        // Serial.println("Bed: EMPTY"); // for debugging
      }
    }
  }
  
  delay(0);  // Execute the code with either a delay or as fast as possible
}
