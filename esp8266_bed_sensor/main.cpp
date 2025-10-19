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

void startBuzzerSequence() {
  if (buzzerActive) return;
  buzzerActive = true;
  buzzerBeepIndex = 0;
  buzzerPhase = 0; // start with ON
  buzzerLastToggle = millis();
  digitalWrite(BUZZER_PIN, HIGH); // turn buzzer on (assume active HIGH)
}

void stopBuzzer() {
  buzzerActive = false;
  buzzerBeepIndex = 0;
  buzzerPhase = 0;
  digitalWrite(BUZZER_PIN, LOW); // ensure buzzer off
}

void updateBuzzer() {
  if (!buzzerActive) return;
  unsigned long nowm = millis();
  switch (buzzerPhase) {
    case 0: // ON
      if (nowm - buzzerLastToggle >= BUZZS_BEEP_ON_MS) {
        // turn off for inter-beep gap (or burst pause if last beep)
        digitalWrite(BUZZER_PIN, LOW);
        buzzerLastToggle = nowm;
        if (buzzerBeepIndex + 1 >= BUZZS_BEEPS_PER_BURST) {
          buzzerPhase = 2; // go to burst pause
        } else {
          buzzerPhase = 1; // short off between beeps
        }
      }
      break;
    case 1: // BETWEEN-BEEPS
      if (nowm - buzzerLastToggle >= BUZZS_BEEP_OFF_MS) {
        buzzerBeepIndex++;
        digitalWrite(BUZZER_PIN, HIGH);
        buzzerLastToggle = nowm;
        buzzerPhase = 0;
      }
      break;
    case 2: // BURST-PAUSE
      if (nowm - buzzerLastToggle >= BUZZS_BURST_PAUSE_MS) {
        // start new burst
        buzzerBeepIndex = 0;
        digitalWrite(BUZZER_PIN, HIGH);
        buzzerLastToggle = nowm;
        buzzerPhase = 0;
      }
      break;
  }
}

bool phoneWarning(const String& phoneSensorState, const String& bedTimeState, const String& homeState, bool bedState, 
                  const unsigned long unusedTime_treshold, const unsigned long useTime_treshold) {
  // The current time
  now = millis();

  // If there is a rising edge response of bed time
  if (bedTimeState == "ON" && prev_bedTimeState == "OFF") {
    firstStartTime_state = true;
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
      // If there is a rising edge response of the phone state
      if (phoneSensorState == "ON" && prev_phoneSensorState == "OFF") {
        // If we use the phone the very first time, we need to ignore the midStartTime initialization
        if (midStartTime != 0) {
          unusedTime += now - midStartTime;  // Add the time it was unused
          midStartTime = 0; // consumed
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

      // The phone have not been used for a long enough period that the whole phone restriction can be resetted
      else if (now - midStartTime >= unusedTime_treshold) {
        firstStartTime_state = true;
        firstStartTime = 0;
        midStartTime = 0;
        useTime = 0;
        elapsedTime = 0;
        unusedTime = 0;
      }
      if (phoneSensorState == "ON" && (useTime >= useTime_treshold)) {
        return true;
      } else {
        return false;
      }
    }
  }
  // It is not bed time anymore or I am not home
  else {
    unusedTime = 0;
    return false;
  }
  // Fallback: if execution reaches here for any unexpected reason, return false
  return false;
}

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
  digitalWrite(RED_LIGHT_PIN, HIGH);

  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GREEN_LIGHT_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // For most ESP8266 dev boards the LED is active LOW, so HIGH = OFF:
  digitalWrite(LED_BUILTIN, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connect to MQTT and publish discovery config
  reconnect();

  // Ensure RED light off at startup
  digitalWrite(RED_LIGHT_PIN, LOW);
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
  // OTA setup
  ArduinoOTA.setHostname("ESP8266-OtA"); // Optional: set a custom name
  ArduinoOTA.begin();
  Serial.println("Ready for OTA");
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
  ArduinoOTA.handle();   // service OTA requests
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Read sensor value
  int sensorValue = analogRead(SENSOR_PIN);

  // Determine bed state
  bool bedOccupied = sensorValue <= OCCUPIED_THRESHOLD;

  /* Debugging prints for state changes
  if (bedOccupied != prev_bedOccupied) {
    Serial.print("Bed occupied state changed to: ");
    Serial.println(bedOccupied ? "ON" : "OFF");
    prev_bedOccupied = bedOccupied;
  }
  if (phoneSensorState != prev_phoneSensorState) {
    Serial.print("Phone sensor state changed to: ");
    Serial.println(phoneSensorState);
    if (bedTimeState == "OFF") {
      prev_phoneSensorState = phoneSensorState;
    }
  }
  if (bedTimeState != prev_bedTimeState) {
    Serial.print("Bed time state changed to: ");
    Serial.println(bedTimeState);
  }
  if (homeState != prev_homeState) {
    Serial.print("Home state changed to: ");
    Serial.println(homeState);
    prev_homeState = homeState;
  }
  */

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

  // A test to control the green light based on phone sensor state
  if (phoneSensorState == "ON") {
    digitalWrite(GREEN_LIGHT_PIN, HIGH);
  } else {
    digitalWrite(GREEN_LIGHT_PIN, LOW);
  }

  if (bedOccupied) {
    digitalWrite(LED_BUILTIN, LOW); // LED ON (active LOW)
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  if (phoneWarning(phoneSensorState, bedTimeState, homeState, currentBedState, 10000, 10000)) {
    digitalWrite(RED_LIGHT_PIN, HIGH);
    if (!buzzerRequested) {
      buzzerRequested = true;
      startBuzzerSequence();
    }
  } else {
    digitalWrite(RED_LIGHT_PIN, LOW);
    if (buzzerRequested) {
      buzzerRequested = false;
      stopBuzzer();
    }
  }

  // Update buzzer state machine (non-blocking)
  updateBuzzer();

  delay(100);  // determines the frequency of the whole code
}