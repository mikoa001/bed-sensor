# Wiring Diagrams

## Option 1: FSR (Force Sensitive Resistor) - RECOMMENDED FOR BEGINNERS

### Schematic
```
                         +3.3V (ESP8266)
                            |
                            |
                         [10kΩ]  <- Resistor
                            |
                            +-------- A0 (ESP8266)
                            |
                         [FSR]
                            |
                            |
                          GND (ESP8266)
```

### Pin Connections
```
ESP8266 NodeMCU          Component
----------------         ----------
3.3V (or 3V3)   -------- 10kΩ Resistor (one end)
                         10kΩ Resistor (other end) -------- FSR (one side)
A0              -------- Junction point (between resistor and FSR)
GND             -------- FSR (other side)
```

### Visual Layout
```
 ESP8266 NodeMCU
 ┌───────────────┐
 │    ┌─────┐   │
 │    │ USB │   │
 │    └─────┘   │
 │              │
 │ 3V3  ●       │ <-- Connect to 10kΩ resistor
 │ GND  ●       │ <-- Connect to FSR (one side)
 │ A0   ●       │ <-- Connect to junction point
 │              │
 └───────────────┘

 Breadboard Connection:
 
 10kΩ Resistor
     │
     └── Connected to 3.3V
     │
     ├── Connected to A0 (junction)
     │
 FSR (Force Sensor)
     │
     └── Connected to junction (with resistor)
     │
     └── Connected to GND
```

### Step-by-Step Wiring (FSR)
1. Insert 10kΩ resistor on breadboard
2. Connect one end of resistor to ESP8266 3.3V
3. Connect FSR to breadboard
4. Connect one side of FSR to the other end of resistor (junction point)
5. Connect ESP8266 A0 to junction point (where FSR and resistor meet)
6. Connect other side of FSR to ESP8266 GND

---

## Option 2: Load Cell with HX711

### Schematic
```
Load Cell               HX711 Module            ESP8266
---------              --------------          --------
Red    (E+) -------- E+ │          │
Black  (E-) -------- E- │          │
White  (A-) -------- A- │  HX711   │
Green  (A+) -------- A+ │          │
                        │ VCC ●    │ -------- 3.3V
                        │ GND ●    │ -------- GND
                        │ DT  ●    │ -------- D1 (GPIO5)
                        │ SCK ●    │ -------- D2 (GPIO4)
                        └──────────┘
```

### Pin Connections
```
HX711 Module             ESP8266 NodeMCU
------------             ---------------
VCC          --------    3.3V (or VIN for 5V version)
GND          --------    GND
DT (Data)    --------    D1 (GPIO5)
SCK (Clock)  --------    D2 (GPIO4)

Load Cell    HX711 Module
---------    ------------
Red wire     E+
Black wire   E-
White wire   A-
Green wire   A+
```

### Visual Layout
```
 Load Cell Wiring:
 ┌─────────────────┐
 │   Load Cell     │
 │                 │
 │  Red  ●●●●●●●   │ <-- E+ (Excitation +)
 │  Blk  ●●●●●●●   │ <-- E- (Excitation -)
 │  Wht  ●●●●●●●   │ <-- A- (Amplifier -)
 │  Grn  ●●●●●●●   │ <-- A+ (Amplifier +)
 └─────────────────┘

 HX711 Module:
 ┌──────────┐
 │  HX711   │
 │ ┌──────┐ │
 │ │      │ │
 │ E+ E- A- A+
 │ │
 │ VCC GND DT SCK
 └──┬───┬──┬──┬──┘
    │   │  │  │
    To ESP8266:
    3V  G  D1 D2
```

### Step-by-Step Wiring (Load Cell)
1. Connect Load Cell wires to HX711:
   - Red to E+
   - Black to E-
   - White to A-
   - Green to A+
2. Connect HX711 to ESP8266:
   - HX711 VCC to ESP8266 3.3V
   - HX711 GND to ESP8266 GND
   - HX711 DT to ESP8266 D1 (GPIO5)
   - HX711 SCK to ESP8266 D2 (GPIO4)

---

## Option 3: Simple Pressure Switch

### Schematic
```
                         +3.3V (ESP8266)
                            |
                         [10kΩ]  <- Pull-up resistor
                            |
                            +-------- D1 (ESP8266)
                            |
                      [Pressure Switch]
                            |
                          GND (ESP8266)
```

### Pin Connections
```
ESP8266 NodeMCU          Component
----------------         ----------
3.3V            -------- 10kΩ Resistor
D1 (GPIO5)      -------- 10kΩ Resistor & Pressure Switch (junction)
GND             -------- Pressure Switch
```

---

## Power Supply Wiring

### USB Power (Recommended)
```
USB Charger (5V) --> Micro USB Cable --> ESP8266 USB Port
```

### External 5V Power (Alternative)
```
5V Power Supply
    │
    ├── VIN (or 5V pin) on ESP8266
    │
    └── GND on ESP8266
```

**Warning:** Do NOT connect both USB and external power simultaneously!

---

## Complete System Diagram (FSR Option)

```
Wall Outlet
    │
    ▼
[USB Charger 5V]
    │
    ▼
[USB Cable] ──────> [ESP8266 NodeMCU]
                         │
                         │ 3.3V ───> [10kΩ Resistor] ───┐
                         │                               │
                         │ A0 <──────────────────────────┤
                         │                               │
                         │ GND <───────── [FSR Sensor] <─┘
                         │
                         │ (WiFi) ~~~~~> Router ~~~~> [Home Assistant]
                         
Sensor Placement:
[Mattress]
─────────────────
   [FSR Sensor]  <- Under mattress
─────────────────
[Bed Slats/Base]
```

---

## Physical Installation

### FSR Sensor Placement
```
Side View of Bed:

     Person
      ┌─┴─┐
      │   │
┌─────┴───┴──────┐
│   Mattress      │
├─────────────────┤ <- Place FSR here (taped to slat)
│   Bed Slats     │
└─────────────────┘

Best Positions:
1. Center of bed, 1/3 from head
2. Where torso weight concentrates
3. Between mattress and slats
```

### Load Cell Placement
```
Side View of Bed:

     Person
      ┌─┴─┐
      │   │
┌─────┴───┴──────┐
│   Mattress      │
├─────────────────┤
│   Bed Frame     │
│         │       │
│      [Load Cell] <- Under one bed leg
│         │       │
└─────────┴───────┘
      Floor
```

---

## Cable Management

### Keeping Wires Tidy
```
[ESP8266] --- (Power Cable) --> Wall
    │
    │ (Sensor Cable - keep short!)
    ▼
[Sensor under bed]

Tips:
- Use cable clips to route wires along bed frame
- Keep sensor cable < 1 meter for stability
- Use cable ties to bundle wires
- Route away from mattress pressure points
```

---

## Testing Points

### Multimeter Testing (FSR)
1. **Measure FSR Resistance:**
   - No pressure: >10kΩ (very high)
   - With pressure: 1-5kΩ (lower)

2. **Measure Voltage at A0:**
   - No pressure: ~3.0V
   - With pressure: ~0.5-1.5V

### Multimeter Testing (Load Cell)
1. **Check HX711 Power:**
   - VCC to GND: Should be 3.3V
   
2. **Load Cell Resistance:**
   - E+ to E-: ~400-1000Ω
   - A+ to A-: ~350-400Ω

---

## Common Wiring Mistakes

### ❌ Wrong Connections
- Connecting FSR directly without resistor
- Using A0 as output (it's input only!)
- Connecting 5V to 3.3V pins
- Reversing load cell wires

### ✅ Correct Practice
- Always use voltage divider for FSR
- A0 is analog input only
- Check voltage ratings
- Follow load cell color code

---

## Advanced: Multiple Sensors

### Dual Bed Sensor (Two FSRs)
```
ESP8266
  │
  ├─ A0 ─── [10kΩ + FSR] ─── GND (Sensor 1)
  │
  └─ Need another ESP8266 or external ADC!

Note: ESP8266 only has ONE analog pin (A0)
For multiple sensors, either:
- Use multiple ESP8266 boards
- Use external ADC (ADS1115)
- Use load cells under multiple legs
```

---

## Safety Notes

⚠️ **Important:**
- Use only 3.3V or 5V power supplies
- Don't exceed voltage ratings
- Insulate exposed connections
- Keep electronics away from moisture
- Use proper gauge wires
- Double-check polarity before powering on

---

## Tools Needed for Wiring

### Essential:
- Wire strippers (if using solid wire)
- Small screwdriver (for terminal blocks)

### Optional:
- Multimeter (testing connections)
- Soldering iron (permanent connections)
- Heat shrink tubing (insulation)
- Label maker (identifying wires)

---

**Tip:** Take photos of your wiring before closing any enclosure for future reference!
