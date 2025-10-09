# Troubleshooting Guide

This guide helps you diagnose and fix common issues with the HA-SleepGuard bed sensor system.

## Quick Diagnostic Checklist

Before diving into specific issues, check:
- [ ] ESP8266 has power (LED should be on)
- [ ] WiFi credentials are correct in code
- [ ] MQTT broker is running and accessible
- [ ] Sensor is properly connected
- [ ] Latest code is uploaded to ESP8266
- [ ] Serial Monitor shows output (115200 baud)

---

## Issue 1: ESP8266 Won't Connect to WiFi

### Symptoms
- Serial Monitor shows "Connecting to WiFi..." forever
- No IP address displayed
- Constant dots (......) in Serial Monitor

### Solutions

#### Check WiFi Credentials
```cpp
// In bed_sensor.ino, verify these are correct:
const char* ssid = "YOUR_WIFI_SSID";        // Check capitalization
const char* password = "YOUR_WIFI_PASSWORD"; // Check special characters
```

#### WiFi Network Requirements
- **2.4GHz network required** - ESP8266 doesn't support 5GHz
- Check router settings for 2.4GHz band
- Try creating separate 2.4GHz SSID if dual-band

#### Hidden SSID
If your WiFi is hidden:
```cpp
WiFi.begin(ssid, password, 0, NULL, true);  // Last parameter enables hidden SSID
```

#### WiFi Signal Strength
- Move ESP8266 closer to router during testing
- Check if ESP8266 is too far from access point
- Add WiFi extender if needed

#### Special Characters in Password
- Avoid: `"`, `\`, `'` in password
- Or escape them: `\"`, `\\`, `\'`

### Verification
```
✅ Success looks like:
WiFi connected
IP address: 192.168.1.XXX
```

---

## Issue 2: MQTT Connection Failed

### Symptoms
- WiFi connects successfully
- "MQTT connection failed, rc=-X" in Serial Monitor
- Never shows "connected"

### MQTT Error Codes
- **rc=-4**: Connection timeout (broker not responding)
- **rc=-2**: Connection refused (check username/password)
- **rc=-1**: Connection failed (broker unavailable)
- **rc=0**: Success!

### Solutions

#### Check MQTT Broker IP
```cpp
const char* mqtt_server = "192.168.1.XXX";  // Must be broker's actual IP
```
- Don't use "localhost" or "127.0.0.1"
- Use broker's network IP address
- Test broker is running: `ping <broker_ip>`

#### Check MQTT Credentials
```cpp
const char* mqtt_user = "YOUR_MQTT_USER";
const char* mqtt_password = "YOUR_MQTT_PASSWORD";
```
- Verify credentials are correct
- Try connecting with MQTT client (MQTT Explorer) to verify

#### Check MQTT Port
```cpp
const int mqtt_port = 1883;  // Default port
```
- Verify broker is using port 1883
- Check firewall isn't blocking port

#### Test MQTT Broker
From computer on same network:
```bash
# Install mosquitto clients
sudo apt-get install mosquitto-clients

# Test connection
mosquitto_sub -h <broker_ip> -p 1883 -u <username> -P <password> -t test/topic
```

#### Home Assistant MQTT Setup
Verify MQTT integration in Home Assistant:
1. Settings → Devices & Services
2. Check MQTT is configured
3. Verify broker is running

### Verification
```
✅ Success looks like:
Attempting MQTT connection...connected
Published discovery config
```

---

## Issue 3: Sensor Not Detecting Occupancy

### Symptoms
- Everything connected
- Sensor value doesn't change when in bed
- Always shows "EMPTY" or "OCCUPIED"

### Solutions

#### Check Sensor Wiring
**For FSR:**
```
Multimeter test:
- Measure FSR resistance
- No pressure: >10kΩ
- With pressure: <5kΩ
- If always same value: wiring issue or bad FSR
```

**For Load Cell:**
```
Check color-coded wires:
- Red to E+
- Black to E-
- White to A-
- Green to A+
```

#### Verify A0 Connection
```cpp
// In Serial Monitor, you should see:
Sensor value: XXX  // This number should change
```
- If always 0: Check A0 connection
- If always 1023: Check GND connection or sensor

#### Calibrate Threshold
Open Serial Monitor and observe values:
```
1. Bed empty: note value (e.g., 100)
2. In bed: note value (e.g., 700)
3. Set threshold between them:
   const int OCCUPIED_THRESHOLD = 400;  // Example
```

#### Check Sensor Placement
- FSR must be under pressure point
- Place under mattress, not on top
- Position where torso weight is
- Tape securely to bed slat

### Verification
```
✅ Success looks like:
Sensor value: 150  (bed empty)
Bed: EMPTY
[get in bed]
Sensor value: 680  (bed occupied)
Bed: OCCUPIED
```

---

## Issue 4: False Triggers / Unstable Detection

### Symptoms
- Sensor rapidly switches between OCCUPIED/EMPTY
- Triggers when no one is in bed
- Doesn't trigger when someone is in bed

### Solutions

#### Increase Debounce Delay
```cpp
const unsigned long DEBOUNCE_DELAY = 5000;  // Increase from 3000 to 5000 or higher
```

#### Adjust Threshold
- If false "OCCUPIED": Increase threshold
- If false "EMPTY": Decrease threshold
```cpp
const int OCCUPIED_THRESHOLD = 512;  // Adjust this value
```

#### Check Power Supply
- Use quality USB charger (at least 1A)
- Avoid laptop USB ports (unstable power)
- Try different power adapter

#### Add Filtering (Advanced)
Replace simple reading with averaged reading:
```cpp
int getAverageSensorValue() {
  long sum = 0;
  for(int i = 0; i < 10; i++) {
    sum += analogRead(SENSOR_PIN);
    delay(10);
  }
  return sum / 10;
}

// In loop():
int sensorValue = getAverageSensorValue();
```

### Verification
```
✅ Success: Stable readings, no rapid switching
```

---

## Issue 5: Home Assistant Not Showing Sensor

### Symptoms
- ESP8266 connects to MQTT successfully
- No binary_sensor.bed_sensor in Home Assistant
- Entity not appearing

### Solutions

#### Check MQTT Discovery
In Home Assistant:
1. Settings → Devices & Services → MQTT
2. Click "Configure"
3. Verify "Enable discovery" is checked
4. Discovery prefix should be "homeassistant"

#### Verify MQTT Messages
Use MQTT Explorer or mosquitto_sub:
```bash
mosquitto_sub -h <broker_ip> -t "homeassistant/#" -v
```
Should see messages on:
- homeassistant/binary_sensor/bed_sensor/config
- homeassistant/binary_sensor/bed_sensor/state

#### Manually Check Entity
In Home Assistant:
1. Developer Tools → States
2. Look for "binary_sensor.bed_sensor"
3. If not there, check MQTT messages

#### Restart Home Assistant
Sometimes needed after new MQTT devices:
1. Settings → System → Restart
2. Wait for restart
3. Check for entity again

#### Check Unique ID Conflicts
If you've uploaded code multiple times with different unique_ids:
```cpp
// In publishDiscoveryConfig(), ensure unique_id is unique:
"unique_id":"bed_sensor_001",  // Change if needed
```

### Verification
```
✅ Success:
- Entity appears in Developer Tools → States
- Shows "on" or "off" state
- Updates when sensor changes
```

---

## Issue 6: Alarm Not Triggering

### Symptoms
- Bed sensor works correctly
- Alarm time reached but nothing happens
- input_boolean.alarm_active stays off

### Solutions

#### Check Automation Status
In Home Assistant:
1. Settings → Automations & Scenes
2. Verify automations are enabled
3. Check "Trigger Aggressive Alarm" automation

#### Verify Input Helpers
Check these exist:
- input_boolean.aggressive_alarm_enabled
- input_datetime.alarm_time
- input_boolean.alarm_active

Create if missing:
1. Settings → Devices & Services → Helpers
2. Create Input Boolean and Input Datetime

#### Check Time
- Verify input_datetime.alarm_time is set
- Check Home Assistant time zone matches local time
- Settings → System → General → Time Zone

#### Test Manually
Trigger alarm manually:
1. Developer Tools → Services
2. Service: input_boolean.turn_on
3. Entity: input_boolean.alarm_active
4. Call Service

### Verification
```
✅ Success:
- Alarm triggers at set time
- input_boolean.alarm_active turns on
- Alarm actions execute
```

---

## Issue 7: Alarm Won't Stop When Out of Bed

### Symptoms
- Alarm triggers correctly
- Getting out of bed doesn't stop alarm
- input_boolean.alarm_active stays on

### Solutions

#### Check Automation Conditions
Verify "Stop Alarm When Out of Bed" automation:
```yaml
trigger:
  - platform: state
    entity_id: binary_sensor.bed_sensor
    from: 'on'
    to: 'off'
    for:
      seconds: 5  # Adjust this delay
```

#### Verify Sensor State
- Check binary_sensor.bed_sensor shows 'off' when out of bed
- Use Developer Tools → States to monitor
- May need to recalibrate sensor

#### Check Automation Enabled
- Ensure "Stop Alarm When Out of Bed" automation is enabled
- Check automation conditions are met

#### Manual Stop Works?
Test manual stop:
1. Turn off input_boolean.alarm_active manually
2. If this works, issue is in automation
3. If this doesn't work, check alarm action automation

### Verification
```
✅ Success:
- Get out of bed
- Wait 5 seconds
- Alarm stops automatically
```

---

## Issue 8: Upload Errors

### Symptoms
- Can't upload sketch to ESP8266
- "espcomm_sync failed" error
- "Timed out waiting for packet header" error

### Solutions

#### Select Correct Board
Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)

#### Select Correct Port
Tools → Port → (select COM port or /dev/ttyUSB0)
- Windows: COM3, COM4, etc.
- Mac: /dev/cu.usbserial-XXXX
- Linux: /dev/ttyUSB0

#### Install Drivers
NodeMCU/Wemos may need CH340 or CP2102 drivers:
- Download from manufacturer website
- Install and restart computer

#### Try Different Upload Speed
Tools → Upload Speed → 115200 (or try 9600)

#### Hold FLASH Button
Some boards require:
1. Hold FLASH/BOOT button
2. Click Upload
3. Release when upload starts

#### Try Different USB Cable
- Some cables are power-only
- Use data-capable USB cable

### Verification
```
✅ Success:
Sketch uses XXXX bytes (XX%) of program storage space.
```

---

## Issue 9: No Serial Monitor Output

### Symptoms
- Upload successful
- Serial Monitor blank or gibberish
- Can't see debug messages

### Solutions

#### Check Baud Rate
Serial Monitor baud rate must match code:
```cpp
Serial.begin(115200);  // Code sets to 115200
```
Set Serial Monitor to 115200 baud (bottom right dropdown)

#### Select Correct Port
Tools → Port → Select ESP8266 port

#### Line Ending
Set line ending to "Both NL & CR"

#### Reset ESP8266
Press RST button on board after opening Serial Monitor

### Verification
```
✅ Success: See WiFi connection messages
```

---

## Advanced Debugging

### Enable Detailed Logging
Add to code:
```cpp
void loop() {
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
  Serial.print("MQTT State: ");
  Serial.println(client.state());
  // ... rest of loop
}
```

### MQTT Debug with Mosquitto
```bash
# Subscribe to all topics
mosquitto_sub -h <broker_ip> -t "#" -v

# Publish test message
mosquitto_pub -h <broker_ip> -t "homeassistant/binary_sensor/bed_sensor/state" -m "ON"
```

### Home Assistant Logs
Check HA logs for MQTT issues:
1. Settings → System → Logs
2. Look for MQTT or sensor errors

---

## Getting Help

### Information to Provide
When asking for help, include:
1. **Hardware**: ESP8266 model, sensor type
2. **Error messages**: From Serial Monitor
3. **Wiring**: How components are connected
4. **Code changes**: Any modifications made
5. **Home Assistant version**
6. **MQTT broker type**: Mosquitto, built-in, etc.

### Useful Commands
```bash
# Check MQTT broker
systemctl status mosquitto

# Test MQTT from HA
mosquitto_sub -h localhost -t "#" -v

# Check HA logs
tail -f /config/home-assistant.log
```

---

## Still Not Working?

### Reset to Factory Settings
1. Erase ESP8266 flash completely
2. Re-upload base ESP8266 blink sketch
3. Verify basic functionality
4. Upload bed sensor code fresh

### Start Simple
1. Test with just Serial Monitor output (no MQTT)
2. Add WiFi connection
3. Add MQTT connection
4. Add sensor reading
5. Add Home Assistant integration

### Common Mistakes Checklist
- [ ] Used 2.4GHz WiFi (not 5GHz)
- [ ] Correct MQTT broker IP address
- [ ] Sensor properly wired with voltage divider
- [ ] Threshold value calibrated
- [ ] All entities exist in Home Assistant
- [ ] Automations are enabled
- [ ] MQTT discovery is enabled

---

**Pro Tip:** Keep a working backup of your code. When it works, save a copy before making changes!
