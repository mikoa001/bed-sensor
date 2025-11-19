/*
 *  HA-SleepGuard Bed Sensor
 *  ESP8266 Bed Occupancy Sensor for Home Assistant
 * 
 *  This sketch reads a pressure/weight sensor to detect bed occupancy
 *  and publishes the state to Home Assistant via MQTT
 * 
 *  This code also supports OTA uploads.
 * 
 *  It subscribes (MQTT) to the following states in my Home Assistant setup:
 * - binary_sensor.bedTimeState
 * - binary_sensor.device_tracker/mikael_sin_mobil
 * - binary_sensor.mikael_sin_mobil_interactive
 */

#include "config.h"

struct PhoneWarningTracker {
  bool firstStartTime_state = true;
  String prev_bedTimeState = "OFF";
  String prev_phoneSensorState = "OFF";
  unsigned long firstStartTime = 0;
  unsigned long midStartTime = 0;
  unsigned long useTime = 0;
  unsigned long elapsedTime = 0;
  unsigned long unusedTime = 0;
  bool alarmTriggered = false;
  bool graceActive = false;
  unsigned long graceStartTime = 0;

  bool check(const String& phoneSensorState, 
             const String& bedTimeState, 
             const String& homeState, 
             bool bedState, 
             const unsigned long useTime_treshold,
             const unsigned long unusedTime_treshold, 
             const unsigned long graceTime_treshold) {

    // If there is a rising edge response of bed time
    if (bedTimeState == "ON" && prev_bedTimeState == "OFF") {
      firstStartTime_state = true;
      alarmTriggered = false;
      graceActive = false;
      graceStartTime = 0;
    }
    prev_bedTimeState = bedTimeState;

    // If it is bed time and I am home
    if (bedTimeState == "ON" && homeState == "HOME") {
      // If I am laying in bed and I am on my phone, and it is the first time of my bed time
      if (bedState && phoneSensorState == "ON" && firstStartTime_state) {
        // Initializing the start time of the current phone session
        firstStartTime = now;
        firstStartTime_state = false;
      }
      // If the initialization have previously runned
      if (!firstStartTime_state) {
        // Calculate the elapsed time from the triggered start of the current phone session
        elapsedTime = now - firstStartTime;
        // If there is a rising edge response of the phone state, need to ignore the midStartTime initialization
        if (phoneSensorState == "ON" && prev_phoneSensorState == "OFF") {
          // If we use the phone the very first time, we need to ignore the midStartTime initialization
          if (midStartTime != 0) {
            // If we are in a grace period, check if the gap time have exceeded the grace
            unusedTime += now - midStartTime;
            if (alarmTriggered && (now - midStartTime) < unusedTime_treshold) {
              graceActive = true;
              graceStartTime = now;
            }
            midStartTime = 0;
          }
        }
        // If there is a falling edge response of the phone state
        else if (phoneSensorState == "OFF" && prev_phoneSensorState == "ON") {
          midStartTime = now;
        }
        prev_phoneSensorState = phoneSensorState;
        // If i am on my phone, but it is not the first time of my bed time
        // Maybe have some debounce for when I am not in bed? Like, I am going to game all night but I triggered the state and still use the phone???
        if (phoneSensorState == "ON") {
          useTime = elapsedTime - unusedTime;
        }
        // The phone have not been used consequtively for a long enough period, ignoring total unused time, reset everything
        else if (midStartTime != 0 && (now - midStartTime) >= unusedTime_treshold) {
          firstStartTime_state = true;
          firstStartTime = 0;
          midStartTime = 0;
          useTime = 0;
          elapsedTime = 0;
          unusedTime = 0;
          alarmTriggered = false;
          graceActive = false;
          graceStartTime = 0;
          return false;
        }

        // Grace handling
        if (graceActive) {
          if (now - graceStartTime < graceTime_treshold) {
            return false; // suppress alarm during grace
          } else {
            graceActive = false; // grace expired
          }
        }

        // Check if the use time have exceeded the treshold and return true if so
        if (phoneSensorState == "ON" && useTime >= useTime_treshold) {
          alarmTriggered = true;
          return true;
        }
      }
      return false;
    }
    // It is not bed time anymore or I am not home
    else {
      // Outside conditions: full reset
      firstStartTime_state = true;
      firstStartTime = 0;
      midStartTime = 0;
      useTime = 0;
      elapsedTime = 0;
      unusedTime = 0;
      alarmTriggered = false;
      graceActive = false;
      graceStartTime = 0;
      return false;
    }
  }
};

// Create a global instance of the tracker
PhoneWarningTracker phoneWarningTracker;

void startBuzzerSequence() {
  if (buzzerActive) return;
  buzzerActive = true;
  buzzerBeepIndex = 0;
  buzzerPhase = 0; // start with ON
  buzzerLastToggle = now;
  digitalWrite(BUZZER_PIN, HIGH); // turn buzzer on (assume active HIGH)
  digitalWrite(RED_LIGHT_PIN, HIGH);
}

void stopBuzzer() {
  buzzerActive = false;
  buzzerBeepIndex = 0;
  buzzerPhase = 0;
  digitalWrite(BUZZER_PIN, LOW); // ensure buzzer off
  digitalWrite(RED_LIGHT_PIN, LOW);
}

void updateBuzzer() {
  if (!buzzerActive) return;
  switch (buzzerPhase) {
    case 0: // ON
      if (now - buzzerLastToggle >= BUZZS_BEEP_ON_MS) {
        // turn off for inter-beep gap (or burst pause if last beep)
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RED_LIGHT_PIN, LOW);
        buzzerLastToggle = now;
        if (buzzerBeepIndex + 1 >= BUZZS_BEEPS_PER_BURST) {
          buzzerPhase = 2; // go to burst pause
        } else {
          buzzerPhase = 1; // short off between beeps
        }
      } 
      break;
    case 1: // BETWEEN-BEEPS
      if (now - buzzerLastToggle >= BUZZS_BEEP_OFF_MS) {
        buzzerBeepIndex++;
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(RED_LIGHT_PIN, HIGH);
        buzzerLastToggle = now;
        buzzerPhase = 0;
      }
      break;
    case 2: // BURST-PAUSE
      if (now - buzzerLastToggle >= BUZZS_BURST_PAUSE_MS) {
        // start new burst
        buzzerBeepIndex = 0;
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(RED_LIGHT_PIN, HIGH);
        buzzerLastToggle = now;
        buzzerPhase = 0;
      }
      break;
  }
}

// MQTT callback for subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  String message;
  for (unsigned int i = 0; i < length; i++) {message += (char)payload[i];}

  // Handle HA state updates
  if (topicStr == phone_sensor_state_topic) phoneSensorState = String(message);
  else if (topicStr == bed_time_state_topic) bedTimeState = String(message);
  else if (topicStr == home_state_topic) homeState = String(message);
  // Handle control/command topics from Home Assistant
  else if (topicStr == String(use_time_command_topic)) {
    int minutes = atoi(message.c_str());
    useTime_treshold = (unsigned long)minutes * 60000UL;
    client.publish(use_time_state_topic, String(minutes).c_str(), true);
  }
  else if (topicStr == String(unused_time_command_topic)) {
    int minutes = atoi(message.c_str());
    unusedTime_treshold = (unsigned long)minutes * 60000UL;
    client.publish(unused_time_state_topic, String(minutes).c_str(), true);
  }
  else if (topicStr == String(grace_time_command_topic)) {
    int seconds = atoi(message.c_str());
    graceTime_treshold = (unsigned long)seconds * 1000UL;
    client.publish(grace_time_state_topic, String(seconds).c_str(), true);
  }
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
  Serial.print("WiFi status: ");
  Serial.println(WiFi.status());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // OTA setup
  ArduinoOTA.setHostname("ESP8266-OtA"); // Optional: set a custom name
  ArduinoOTA.begin();
  Serial.println("Ready for OTA");
}

void reconnect() {
  // Helper to map PubSubClient states to human-readable reasons
  auto mqttReason = [](int state) -> const char* {
    switch (state) {
      case -4: return "TCP connection failed";
      case -3: return "Failed to send MQTT CONNECT";
      case -2: return "Network disconnected (no TCP)";
      case -1: return "Connection refused (bad protocol)";
      case 0:  return "Connection refused (server unavailable)";
      case 1:  return "Connection refused (bad credentials)";
      case 2:  return "Connection refused (not authorised)";
      default: return "Unknown";
    }
  };

  // Only try to connect if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping MQTT reconnect");
    discovery_published = false; // Reset flag if wifi is down
    return;
  }

  while (!client.connected()) {
    // Reset discovery flag on each connection attempt
    discovery_published = false;
    // Use a unique client id to avoid clashes (append MAC)
    String clientId = String(mqtt_client_id) + "-" + WiFi.macAddress();
    Serial.print("Attempting MQTT connection with id: ");
    Serial.println(clientId);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password, availability_topic, 0, true, "offline")) {
      Serial.println("MQTT connected");
      client.publish(availability_topic, "online", true);
      
      // Subscribe to the monitored entity's state topics
      client.subscribe(bed_time_state_topic);
      Serial.print("Subscribed to: ");
      Serial.println(bed_time_state_topic);
      client.subscribe(home_state_topic);
      Serial.print("Subscribed to: ");
      Serial.println(home_state_topic);
      client.subscribe(phone_sensor_state_topic);
      Serial.print("Subscribed to: ");
      Serial.println(phone_sensor_state_topic);
      // Subscribe to control topics so HA can change thresholds
      client.subscribe(use_time_command_topic);
      Serial.print("Subscribed to: ");
      Serial.println(use_time_command_topic);
      client.subscribe(unused_time_command_topic);
      Serial.print("Subscribed to: ");
      Serial.println(unused_time_command_topic);
      client.subscribe(grace_time_command_topic);
      Serial.print("Subscribed to: ");
      Serial.println(grace_time_command_topic);
    } else {
      int rc = client.state();
      Serial.print("MQTT connect failed, rc=");
      Serial.print(rc);
      Serial.print(" => ");
      Serial.println(mqttReason(rc));
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void publishDiscoveryConfig() {
  // This function is now responsible for publishing discovery messages for all entities.
  
  // Common device information
  String device_json = "\"device\":{\"identifiers\":[\"bed_sensor_001\"],\"name\":\"Bed Sensor\",\"model\":\"ESP8266 Bed Sensor\",\"manufacturer\":\"Mikael Kofoed\"}";

  // 1. Bed Occupancy Sensor (Binary Sensor)
  String config_bed_sensor = "{\"name\":\"Bed Sensor\",\"state_topic\":\"" + String(state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"payload_on\":\"ON\",\"payload_off\":\"OFF\",\"device_class\":\"occupancy\",\"unique_id\":\"bed_sensor_001_occupancy\"," + device_json + "}";
  if (!client.publish("homeassistant/binary_sensor/bed_sensor/config", config_bed_sensor.c_str(), true)) {
    Serial.println("Failed to publish Bed Sensor discovery.");
  }

  // 2. WiFi Signal Sensor (RSSI)
  String config_rssi = "{\"name\":\"Bed Sensor WiFi Signal\",\"state_topic\":\"" + String(rssi_state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"device_class\":\"signal_strength\",\"unit_of_measurement\":\"dBm\",\"unique_id\":\"bed_sensor_001_rssi\"," + device_json + "}";
  if (!client.publish("homeassistant/sensor/bed_sensor_rssi/config", config_rssi.c_str(), true)) {
    Serial.println("Failed to publish RSSI Sensor discovery.");
  }

  // 3. IP Address Sensor
  String config_ip = "{\"name\":\"Bed Sensor IP Address\",\"state_topic\":\"" + String(ip_state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"icon\":\"mdi:ip-network\",\"unique_id\":\"bed_sensor_001_ip\"," + device_json + "}";
  if (!client.publish("homeassistant/sensor/bed_sensor_ip/config", config_ip.c_str(), true)) {
    Serial.println("Failed to publish IP Sensor discovery.");
  }

  // 4. Raw Pressure Sensor
  String config_pressure = "{\"name\":\"Bed Sensor Raw Pressure\",\"state_topic\":\"" + String(pressure_state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"icon\":\"mdi:gauge\",\"unit_of_measurement\":\"\",\"state_class\":\"measurement\",\"unique_id\":\"bed_sensor_001_pressure\"," + device_json + "}";
  if (!client.publish("homeassistant/sensor/bed_sensor_pressure/config", config_pressure.c_str(), true)) {
    Serial.println("Failed to publish Pressure Sensor discovery.");
  }

  // 5. Phone Usage Sensor
  String config_phone_usage = "{\"name\":\"Bed Sensor Phone Usage\",\"state_topic\":\"" + String(phone_usage_state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"device_class\":\"duration\",\"unit_of_measurement\":\"min\",\"icon\":\"mdi:cellphone-timer\",\"unique_id\":\"bed_sensor_001_phone_usage\"," + device_json + "}";
  if (!client.publish("homeassistant/sensor/bed_sensor_phone_usage/config", config_phone_usage.c_str(), true)) {
    Serial.println("Failed to publish Phone Usage Sensor discovery.");
  }

  // 6. Phone Warning use time treshold input
  String config_use_time_treshold = "{\"name\":\"Bed Sensor Phone Use Time Treshold\",\"command_topic\":\"" + String(use_time_command_topic) + "\",\"state_topic\":\"" + String(use_time_state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"min\":0,\"max\":120,\"step\":1,\"unit_of_measurement\":\"min\",\"mode\":\"box\",\"unique_id\":\"bed_sensor_001_use_time_treshold\"," + device_json + "}";
  if (!client.publish("homeassistant/number/bed_sensor_phone_use_time_treshold/config", config_use_time_treshold.c_str(), true)) {
    Serial.println("Failed to publish Use Time Treshold discovery.");
  }

  // 7. Phone Warning unused time treshold input
  String config_unused_time_treshold = "{\"name\":\"Bed Sensor Phone Unused Time Treshold\",\"command_topic\":\"" + String(unused_time_command_topic) + "\",\"state_topic\":\"" + String(unused_time_state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"min\":0,\"max\":120,\"step\":10,\"unit_of_measurement\":\"min\",\"mode\":\"box\",\"unique_id\":\"bed_sensor_001_unused_time_treshold\"," + device_json + "}";
  if (!client.publish("homeassistant/number/bed_sensor_phone_unused_time_treshold/config", config_unused_time_treshold.c_str(), true)) {
    Serial.println("Failed to publish Unused Time Treshold discovery.");
  }

  // 8. Phone Warning grace time treshold input
  String config_grace_time_treshold = "{\"name\":\"Bed Sensor Phone Grace Time Treshold\",\"command_topic\":\"" + String(grace_time_command_topic) + "\",\"state_topic\":\"" + String(grace_time_state_topic) + "\",\"availability_topic\":\"" + String(availability_topic) + "\",\"min\":0,\"max\":120,\"step\":5,\"unit_of_measurement\":\"sec\",\"mode\":\"box\",\"unique_id\":\"bed_sensor_001_grace_time_treshold\"," + device_json + "}";
  if (!client.publish("homeassistant/number/bed_sensor_phone_grace_time_treshold/config", config_grace_time_treshold.c_str(), true)) {
    Serial.println("Failed to publish Grace Time Treshold discovery.");
  }

  Serial.println("Finished publishing all discovery configs.");
}

void publishInitialStates() {
  Serial.println("Publishing initial states...");
  // Publish initial state for use time threshold (in minutes)
  client.publish(use_time_state_topic, String(useTime_treshold / 60000).c_str(), true);
  // Publish initial state for unused time threshold (in minutes)
  client.publish(unused_time_state_topic, String(unusedTime_treshold / 60000).c_str(), true);
  // Publish initial state for grace time threshold (in seconds)
  client.publish(grace_time_state_topic, String(graceTime_treshold / 1000).c_str(), true);
  Serial.println("Finished publishing initial states.");
}

void setup() {
  digitalWrite(RED_LIGHT_PIN, HIGH);

  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_BUILTIN_1, OUTPUT);
  pinMode(LED_BUILTIN_2, OUTPUT);

  // For most ESP8266 dev boards the built-in LEDs is active LOW, so HIGH = OFF:
  digitalWrite(LED_BUILTIN_1, HIGH);
  digitalWrite(LED_BUILTIN_2, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // Set a larger buffer size for the MQTT client to handle the large discovery payload
  client.setBufferSize(1024);

  // Connect to MQTT and publish discovery config
  reconnect();

  // Ensure RED light off at startup
  digitalWrite(RED_LIGHT_PIN, LOW);
}

void loop() {
  // OTA HANDLER
  // =============================================================================================================
  // service OTA requests
  ArduinoOTA.handle();

  // VARIABLE UPDATES
  // =============================================================================================================
  // Update current time
  now = millis();
  // Read sensor value
  int sensorValue = analogRead(SENSOR_PIN);
  // Determine bed state
  bool bedOccupied = sensorValue <= OCCUPIED_THRESHOLD;

  // MQTT
  // =============================================================================================================
  // Ensure MQTT connection
  if (!client.connected()) reconnect();
  // MQTT client loop
  client.loop();
  // Publish discovery config once after a successful connection
  if (client.connected() && !discovery_published) {
    publishDiscoveryConfig();
    publishInitialStates(); // Publish initial input states to default values
    discovery_published = true; // Set flag so it doesn't run again
  }
  // Debounce logic
  if (bedOccupied != currentBedState) {
    lastDebounceTime = now;
    currentBedState = bedOccupied;
  }
  if ((now - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (currentBedState != lastBedState) {
      lastBedState = currentBedState;
      // Publish bed state to MQTT
      if (currentBedState) {
        client.publish(state_topic, "ON", true);
      } else {
        client.publish(state_topic, "OFF", true);
      }
    }
  }
  // Periodically publish diagnostic and sensor data to MQTT
  if (client.connected() && (millis() - lastDataPublishTime > DATA_PUBLISH_INTERVAL)) {
    // 1. Publish WiFi Signal Strength (RSSI)
    client.publish(rssi_state_topic, String(WiFi.RSSI()).c_str(), true);
    // 2. Publish IP Address
    client.publish(ip_state_topic, WiFi.localIP().toString().c_str(), true);
    // 3. Publish Raw Pressure Value
    client.publish(pressure_state_topic, String(sensorValue).c_str(), true);
    // 4. Publish Phone Usage Time (in minutes)
    unsigned long useTimeMinutes = phoneWarningTracker.useTime / 60000;
    client.publish(phone_usage_state_topic, String(useTimeMinutes).c_str(), true);
    
    // Update last publish time
    lastDataPublishTime = millis();
  }

  // MAIN LOGIC
  // =============================================================================================================
  // When the phone sensor is active, turn on the green light
  if (phoneSensorState == "ON") {
    digitalWrite(GREEN_LIGHT_PIN, HIGH);
  } else {
    digitalWrite(GREEN_LIGHT_PIN, LOW);
  }

  // Check if phone warning conditions are met
  if (phoneWarningTracker.check(phoneSensorState, 
                                bedTimeState, 
                                homeState, 
                                currentBedState, 
                                useTime_treshold, 
                                unusedTime_treshold, 
                                graceTime_treshold)) {
    if (!buzzerRequested) {
      buzzerRequested = true;
      startBuzzerSequence();
    }
  } else {
    if (buzzerRequested) {
      buzzerRequested = false;
      stopBuzzer();
    }
  }

  // Update buzzer state machine (non-blocking)
  updateBuzzer();

  delay(50);  // determines the frequency of the whole code
}