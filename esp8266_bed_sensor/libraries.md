# Required Libraries for ESP8266 Bed Sensor

# ESP8266 Board Package
# Install via Arduino IDE Board Manager:
# - Go to File → Preferences
# - Add to Additional Board Manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json
# - Go to Tools → Board → Boards Manager
# - Search for "esp8266" and install "esp8266 by ESP8266 Community"

# Required Arduino Libraries
# Install via Arduino IDE Library Manager (Sketch → Include Library → Manage Libraries):

## 1. PubSubClient
- Name: PubSubClient
- Author: Nick O'Leary
- Version: 2.8.0 or later
- Description: MQTT client library for Arduino
- Installation: Search "PubSubClient" in Library Manager and click Install

## 2. ESP8266WiFi
- Included with ESP8266 board package
- No separate installation required

# Optional Libraries (for advanced sensor types)

## For Load Cell with HX711:
- Name: HX711 Arduino Library
- Author: Bogdan Necula
- Installation: Search "HX711" in Library Manager
- Use if you're using a load cell instead of FSR

## For Alternative Sensors:
- Adafruit_ADS1X15 (for external ADC if needed)
- OneButton (for physical button integration)
