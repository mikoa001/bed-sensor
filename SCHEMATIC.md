# Visual Schematic - FSR Bed Sensor

## Complete System Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                    HA-SleepGuard System                         │
└─────────────────────────────────────────────────────────────────┘

        PHYSICAL SETUP                    SOFTWARE FLOW
        
    [Person in Bed]                   ┌─────────────────┐
          │                           │  Home Assistant │
          ▼                           │                 │
    ┌──────────┐                      │  - Automations  │
    │ Mattress │                      │  - Dashboard    │
    └──────────┘                      │  - Alarms       │
    ┌──────────┐                      └────────┬────────┘
    │   FSR    │◄─── Pressure                  │
    │  Sensor  │     Detection                 │ MQTT
    └────┬─────┘                               │
         │                                     │
    ┌────▼─────────┐                    ┌──────▼──────┐
    │   ESP8266    │◄───── WiFi ───────►│ MQTT Broker │
    │   NodeMCU    │                    │ (Mosquitto) │
    └──────────────┘                    └─────────────┘
         │
         ▼
    [USB Power]
```

## Detailed FSR Wiring Schematic

```
┌─────────────────────────────────────────────────────────────┐
│                ESP8266 NodeMCU Board                        │
│                                                             │
│                    ┌───────┐                                │
│                    │  USB  │  ◄── Power Input               │
│                    └───────┘                                │
│                                                             │
│  PIN              CONNECTION                                │
│  ───              ──────────                                │
│                                                             │
│  3.3V  ●──────┬──────────────────────┐                     │
│               │                      │                     │
│  GND   ●──────┼────────┐             │                     │
│               │        │             │                     │
│  A0    ●──────┼────┬───┼─────────────┼─────────┐           │
│               │    │   │             │         │           │
└───────────────┼────┼───┼─────────────┼─────────┼───────────┘
                │    │   │             │         │
                │    │   │             │         │
         To FSR │    │   │ To Resistor │         │ Junction
                │    │   │             │         │
                │    │   │    ┌────────▼────┐    │
                │    │   │    │   10kΩ      │    │
                │    │   │    │  Resistor   │    │
                │    │   │    └─────────────┘    │
                │    │   │                       │
                │    └───┴───────────────────────┘
                │            Junction Point
                │           (A0 reads here)
                │
                ▼
         ┌─────────────┐
         │     FSR     │
         │   Sensor    │
         └─────────────┘
                │
                ▼
              (GND)

Voltage Divider Calculation:
────────────────────────────
Vout = Vin × (R_FSR / (R_FSR + R_fixed))

No pressure: R_FSR = ∞     → Vout ≈ 3.3V → ADC ≈ 1023
Pressure:    R_FSR = 1kΩ   → Vout ≈ 0.3V → ADC ≈ 90
```

## Breadboard Layout

```
Breadboard Connection Diagram:

     ESP8266 NodeMCU
     ┌─────────────┐
     │    USB      │
     │   ┌───┐     │
     │   └───┘     │
     │             │
     │ 3V3●  ●A0   │
     │             │
     │ GND●        │
     │             │
     └─────────────┘
        │ │   │
        │ │   └────────────┐
        │ │                │
        │ └──────┐         │
        │        │         │
        ▼        ▼         ▼

    [BREADBOARD]
    
    Row 1:  3.3V ─── [10kΩ Resistor] ─── Junction (Row 2)
    Row 2:  Junction ─── A0 (sense point)
    Row 3:  Junction ─── [FSR] ─── Row 4
    Row 4:  GND from FSR
    Row 5:  GND from ESP8266
```

## Physical Installation Diagram

```
Side View of Bed:

         [Person Sleeping]
              ┌─┴─┐
              │   │
     ┌────────┴───┴─────────┐
     │      Mattress         │
     │                       │
     ├───────────────────────┤  ◄── Place FSR here
     │    [FSR Sensor]       │      (tape to slat)
     ├───────────────────────┤
     │      Bed Slats        │
     │                       │
     ├───────────────────────┤
     │      Bed Frame        │
     │                       │
     │    ┌─────────────┐    │
     │    │ ESP8266 Box │    │  ◄── Under bed
     │    └──────┬──────┘    │
     └───────────┼───────────┘
                 │
                 ▼
            (USB Power)

FSR Placement (Top View):

    ┌─────────────────────────┐
    │         HEADBOARD       │
    └─────────────────────────┘
    ┌─────────────────────────┐
    │                         │
    │      ▲                  │
    │      │ 1/3 distance     │
    │      │ from headboard   │
    │      ▼                  │
    │         ◉  ◄── FSR      │
    │       (Center)          │
    │                         │
    │                         │
    │                         │
    └─────────────────────────┘
    ┌─────────────────────────┐
    │         FOOTBOARD       │
    └─────────────────────────┘
```

## Load Cell Alternative Wiring

```
Load Cell Setup:

    ┌─────────────┐
    │  Load Cell  │  (Under bed leg)
    │             │
    │ R  B  W  G  │  ◄── 4 wires
    └─┬──┬──┬──┬──┘
      │  │  │  │
      │  │  │  └──────────────┐
      │  │  └─────────────┐   │
      │  └────────────┐   │   │
      └───────────┐   │   │   │
                  │   │   │   │
                  ▼   ▼   ▼   ▼
               ┌──────────────────┐
               │   HX711 Module   │
               │                  │
               │  E+ E- A- A+     │
               │                  │
               │  V  G  DT SCK    │
               └──┬──┬──┬───┬────┘
                  │  │  │   │
                  ▼  ▼  ▼   ▼
               ┌────────────────┐
               │   ESP8266      │
               │                │
               │  3V G  D1  D2  │
               └────────────────┘

Color Code:
  R (Red)   → E+ (Excitation +)
  B (Black) → E- (Excitation -)
  W (White) → A- (Signal -)
  G (Green) → A+ (Signal +)
```

## Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    DATA FLOW                                │
└─────────────────────────────────────────────────────────────┘

1. SENSOR READING
   ───────────────
   [Physical Pressure] → [FSR] → [Voltage Change] → [ESP8266 A0]
                                                           │
                                                           ▼
2. PROCESSING                                    [Analog to Digital]
   ──────────                                              │
                                                           ▼
                                            [Compare to Threshold]
                                                           │
                                          ┌────────────────┴────────────────┐
                                          ▼                                 ▼
                                    Value ≥ Threshold              Value < Threshold
                                          │                                 │
                                          ▼                                 ▼
                                    [Occupied]                         [Empty]
                                          │                                 │
                                          └─────────────┬───────────────────┘
                                                        │
3. DEBOUNCE                                             ▼
   ────────                                    [Wait 3 seconds]
                                                        │
                                                        ▼
                                                 [State Changed?]
                                                        │
                                          ┌─────────────┴─────────────┐
                                          ▼                           ▼
                                         Yes                          No
                                          │                           │
                                          ▼                           ▼
4. MQTT PUBLISH                     [Publish to MQTT]           [Skip]
   ────────────                            │
                                           ▼
                              ┌────────────────────────┐
                              │    MQTT Message        │
                              │                        │
                              │ Topic: .../state       │
                              │ Payload: ON/OFF        │
                              └────────┬───────────────┘
                                       │
                                       ▼
5. HOME ASSISTANT              ┌──────────────┐
   ──────────────              │ MQTT Broker  │
                               └──────┬───────┘
                                      │
                                      ▼
                            ┌──────────────────┐
                            │ Home Assistant   │
                            │                  │
                            │ binary_sensor.   │
                            │ bed_sensor       │
                            └────────┬─────────┘
                                     │
                                     ▼
6. AUTOMATION TRIGGER      ┌─────────────────────┐
   ──────────────────      │   Automations       │
                           │                     │
                           │ - Start Alarm       │
                           │ - Stop Alarm        │
                           │ - Escalate Alarm    │
                           └──────┬──────────────┘
                                  │
                                  ▼
7. ACTIONS                 ┌─────────────────┐
   ───────                 │  TTS / Lights   │
                           │  Switches etc.  │
                           └─────────────────┘
```

## Timing Diagram

```
Time     Bed State    Sensor     MQTT      HA State    Alarm
────     ─────────    ──────     ────      ────────    ─────
00:00    Empty        100        OFF       Clear       -
00:01    Empty        95         -         Clear       -
00:02    Person       650        -         Clear       -
         enters       
00:03    Occupied     700        -         Clear       -
00:04    Occupied     680        -         Clear       -
00:05    Occupied     690        ON        Detected    -
                                 ↑
                                 3s debounce complete
                                 
07:00    Occupied     685        -         Detected    START
                                                        TTS + Light
07:00:15 Occupied     680        -         Detected    TTS + Light
07:00:30 Occupied     675        -         Detected    TTS + Light
07:02:00 Occupied     690        -         Detected    ESCALATE
                                                        Louder TTS
07:02:15 Person       200        -         Detected    TTS + Light
         exits
07:02:16 Empty        80         -         Detected    TTS + Light
07:02:17 Empty        75         -         Detected    TTS + Light
07:02:20 Empty        70         OFF       Clear       STOP
                                 ↑                      ↑
                                 5s debounce            Alarm stops
```

## Legend

```
Symbols Used:
─────────────
●  Connection point
│  Vertical line
─  Horizontal line
┌┐ Corners
└┘ Corners
◄─ Arrow pointing left
─► Arrow pointing right
▲  Arrow pointing up
▼  Arrow pointing down
◉  Center point / sensor location
[  ] Box / component
```

---

**Print this page for easy reference during installation!**
