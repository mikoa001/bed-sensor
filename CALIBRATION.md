# Sensor Calibration Guide

This guide helps you calibrate your bed sensor for optimal performance.

## Step 1: Initial Setup

1. Upload the bed_sensor.ino sketch to your ESP8266
2. Open Serial Monitor (Tools → Serial Monitor)
3. Set baud rate to 115200
4. Ensure WiFi and MQTT connections are successful

## Step 2: Collect Baseline Data

### Empty Bed Reading
1. Ensure bed is completely empty
2. Observe the sensor values in Serial Monitor for 30 seconds
3. Note the range of values (e.g., 50-100)
4. Calculate average: **Empty Bed Average = _____**

### Occupied Bed Reading
1. Have someone lie down on the bed
2. Stay still for 30 seconds
3. Note the range of values (e.g., 600-700)
4. Calculate average: **Occupied Bed Average = _____**

## Step 3: Calculate Threshold

The threshold should be set between the empty and occupied averages:

```
Threshold = (Empty Bed Average + Occupied Bed Average) / 2
```

**Example:**
- Empty Bed Average: 75
- Occupied Bed Average: 650
- Threshold = (75 + 650) / 2 = 362.5 ≈ **363**

## Step 4: Update Code

1. Open bed_sensor.ino
2. Find the line:
   ```cpp
   const int OCCUPIED_THRESHOLD = 512;
   ```
3. Replace 512 with your calculated threshold
4. Upload the updated sketch

## Step 5: Test and Fine-Tune

1. Get in and out of bed several times
2. Check Serial Monitor output:
   - Should show "Bed: OCCUPIED" when in bed
   - Should show "Bed: EMPTY" when out of bed
3. If false triggers occur:
   - Increase `DEBOUNCE_DELAY` from 3000 to 5000 milliseconds
   - Adjust threshold up or down by 10-20 units

## Step 6: Verify in Home Assistant

1. Check the `binary_sensor.bed_sensor` entity in Home Assistant
2. Get in bed - should show "Detected" or "On"
3. Get out of bed - should show "Clear" or "Off" after debounce delay

## Troubleshooting Calibration Issues

### Problem: Sensor always shows occupied
- **Solution**: Increase threshold value
- Your threshold may be too low

### Problem: Sensor always shows empty
- **Solution**: Decrease threshold value
- Your threshold may be too high

### Problem: Frequent false triggers
- **Solution**: Increase debounce delay
- Change from 3000 to 5000 or even 10000 milliseconds

### Problem: Sensor values fluctuate wildly
- **Check**: Sensor wiring connections
- **Check**: Power supply stability
- **Try**: Adding a capacitor (100nF) across sensor connections

### Problem: No reading change between empty/occupied
- **Check**: Sensor is properly positioned under mattress
- **Check**: Sensor is actually connected to A0 pin
- **Try**: Different sensor placement location

## Advanced: Using Load Cell

If using a load cell with HX711 instead of FSR:

1. Modify the code to use HX711 library
2. Calibration will measure actual weight
3. Set threshold based on minimum body weight (e.g., 30kg or 66lbs)

Example threshold for load cell:
```cpp
const float OCCUPIED_THRESHOLD = 30.0; // 30 kg
```

## Optimal Sensor Placement

### FSR Placement:
- Under mattress, near center of bed
- Where most body weight concentrates
- Avoid edge areas
- Test multiple positions if needed

### Load Cell Placement:
- Under bed leg (measures weight distribution)
- All four legs for accurate full bed monitoring
- Ensure level surface for accurate readings

## Recording Your Values

Keep a log for reference:

```
Date: __________
Sensor Type: __________
Empty Bed Reading: __________
Occupied Bed Reading: __________
Threshold Used: __________
Debounce Delay: __________
Notes: _________________________________
```

## Seasonal Adjustments

Note: You may need to recalibrate if:
- You change mattress or bedding
- Seasonal bedding changes (heavy winter blankets)
- Different people use the bed regularly
- Sensor position is adjusted

Recalibrate every 3-6 months or when you notice false triggers.
