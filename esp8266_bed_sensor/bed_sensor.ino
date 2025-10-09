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
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker settings
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;
const char* mqtt_user = "YOUR_MQTT_USER";
const char* mqtt_password = "YOUR_MQTT_PASSWORD";
const char* mqtt_client_id = "bed_sensor";

// MQTT Topics
const char* state_topic = "homeassistant/binary_sensor/bed_sensor/state";
const char* config_topic = "homeassistant/binary_sensor/bed_sensor/config";
const char* availability_topic = "homeassistant/binary_sensor/bed_sensor/availability";

// Sensor pin - connected to pressure/weight sensor
const int SENSOR_PIN = A0;  // Analog pin for pressure sensor

// Sensor threshold - adjust based on your sensor
const int OCCUPIED_THRESHOLD = 512;  // Value when bed is occupied (0-1023)

// Debounce settings
const unsigned long DEBOUNCE_DELAY = 3000;  // 3 seconds
unsigned long lastDebounceTime = 0;
bool lastBedState = false;
bool currentBedState = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  
  // Connect to MQTT and publish discovery config
  reconnect();
  publishDiscoveryConfig();
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
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishDiscoveryConfig() {
  // Home Assistant MQTT Discovery
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
  Serial.print("Sensor value: ");
  Serial.println(sensorValue);

  // Determine bed state
  bool bedOccupied = sensorValue >= OCCUPIED_THRESHOLD;

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
        Serial.println("Bed: OCCUPIED");
      } else {
        client.publish(state_topic, "OFF", true);
        Serial.println("Bed: EMPTY");
      }
    }
  }

  delay(500);  // Check sensor every 500ms
}
