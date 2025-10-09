# Quick Start Guide

Get your HA-SleepGuard bed sensor up and running in under an hour!

## What You Need

### Hardware (Total: ~$20-30)
- ✅ ESP8266 board (NodeMCU or Wemos D1 Mini)
- ✅ FSR sensor (Force Sensitive Resistor)
- ✅ 10kΩ resistor
- ✅ USB cable and power adapter
- ✅ Jumper wires
- ✅ Breadboard (optional for testing)

### Software
- ✅ Computer (Windows/Mac/Linux)
- ✅ Home Assistant with MQTT
- ✅ Internet connection

## Step 1: Install Arduino IDE (10 minutes)

1. **Download Arduino IDE**
   - Go to: https://www.arduino.cc/en/software
   - Download for your OS
   - Install and open Arduino IDE

2. **Add ESP8266 Board Support**
   - File → Preferences
   - In "Additional Board Manager URLs" add:
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Click OK
   - Tools → Board → Boards Manager
   - Search "esp8266"
   - Install "esp8266 by ESP8266 Community"

3. **Install MQTT Library**
   - Sketch → Include Library → Manage Libraries
   - Search "PubSubClient"
   - Install "PubSubClient by Nick O'Leary"

✅ Arduino IDE is ready!

## Step 2: Wire the Hardware (10 minutes)

### Simple Wiring Diagram
```
ESP8266 3.3V ----[10kΩ Resistor]----+---- ESP8266 A0
                                    |
                              [FSR Sensor]
                                    |
ESP8266 GND -------------------------+
```

### Step-by-Step
1. Connect 10kΩ resistor between 3.3V and A0
2. Connect FSR between A0 and GND
3. Connect USB cable to ESP8266
4. Plug into computer

✅ Hardware connected!

## Step 3: Configure the Code (5 minutes)

1. **Download the Code**
   - Download `bed_sensor.ino` from this repository
   - Open in Arduino IDE

2. **Update WiFi Settings**
   Find these lines and update:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";           // Your WiFi name
   const char* password = "YOUR_WIFI_PASSWORD";   // Your WiFi password
   ```

3. **Update MQTT Settings**
   Find these lines and update:
   ```cpp
   const char* mqtt_server = "192.168.1.XXX";     // Your HA IP address
   const char* mqtt_user = "YOUR_MQTT_USER";      // MQTT username
   const char* mqtt_password = "YOUR_MQTT_PASS";  // MQTT password
   ```

   **To find Home Assistant IP:**
   - Settings → System → Network
   - Look for IPv4 address

✅ Code configured!

## Step 4: Upload to ESP8266 (5 minutes)

1. **Select Board**
   - Tools → Board → ESP8266 Boards → NodeMCU 1.0

2. **Select Port**
   - Tools → Port → (select your COM port or /dev/ttyUSB0)

3. **Upload**
   - Click Upload button (→)
   - Wait for "Done uploading"

4. **Open Serial Monitor**
   - Tools → Serial Monitor
   - Set to 115200 baud
   - You should see:
     ```
     WiFi connected
     IP address: 192.168.1.XXX
     MQTT connected
     ```

✅ ESP8266 programmed!

## Step 5: Configure Home Assistant (10 minutes)

1. **Enable MQTT**
   - Settings → Devices & Services
   - Add Integration → MQTT
   - Configure your broker
   - Enable Discovery ✅

2. **Add Input Helpers**
   - Settings → Devices & Services → Helpers
   - Create Input Boolean: `aggressive_alarm_enabled`
   - Create Input DateTime: `alarm_time`
   - Create Input Boolean: `alarm_active`

3. **Add Automations**
   - Copy automations from `homeassistant/automations.yaml`
   - Settings → Automations & Scenes → Import
   - Or manually create in UI

4. **Verify Sensor**
   - Developer Tools → States
   - Look for `binary_sensor.bed_sensor`
   - Should show "on" or "off"

✅ Home Assistant configured!

## Step 6: Calibrate Sensor (10 minutes)

1. **Check Baseline**
   - Keep bed empty
   - Open Serial Monitor
   - Note the sensor value (e.g., "Sensor value: 120")

2. **Check Occupied Value**
   - Get in bed
   - Note the sensor value (e.g., "Sensor value: 680")

3. **Calculate Threshold**
   ```
   Threshold = (Empty + Occupied) / 2
   Example: (120 + 680) / 2 = 400
   ```

4. **Update Code**
   ```cpp
   const int OCCUPIED_THRESHOLD = 400;  // Your calculated value
   ```

5. **Re-upload**
   - Click Upload
   - Test getting in/out of bed
   - Verify "Bed: OCCUPIED" and "Bed: EMPTY" messages

✅ Sensor calibrated!

## Step 7: Test the System (5 minutes)

1. **Check Sensor in HA**
   - Go to `binary_sensor.bed_sensor` in HA
   - Get in bed → should show "Detected"
   - Get out → should show "Clear"

2. **Test Alarm Manually**
   - Turn on `aggressive_alarm_enabled`
   - Turn on `alarm_active`
   - Verify alarm sounds/lights flash

3. **Test Alarm Stop**
   - While alarm is active
   - Get out of bed
   - Wait 5 seconds
   - Alarm should stop

✅ System working!

## Step 8: Customize (Optional)

### Add Dashboard Card
```yaml
type: entities
title: Bed Sensor
entities:
  - binary_sensor.bed_sensor
  - input_boolean.aggressive_alarm_enabled
  - input_datetime.alarm_time
```

### Customize Alarm Actions
Edit automations to add your devices:
```yaml
action:
  - service: light.turn_on
    target:
      entity_id: light.bedroom_light
  - service: media_player.volume_set
    target:
      entity_id: media_player.bedroom_speaker
    data:
      volume_level: 0.8
```

## Troubleshooting Quick Fixes

### WiFi Won't Connect
- ✅ Use 2.4GHz network (not 5GHz)
- ✅ Check SSID and password (case-sensitive)
- ✅ Move ESP8266 closer to router

### MQTT Won't Connect
- ✅ Verify broker IP address
- ✅ Check username/password
- ✅ Ensure MQTT is running in HA

### Sensor Always Empty/Occupied
- ✅ Check wiring (FSR connected properly?)
- ✅ Recalibrate threshold
- ✅ Check Serial Monitor values

### Alarm Won't Stop
- ✅ Verify automation is enabled
- ✅ Check sensor shows "off" when out of bed
- ✅ Increase delay in automation (from 5s to 10s)

**Need more help?** See [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

## Next Steps

### Install Permanently
1. Mount ESP8266 in enclosure
2. Secure sensor under mattress with tape
3. Route cables neatly along bed frame
4. Label everything for future reference

### Add Features
- Set alarm for tomorrow morning
- Add more alarm devices (sirens, blinds)
- Create morning routine automations
- Add sleep tracking

### Optimize
- Adjust threshold for perfect detection
- Fine-tune debounce delay
- Customize alarm intensity
- Add conditional alarms (weekday only)

## Resources

- **Full Documentation:** [README.md](README.md)
- **Detailed Wiring:** [WIRING.md](WIRING.md)
- **Hardware Guide:** [HARDWARE.md](HARDWARE.md)
- **Calibration Help:** [CALIBRATION.md](CALIBRATION.md)
- **Troubleshooting:** [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
- **FAQ:** [FAQ.md](FAQ.md)

## Support

- 📖 Check documentation first
- 🐛 Report bugs via GitHub Issues
- 💬 Ask questions in GitHub Discussions
- 🤝 Contribute via Pull Requests

---

**Congratulations! Your HA-SleepGuard bed sensor is now active! 🎉**

You now have an aggressive alarm system that won't let you snooze in bed. Wake up time just got serious! ⏰
