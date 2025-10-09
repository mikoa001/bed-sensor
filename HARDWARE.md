# Hardware Shopping List

## Essential Components

### 1. Microcontroller
**ESP8266 Development Board** (Choose one):
- **NodeMCU V3** - $3-5 USD
  - Recommended for beginners
  - Built-in USB, easy to program
  - [Amazon/AliExpress/eBay]
  
- **Wemos D1 Mini** - $2-4 USD
  - Compact size
  - Good for permanent installations
  - [Amazon/AliExpress/eBay]

### 2. Pressure/Weight Sensor (Choose one option)

#### Option A: Force Sensitive Resistor (FSR) - EASIEST
- **Interlink FSR 402 or similar** - $5-10 USD
  - Circular or strip type
  - Measures pressure/force
  - Simple to connect
  - Good for most applications
  - [Adafruit, SparkFun, Amazon]

- **Additional for FSR:**
  - 10kΩ resistor - $0.10 USD
  - For voltage divider circuit

#### Option B: Load Cell with HX711 - MORE ACCURATE
- **50kg Load Cell** - $5-8 USD
  - More accurate weight measurement
  - Better long-term stability
  - [Amazon, AliExpress]

- **HX711 Amplifier Module** - $1-2 USD
  - Required for load cell
  - [Amazon, AliExpress]

#### Option C: Pressure Mat/Switch - SIMPLEST
- **Pressure Mat Switch** - $10-20 USD
  - Simple on/off detection
  - No calibration needed
  - May have lower reliability
  - [Amazon, specialized suppliers]

### 3. Power Supply
**Micro USB Cable** - $2-5 USD
- For NodeMCU or Wemos D1 Mini
- 1-2 meter length recommended
- [Any electronics store]

**USB Power Adapter (5V, 1A minimum)** - $3-8 USD
- Old phone charger works
- [Any electronics store]

### 4. Connecting Materials
**Jumper Wires (Male-to-Female)** - $2-5 USD
- Pack of 40 wires
- For prototyping and connections
- [Amazon, AliExpress, local electronics]

**Breadboard (Optional for testing)** - $3-5 USD
- Half-size breadboard
- Useful for prototyping
- [Amazon, AliExpress]

### 5. Enclosure (Optional but Recommended)
**Project Box** - $3-8 USD
- Plastic enclosure for ESP8266
- Protects electronics
- Various sizes available
- [Amazon, AliExpress]

## Optional Components

### For Enhanced Functionality
- **Physical Reset Button** - $1-2 USD
  - Emergency alarm stop
  - [Electronics store]

- **Status LED** - $0.50 USD
  - Visual indicator
  - 220Ω resistor for LED
  - [Electronics store]

- **Buzzer Module** - $1-2 USD
  - Local alarm sound
  - [Amazon, AliExpress]

### For Professional Installation
- **Heat Shrink Tubing** - $5-10 USD (kit)
  - For wire insulation
  - [Amazon, hardware store]

- **Wire Connectors** - $5-10 USD
  - Terminal blocks or Wago connectors
  - [Hardware store]

- **Double-sided Tape** - $3-5 USD
  - For mounting sensor under mattress
  - [Any store]

## Total Cost Estimates

### Budget Build (FSR Sensor)
- ESP8266 NodeMCU: $4
- FSR Sensor: $7
- 10kΩ Resistor: $0.10
- USB Cable: $2
- USB Adapter: $3
- Jumper Wires: $3
- **Total: ~$20 USD**

### Accurate Build (Load Cell)
- ESP8266 NodeMCU: $4
- Load Cell: $6
- HX711 Module: $2
- USB Cable: $2
- USB Adapter: $3
- Jumper Wires: $3
- **Total: ~$20 USD**

### Complete Kit (with extras)
- ESP8266 NodeMCU: $4
- FSR Sensor: $7
- Resistor: $0.10
- USB Cable + Adapter: $5
- Jumper Wires: $3
- Breadboard: $4
- Enclosure: $5
- **Total: ~$28 USD**

## Where to Buy

### Online Retailers
1. **Amazon** - Fast shipping, good for US/EU
2. **AliExpress** - Cheapest, slow shipping (2-4 weeks)
3. **eBay** - Middle ground pricing and shipping
4. **Adafruit** - Quality components, US-based
5. **SparkFun** - Quality components, US-based
6. **Local Electronics Store** - Immediate availability

### Recommended Starter Kits
Some sellers offer ESP8266 starter kits with:
- ESP8266 board
- Breadboard
- Jumper wires
- Various sensors
- Usually $15-25 USD
- Good value for beginners

## Tools Required

### Essential
- **Computer with USB port** - For programming
- **Internet connection** - For downloading Arduino IDE

### Helpful
- **Multimeter** - $10-20 USD (for troubleshooting)
- **Soldering Iron** - $15-30 USD (for permanent installation)
- **Wire Strippers** - $5-10 USD (for permanent installation)

## Sensor Selection Guide

### Choose FSR if:
- You want simple setup
- Budget is tight
- First time with Arduino/ESP8266
- Don't need extreme accuracy

### Choose Load Cell if:
- You want accurate weight measurement
- Plan to expand functionality later
- Want professional-grade reliability
- Comfortable with slightly more complex wiring

### Choose Pressure Mat if:
- Want absolute simplest setup
- Don't need customization
- Okay with digital (on/off) detection only

## Alternative Sensors (Advanced)

### Other Sensor Options:
1. **Ultrasonic Sensor (HC-SR04)** - $2-3
   - Measures distance to mattress
   - Non-contact detection

2. **PIR Motion Sensor** - $2-3
   - Detects movement in bed
   - Less reliable for occupancy

3. **Capacitive Touch Sensor** - $3-5
   - Detects presence through touch
   - Can work through fabric

4. **Time-of-Flight Sensor (VL53L0X)** - $5-8
   - Precise distance measurement
   - More expensive

## Quality Tips

1. **Buy from reputable sellers** - Check reviews
2. **ESP8266 boards vary** - Some cheap ones have issues
3. **Load cells** - Make sure HX711 is included or bought separately
4. **FSR sensors** - Interlink brand is most reliable
5. **Power supplies** - Use quality chargers to avoid issues

## Bulk Discounts

If building multiple sensors:
- ESP8266 boards: 5-pack often 30% cheaper
- FSR sensors: Buy 5+ for discount
- Consider group buy with friends

## Return Policy

When buying:
- Check return policy
- Test components upon arrival
- Keep packaging for potential returns

---

**Note:** Prices are approximate and vary by region and seller. Always compare prices across platforms. Watch for sales on Amazon Prime Day, Black Friday, or AliExpress anniversary sales for significant discounts.
