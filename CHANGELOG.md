# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - 2024

### Added - Initial Release

#### ESP8266 Bed Sensor
- ESP8266 firmware for bed occupancy detection
- WiFi connectivity with configurable credentials
- MQTT integration with Home Assistant auto-discovery
- Support for Force Sensitive Resistor (FSR) sensors
- Configurable threshold for occupancy detection
- Debounce logic to prevent false triggers
- Serial debugging output for calibration
- Automatic reconnection for WiFi and MQTT

#### Home Assistant Integration
- MQTT binary sensor for bed occupancy
- Complete automation suite for aggressive alarms:
  - Trigger alarm at scheduled time
  - Continuous alarm while in bed
  - Automatic stop when out of bed
  - Manual emergency stop option
  - Escalating alarm intensity
- Input helpers for alarm control:
  - Aggressive alarm enable/disable
  - Alarm time setting
  - Alarm active status
- Dashboard examples with multiple card styles
- Support for TTS announcements
- Light automation examples
- Customizable alarm actions

#### Documentation
- Comprehensive README with full setup guide
- Quick Start Guide for fast deployment
- Hardware shopping list with cost estimates
- Detailed wiring diagrams for multiple sensor types
- Step-by-step calibration guide
- Extensive troubleshooting guide
- Frequently Asked Questions (FAQ)
- Contributing guidelines
- MIT License

#### Configuration Examples
- Example configuration file (config.h.example)
- Multiple sensor setup examples
- Dashboard card configurations
- Automation templates

#### Developer Resources
- Arduino library requirements
- Code comments and documentation
- Serial debugging capabilities
- Modular code structure for easy customization

### Features
- **Real-time bed occupancy detection**: Instant detection when someone enters or leaves the bed
- **MQTT auto-discovery**: Automatic sensor registration in Home Assistant
- **Aggressive alarms**: Alarms that require physical action to stop
- **Escalating intensity**: Alarms get progressively more aggressive
- **Customizable thresholds**: Adjustable sensitivity for different sensors
- **Debounce protection**: Prevents false triggers
- **WiFi auto-reconnect**: Automatic recovery from network issues
- **MQTT auto-reconnect**: Maintains connection reliability
- **Low power consumption**: Efficient ESP8266 operation
- **Local processing**: All data stays on local network

### Sensor Support
- Force Sensitive Resistor (FSR) - Primary support
- Load Cell with HX711 - Documentation and wiring diagrams
- Pressure mat/switch - Basic support with modifications
- Ultrasonic sensor - Mentioned in advanced documentation
- Extensible to other sensor types

### Home Assistant Features
- Binary sensor entity with occupancy device class
- MQTT availability tracking
- Persistent configuration via MQTT discovery
- Input Boolean helpers for alarm control
- Input DateTime helper for alarm scheduling
- Multiple automation examples
- Dashboard card templates
- Integration with lights, speakers, and other devices

### Documentation Highlights
- **README.md**: Complete project overview and setup
- **QUICKSTART.md**: Fast track to get running in under an hour
- **HARDWARE.md**: Shopping list with prices and recommendations
- **WIRING.md**: Detailed wiring diagrams for all sensor types
- **CALIBRATION.md**: Step-by-step sensor calibration process
- **TROUBLESHOOTING.md**: Comprehensive problem-solving guide
- **FAQ.md**: Common questions and answers
- **CONTRIBUTING.md**: Guidelines for contributors
- **LICENSE**: MIT License for open source use

### Hardware Compatibility
- **Microcontrollers**: 
  - NodeMCU V3 (ESP8266)
  - Wemos D1 Mini (ESP8266)
  - Generic ESP8266 boards
- **Sensors**:
  - Interlink FSR 402 and compatible
  - 50kg Load Cells with HX711
  - Generic pressure switches
- **Power**: USB 5V or external 5V power supply

### Software Requirements
- Arduino IDE 1.8.x or 2.x
- ESP8266 Board Package 3.x
- PubSubClient Library 2.8+
- Home Assistant 2021.x or newer
- Mosquitto MQTT Broker or HA built-in MQTT

### Network Requirements
- 2.4GHz WiFi network
- MQTT broker accessible on local network
- Home Assistant on same network
- Optional: Static IP for ESP8266

### Security
- Local network operation only
- No cloud dependencies
- MQTT authentication support
- Configurable credentials
- Data stays private on local network

### Limitations
- Single analog input (A0) on ESP8266
- WiFi 2.4GHz only (no 5GHz support)
- Requires calibration for each installation
- No battery operation (requires constant power)
- MQTT broker required

### Known Issues
None at initial release

### Future Enhancements (Planned)
- Deep sleep mode for power saving
- OTA (Over-The-Air) updates
- Multiple sensor support via external ADC
- Web interface for configuration
- Sleep quality tracking
- Integration with fitness trackers
- Alternative communication protocols (HTTP, WebSocket)
- ESP32 support

## Version History

### [1.0.0] - Initial Release
- Complete bed sensor implementation
- ESP8266 firmware
- Home Assistant integration
- Comprehensive documentation
- Example configurations
- Troubleshooting guides

---

## Upgrade Notes

### From Nothing to 1.0.0
This is the initial release. Follow the installation instructions in README.md or QUICKSTART.md to get started.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for information on how to contribute to this project.

## Support

- **Issues**: https://github.com/mikoa001/HA-SleepGuard/issues
- **Discussions**: https://github.com/mikoa001/HA-SleepGuard/discussions
- **Documentation**: See README.md and related guides
