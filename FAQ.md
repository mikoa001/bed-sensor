# Frequently Asked Questions (FAQ)

## General Questions

### What is HA-SleepGuard?
HA-SleepGuard is a bed occupancy sensor system that integrates with Home Assistant. It can detect when someone is in bed and trigger aggressive alarms that won't stop until you physically get out of bed.

### Do I need programming experience?
Basic familiarity with Arduino and Home Assistant is helpful, but not required. The documentation provides step-by-step instructions suitable for beginners.

### How much does it cost to build?
Approximately $20-30 USD for basic components:
- ESP8266: $3-5
- Sensor: $5-10
- Cables and accessories: $5-10

### How long does it take to build?
- Hardware assembly: 30-60 minutes
- Software setup: 30-45 minutes
- Calibration and testing: 15-30 minutes
- Total: 1.5-2.5 hours

## Hardware Questions

### Which ESP8266 board should I buy?
**Recommended:**
- **NodeMCU V3** - Best for beginners (built-in USB, easy to use)
- **Wemos D1 Mini** - Best for compact installations

Both work equally well. Choose based on your preference and availability.

### Which sensor type is best?

**Force Sensitive Resistor (FSR):**
- ✅ Easy to set up
- ✅ Affordable ($5-10)
- ✅ Good for beginners
- ❌ Less accurate than load cell
- **Best for:** Most users

**Load Cell:**
- ✅ Very accurate
- ✅ Measures actual weight
- ✅ Long-term stability
- ❌ Slightly more complex setup
- **Best for:** Advanced users wanting precision

**Recommendation:** Start with FSR, upgrade to load cell if needed.

### Can I use this with any bed?
Yes! The sensor works with:
- Regular beds
- Bunk beds (need one sensor per bunk)
- Adjustable beds
- Murphy beds
- Daybeds

Placement may vary, but the concept remains the same.

### Where exactly should I place the sensor?
**FSR Sensor:**
- Under the mattress, on top of bed slats
- Where torso weight concentrates (1/3 from headboard)
- Centered width-wise
- Secure with double-sided tape

**Load Cell:**
- Under one bed leg
- On a stable, level surface
- For best accuracy, use 4 load cells (one per leg)

### Will this work with a memory foam mattress?
Yes, but may require adjustment:
- FSR: Still works, may need to increase threshold
- Load Cell: Works perfectly, measures actual weight
- Thicker mattresses may dampen FSR response

## Software Questions

### What versions are required?
- **Arduino IDE:** 1.8.x or 2.x
- **Home Assistant:** 2021.x or newer (any recent version)
- **MQTT Broker:** Mosquitto 1.6+ (or HA built-in MQTT)

### Do I need a separate MQTT broker?
No, Home Assistant has a built-in MQTT broker (Mosquitto add-on). You can use:
- HA Mosquitto add-on (easiest)
- Separate Mosquitto server
- Other MQTT brokers (HiveMQ, etc.)

### Can this work without Home Assistant?
Not with the current implementation. The system is designed for Home Assistant integration. However, you could modify the code to:
- Use standalone MQTT
- Add a web interface
- Trigger local alarms

### Does this work with HomeKit/Alexa/Google Home?
Yes, through Home Assistant:
- **HomeKit:** Use Home Assistant HomeKit integration
- **Alexa:** Use Home Assistant Alexa integration  
- **Google Home:** Use Home Assistant Google Assistant integration

The bed sensor becomes available to these platforms through HA.

## Setup and Configuration

### I'm not seeing the sensor in Home Assistant
**Check these:**
1. MQTT integration enabled in HA
2. MQTT discovery enabled
3. ESP8266 connected to MQTT (check Serial Monitor)
4. Correct MQTT topics in code
5. Restart Home Assistant

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for detailed steps.

### How do I calibrate the sensor?
1. Open Serial Monitor (115200 baud)
2. Note value when bed is empty (e.g., 100)
3. Get in bed, note value (e.g., 700)
4. Set threshold halfway: `(100 + 700) / 2 = 400`
5. Update code: `const int OCCUPIED_THRESHOLD = 400;`
6. Re-upload and test

See [CALIBRATION.md](CALIBRATION.md) for complete guide.

### The sensor keeps triggering randomly
**Solutions:**
1. Increase debounce delay: `DEBOUNCE_DELAY = 5000` (5 seconds)
2. Recalibrate threshold
3. Check for loose wiring
4. Use better power supply (quality USB charger)
5. Add sensor value averaging (see Troubleshooting)

### Can I use WiFi with a hidden SSID?
Yes, modify the WiFi connection:
```cpp
WiFi.begin(ssid, password, 0, NULL, true);  // Last parameter enables hidden SSID
```

## Alarm Questions

### How does the aggressive alarm work?
1. At alarm time, if enabled, alarm activates
2. Alarm sounds/lights flash while you're in bed
3. After 2 minutes, alarm escalates (louder, more devices)
4. Alarm ONLY stops when sensor detects you've left the bed
5. No snoozing from bed!

### Can I snooze the alarm?
No, that's the point! The alarm won't stop until you physically get out of bed. However, there is an emergency manual stop option via Home Assistant.

### What devices can be used for alarms?
Any Home Assistant-compatible device:
- **Speakers:** TTS announcements, music, sounds
- **Lights:** Flash, turn on at full brightness
- **Smart plugs:** Trigger any connected device
- **Sirens:** Loud audio alerts
- **Vibration pads:** Physical shaking alerts
- **Smart blinds:** Open to let sunlight in

### Can I set different alarms for weekdays/weekends?
Yes! Add conditions to your automations:
```yaml
condition:
  - condition: time
    weekday:
      - mon
      - tue
      - wed
      - thu
      - fri
```

### What if I need to disable the alarm in an emergency?
Two options:
1. **Manual stop:** Toggle `input_boolean.alarm_active` off in HA dashboard
2. **Disable alarm:** Toggle `input_boolean.aggressive_alarm_enabled` off
3. **Physical:** Unplug the sensor (last resort)

### Can I have different alarm intensities?
Yes! Create multiple automations with different time-based conditions:
- 0-2 min: Gentle (quiet TTS)
- 2-5 min: Medium (louder TTS + lights)
- 5-10 min: Aggressive (sirens + all lights + blinds)
- 10+ min: Maximum (everything at full intensity)

## Technical Questions

### Can I monitor multiple beds?
Yes! For each additional bed:
1. Use separate ESP8266 and sensor
2. Change unique IDs in code
3. Change MQTT topics (e.g., `bed_sensor_bedroom2`)
4. Create separate automations for each

### How reliable is the sensor?
When properly calibrated:
- FSR: 95-98% accuracy
- Load Cell: 99%+ accuracy
- False triggers: <1% with proper debouncing

### What's the power consumption?
- ESP8266: ~80mA when active (WiFi on)
- Deep sleep mode: ~15μA (can be added)
- Annual cost: <$2 USD (assuming $0.12/kWh)

### Can I make it battery-powered?
Possible but not recommended:
- ESP8266 draws too much power for long-term battery use
- Would need frequent recharging (daily/weekly)
- Better: Use power-efficient ESP32 with deep sleep
- Or: Use mains power

### Does it work with 5GHz WiFi?
No, ESP8266 only supports 2.4GHz WiFi networks. Make sure you have a 2.4GHz network available.

### What's the WiFi range?
Typical ESP8266 range: 30-50 meters indoors (100-150 feet)
Factors affecting range:
- Walls and obstacles
- WiFi router strength
- Interference from other devices

Use WiFi extender if needed.

## Privacy and Security

### Is my bed occupancy data private?
Yes, all data stays on your local network:
- MQTT messages are local (no cloud)
- Home Assistant processes data locally
- No external services required
- No data sent to internet (unless you configure it)

### Can I use this with SSL/TLS?
Yes, modify MQTT connection to use secure connection:
- Configure MQTT broker for TLS
- Update ESP8266 code to use WiFiClientSecure
- Add certificate verification

### What happens if internet goes down?
Everything still works! The system operates entirely on your local network:
- ESP8266 → MQTT → Home Assistant (all local)
- No internet required for basic functionality
- Only remote access (HA app) requires internet

## Troubleshooting

### ESP8266 won't connect to WiFi
1. Verify 2.4GHz network (not 5GHz)
2. Check WiFi credentials (case-sensitive)
3. Move closer to router
4. Check router settings (guest network restrictions?)

### Sensor readings are unstable
1. Check wiring connections
2. Use quality power supply
3. Increase debounce delay
4. Add sensor averaging (see code examples)

### Alarm won't trigger
1. Check automation is enabled
2. Verify alarm time is set
3. Check `aggressive_alarm_enabled` is ON
4. Test manually (Developer Tools → Services)

### More issues?
See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for comprehensive troubleshooting guide.

## Advanced Topics

### Can I add more features?
Yes! Ideas:
- Sleep quality tracking (time in bed)
- Wake-up light (gradual brightness)
- Sleep schedule optimization
- Integration with fitness trackers
- Automatic heating/cooling based on presence

### Can I use OTA updates?
Yes, add OTA (Over-The-Air) update capability:
- Include ArduinoOTA library
- Enable OTA in code
- Update ESP8266 wirelessly

### Can I add a display?
Yes, add OLED display to show:
- Sensor status
- WiFi/MQTT status
- Current state (occupied/empty)
- IP address

Popular options: SSD1306 OLED (I2C)

### Can I integrate with sleep tracking apps?
Possible approaches:
- Export HA data to CSV
- Use HA REST API
- MQTT integration with other platforms
- Create Node-RED flows

## Getting Help

### Where can I get support?
1. **Documentation:** README, guides, wiki
2. **Troubleshooting:** [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
3. **GitHub Issues:** Report bugs, request features
4. **GitHub Discussions:** Ask questions, share ideas
5. **Home Assistant Community:** Forums and Discord

### How can I contribute?
See [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Code contributions
- Documentation improvements
- Hardware testing
- Feature suggestions

### Found a bug?
Please report:
1. Describe the issue
2. Steps to reproduce
3. Expected vs actual behavior
4. Hardware/software versions
5. Serial Monitor output
6. Photos (if hardware-related)

---

**Still have questions?** 
- Open a [GitHub Discussion](https://github.com/mikoa001/HA-SleepGuard/discussions)
- Check existing [Issues](https://github.com/mikoa001/HA-SleepGuard/issues)
- Read the full [Documentation](README.md)
