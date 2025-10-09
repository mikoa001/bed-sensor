# Home Assistant - Sleep Guard

An advanced bed occupancy sensor system for Home Assistant that creates aggressive alarms which won't stop until you physically get out of bed!

## Overview

HA-SleepGuard uses an ESP8266 microcontroller with a pressure/weight sensor to detect bed occupancy. This data is sent to Home Assistant via MQTT, enabling sophisticated automations including aggressive alarms that require you to physically stand up from the bed to turn them off.

**Beyond the Bedroom:** While designed for bed occupancy detection, this system is easily adaptable to monitor other furniture and areas - detect when someone sits on a sofa, steps on a rug, uses a doormat, or any other pressure-sensitive application you can imagine!

## Features

- **Real-time bed occupancy detection** using pressure/weight sensors
- **MQTT integration** with Home Assistant auto-discovery
- **Aggressive alarm system** that continues until you get out of bed
- **Escalating intensity** - alarms get louder/more aggressive over time
- **Physical presence requirement** - no snoozing from bed!
- **Customizable automations** for different alarm behaviors

## Hardware Requirements

### Components Needed

1. **ESP8266 Development Board** (NodeMCU, Wemos D1 Mini, etc.)
2. **Pressure/Weight Sensor** (Options):
   - FSR (Force Sensitive Resistor) - affordable and simple
   - Load Cell with HX711 amplifier - more accurate
   - Pressure mat/switch
3. **Power Supply** (USB or 5V adapter)
4. **Resistor** (10kΩ for FSR voltage divider, if using FSR)
5. **Wires and breadboard** (for prototyping)

### Wiring Diagram

#### For FSR (Force Sensitive Resistor):
```
ESP8266          FSR Sensor
--------         ----------
A0 ------------- FSR (one side)
                 FSR (other side) --- GND
                                 |
                                 +--- 10kΩ resistor --- 3.3V
```

#### For Load Cell with HX711:
```
ESP8266          HX711
--------         -----
D1 (GPIO5) ----- DT
D2 (GPIO4) ----- SCK
3.3V ----------- VCC
GND ------------ GND
```

## Software Requirements

- **Arduino IDE** with ESP8266 board support
- **Home Assistant** with MQTT integration
- **MQTT Broker** (Mosquitto recommended)

### Required Arduino Libraries

- ESP8266WiFi (included with ESP8266 board package)
- PubSubClient (for MQTT communication)

Install via Arduino IDE Library Manager:
1. Go to Sketch → Include Library → Manage Libraries
2. Search for "PubSubClient" by Nick O'Leary
3. Click Install

## Installation

### 1. ESP8266 Setup

1. **Install Arduino IDE and ESP8266 Support**:
   - Install Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)
   - Add ESP8266 board support: File → Preferences → Additional Board Manager URLs
   - Add: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Go to Tools → Board → Boards Manager, search "esp8266" and install

2. **Configure the Code**:
   - Open `esp8266_bed_sensor/bed_sensor.ino` in Arduino IDE
   - Update WiFi credentials:
     ```cpp
     const char* ssid = "YOUR_WIFI_SSID";
     const char* password = "YOUR_WIFI_PASSWORD";
     ```
   - Update MQTT broker settings:
     ```cpp
     const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
     const char* mqtt_user = "YOUR_MQTT_USER";
     const char* mqtt_password = "YOUR_MQTT_PASSWORD";
     ```
   - Adjust sensor threshold based on your sensor:
     ```cpp
     const int OCCUPIED_THRESHOLD = 512;  // Calibrate this value
     ```

3. **Upload to ESP8266**:
   - Connect ESP8266 via USB
   - Select your board: Tools → Board → ESP8266 Boards → (your board)
   - Select correct port: Tools → Port
   - Click Upload

### 2. Home Assistant Setup

1. **Enable MQTT Integration**:
   - Go to Settings → Devices & Services → Add Integration
   - Search for MQTT and configure your broker

2. **Add Configuration**:
   - Copy contents from `homeassistant/configuration.yaml` to your HA configuration
   - Copy contents from `homeassistant/automations.yaml` to your automations file
   - Restart Home Assistant

3. **Customize Entity IDs**:
   - Update entity IDs in automations to match your devices:
     - `media_player.bedroom_speaker` → your speaker
     - `light.bedroom_light` → your lights
     - Add your specific devices (sirens, smart plugs, etc.)

### 3. Sensor Calibration

1. **Monitor Serial Output**:
   - Open Serial Monitor in Arduino IDE (Tools → Serial Monitor)
   - Set baud rate to 115200
   - Observe sensor values when bed is empty vs occupied

2. **Adjust Threshold**:
   - Note the value when bed is empty
   - Note the value when someone is in bed
   - Set `OCCUPIED_THRESHOLD` to a value between these two
   - Reupload the sketch

## Usage

### Basic Operation

1. The sensor automatically detects when someone is in bed
2. Status is published to Home Assistant via MQTT
3. The binary sensor `binary_sensor.bed_sensor` shows:
   - `ON` when bed is occupied
   - `OFF` when bed is empty

### Setting Up Alarms

1. **Enable Aggressive Alarm**:
   - Toggle `input_boolean.aggressive_alarm_enabled` to ON

2. **Set Alarm Time**:
   - Set your desired wake time in `input_datetime.alarm_time`

3. **Alarm Behavior**:
   - At alarm time, if aggressive alarm is enabled, it activates
   - Alarm sounds continuously while you're still in bed
   - After 2 minutes, alarm intensity escalates
   - Alarm only stops when you physically get out of bed (sensor detects empty bed)
   Notice: The threshold between detecting and not detecting should be low, meaning it should be much easier to get a false detection of bed occupiency than a true detection to prioritize turning off the bed-state and therefore the attached alarm-state.

### Customizing Alarms

Edit `homeassistant/automations.yaml` to customize:

- **Change alarm sounds**: Modify TTS messages or use media files
- **Add more devices**: Include smart plugs, vibration devices, etc.
- **Adjust timing**: Change escalation time, check intervals
- **Add conditions**: Skip alarm on weekends, holidays, etc.

Example additions:
```yaml
# Add siren
- service: switch.turn_on
  target:
    entity_id: switch.bedroom_siren

# Add smart blinds
- service: cover.open_cover
  target:
    entity_id: cover.bedroom_blinds

# Add vibration device
- service: switch.turn_on
  target:
    entity_id: switch.vibration_alarm
```

## Advanced Configuration

### Multiple Bed Sensors

To monitor multiple beds, duplicate the ESP8266 code with different:
- `mqtt_client_id`
- Topic names (e.g., `bed_sensor_bedroom1`, `bed_sensor_bedroom2`)
- Unique IDs in discovery config

### Conditional Alarms

Add conditions to skip alarms on certain days:
```yaml
condition:
  - condition: state
    entity_id: input_boolean.aggressive_alarm_enabled
    state: 'on'
  - condition: time
    weekday:
      - mon
      - tue
      - wed
      - thu
      - fri
```

### Smart Escalation

Create multiple escalation levels:
1. Gentle alarm (sound only) - first 2 minutes
2. Medium alarm (sound + lights) - 2-5 minutes
3. Aggressive alarm (sound + lights + all devices) - 5+ minutes
4. Maximum alarm (everything at full intensity) - 10+ minutes

## Beyond the Bedroom: Other Applications

While HA-SleepGuard was designed for bed occupancy detection, the underlying technology is highly versatile and can be adapted for many other pressure-sensing applications:

### Living Room & Furniture
- **Sofa/Couch Detection**: Know when someone is sitting on the couch to trigger "movie mode" lighting, pause cleaning robots, or track family room usage
- **Recliner Occupancy**: Automatically adjust room temperature or lighting when your favorite chair is occupied
- **Office Chair**: Track desk time, remind you to take breaks, or turn on task lighting when you sit down

### Entryways & Safety
- **Doormat/Welcome Mat**: Get notified when someone arrives at your door, trigger entrance lighting, or log visitor activity
- **Floor Mat Sensors**: Detect when someone steps into specific areas (nursery, elderly care monitoring, security zones)
- **Bathroom Mat**: Turn on night lights automatically, monitor elderly family members, or track bathroom usage

### Creative Applications
- **Pet Bed Monitoring**: Know when your pet is sleeping or resting
- **Workshop Stool**: Automatically enable tools and lighting when you sit at your workbench
- **Porch/Patio Furniture**: Trigger outdoor lighting or music when someone sits outside
- **Gaming Chair**: Log gaming sessions or switch scenes when you're ready to play

### Implementation Tips for Other Applications
The same ESP8266 code and Home Assistant setup can be reused with minimal changes:

1. **Adjust Sensor Type**: Choose the appropriate sensor for your application (FSR for light pressure, load cell for precise weight)
2. **Modify Topic Names**: Update MQTT topics to reflect the application (e.g., `sofa_sensor`, `doormat_sensor`)
3. **Calibrate Threshold**: Each application will have different pressure requirements
4. **Create Custom Automations**: Replace alarm automations with actions relevant to your use case

**Example**: Converting to a Sofa Sensor
```cpp
// In ESP8266 code, update these lines:
const char* mqtt_topic = "homeassistant/binary_sensor/sofa_sensor/state";
const char* mqtt_discovery_topic = "homeassistant/binary_sensor/sofa_sensor/config";
const char* device_name = "Sofa Occupancy Sensor";

// In Home Assistant, create automations like:
// - Turn on ambient lighting when someone sits down
// - Pause robot vacuum when couch is occupied
// - Start "movie mode" scene after 5 minutes of occupancy
```

The possibilities are endless - anywhere you need to detect presence or pressure, this system can be adapted to fit your needs!

## Troubleshooting

### Sensor Not Connecting

- Check WiFi credentials in code
- Verify MQTT broker IP address and credentials
- Check Serial Monitor for connection errors
- Ensure MQTT broker is running and accessible

### False Triggers

- Increase `DEBOUNCE_DELAY` to avoid false triggers
- Calibrate `OCCUPIED_THRESHOLD` more carefully
- Check sensor placement - should be under mattress, not on top

### Sensor Not Detecting Occupancy

- Check sensor wiring
- Monitor analog readings in Serial Monitor
- Adjust threshold value
- Ensure sensor is positioned correctly

### Home Assistant Not Receiving Updates

- Verify MQTT integration is working in HA
- Check MQTT discovery is enabled
- Verify topic names match in both ESP8266 and HA
- Check MQTT broker logs for messages

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## License

This project is open source. Feel free to use and modify as needed.

## Acknowledgments

- Built for Home Assistant integration
- Uses MQTT for reliable communication
- Designed for ESP8266 platform

## Safety Notice

⚠️ **Important**: Aggressive alarms can be stressful. Use responsibly and ensure you can manually disable the alarm if needed. Always have a backup method to disable the alarm (e.g., physical switch, HA dashboard access from phone).
