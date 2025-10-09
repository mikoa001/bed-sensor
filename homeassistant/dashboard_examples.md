# Home Assistant Dashboard Card Examples

## Basic Bed Sensor Status Card

```yaml
type: entities
title: Bed Sensor
entities:
  - entity: binary_sensor.bed_sensor
    name: Bed Status
  - entity: input_boolean.aggressive_alarm_enabled
    name: Alarm Enabled
  - entity: input_datetime.alarm_time
    name: Wake Time
  - entity: input_boolean.alarm_active
    name: Alarm Active
```

## Alarm Control Card

```yaml
type: vertical-stack
cards:
  - type: entity
    entity: binary_sensor.bed_sensor
    name: Bed Occupancy
    icon: mdi:bed
  
  - type: entities
    title: Alarm Settings
    entities:
      - entity: input_boolean.aggressive_alarm_enabled
        name: Enable Alarm
      - entity: input_datetime.alarm_time
        name: Alarm Time
  
  - type: conditional
    conditions:
      - entity: input_boolean.alarm_active
        state: 'on'
    card:
      type: button
      name: Stop Alarm
      icon: mdi:alarm-off
      tap_action:
        action: call-service
        service: input_boolean.turn_off
        service_data:
          entity_id: input_boolean.alarm_active
      hold_action:
        action: none
```

## Advanced Dashboard with History

```yaml
type: vertical-stack
cards:
  - type: glance
    title: SleepGuard Status
    entities:
      - entity: binary_sensor.bed_sensor
        name: Bed
      - entity: input_boolean.aggressive_alarm_enabled
        name: Alarm
      - entity: input_boolean.alarm_active
        name: Active
  
  - type: history-graph
    title: Bed Occupancy History
    hours_to_show: 24
    entities:
      - entity: binary_sensor.bed_sensor
        name: Bed Sensor
  
  - type: entities
    title: Alarm Controls
    entities:
      - entity: input_datetime.alarm_time
      - entity: input_boolean.aggressive_alarm_enabled
      - type: button
        name: Test Alarm
        icon: mdi:alarm-check
        tap_action:
          action: call-service
          service: input_boolean.turn_on
          service_data:
            entity_id: input_boolean.alarm_active
```

## Mushroom Card Style (if using Mushroom cards)

```yaml
type: vertical-stack
cards:
  - type: custom:mushroom-entity-card
    entity: binary_sensor.bed_sensor
    name: Bed Sensor
    icon: mdi:bed
    tap_action:
      action: more-info
  
  - type: custom:mushroom-entity-card
    entity: input_boolean.aggressive_alarm_enabled
    name: Aggressive Alarm
    icon: mdi:alarm
    tap_action:
      action: toggle
  
  - type: custom:mushroom-entity-card
    entity: input_datetime.alarm_time
    name: Wake Time
    icon: mdi:clock-outline
  
  - type: custom:mushroom-entity-card
    entity: input_boolean.alarm_active
    name: Alarm Status
    icon: mdi:alarm-bell
    icon_color: red
```

## Mobile-Friendly Quick Control

```yaml
type: vertical-stack
cards:
  - type: custom:button-card
    entity: binary_sensor.bed_sensor
    name: Bed Status
    show_state: true
    styles:
      card:
        - height: 80px
      name:
        - font-size: 18px
      state:
        - font-size: 16px
    state:
      - value: 'on'
        color: rgb(68, 115, 158)
        icon: mdi:bed
      - value: 'off'
        color: rgb(128, 128, 128)
        icon: mdi:bed-empty
  
  - type: horizontal-stack
    cards:
      - type: custom:button-card
        entity: input_boolean.aggressive_alarm_enabled
        name: Alarm
        tap_action:
          action: toggle
        state:
          - value: 'on'
            color: rgb(68, 115, 158)
          - value: 'off'
            color: rgb(128, 128, 128)
      
      - type: custom:button-card
        entity: input_datetime.alarm_time
        name: Time
        show_state: true
        tap_action:
          action: more-info
```

## Lovelace YAML Full Page Example

Add to your Lovelace dashboard configuration:

```yaml
title: SleepGuard
path: sleepguard
icon: mdi:bed
badges: []
cards:
  - type: vertical-stack
    cards:
      - type: markdown
        content: |
          # 🛏️ SleepGuard Bed Sensor
          Advanced alarm system that won't stop until you get out of bed!
      
      - type: entities
        title: Current Status
        show_header_toggle: false
        entities:
          - entity: binary_sensor.bed_sensor
            name: Bed Occupancy
            icon: mdi:bed
          - type: divider
          - entity: input_boolean.alarm_active
            name: Alarm Currently Active
            icon: mdi:alarm-light
      
      - type: entities
        title: Alarm Configuration
        show_header_toggle: false
        entities:
          - entity: input_boolean.aggressive_alarm_enabled
            name: Enable Aggressive Alarm
          - entity: input_datetime.alarm_time
            name: Alarm Time
          - type: divider
          - type: button
            name: Test Alarm Now
            icon: mdi:alarm-check
            action_name: Test
            tap_action:
              action: call-service
              service: input_boolean.turn_on
              service_data:
                entity_id: input_boolean.alarm_active
      
      - type: conditional
        conditions:
          - entity: input_boolean.alarm_active
            state: 'on'
        card:
          type: entities
          title: ⚠️ ALARM ACTIVE
          entities:
            - type: button
              name: Emergency Stop Alarm
              icon: mdi:alarm-off
              tap_action:
                action: call-service
                service: input_boolean.turn_off
                service_data:
                  entity_id: input_boolean.alarm_active
              action_name: STOP
      
      - type: history-graph
        title: Last 24 Hours
        hours_to_show: 24
        entities:
          - entity: binary_sensor.bed_sensor
            name: Bed Occupancy
```

## Installation Instructions

1. Copy any of the above card configurations
2. Go to your Home Assistant dashboard
3. Click the three dots (⋮) → Edit Dashboard
4. Click "+ ADD CARD" button
5. Scroll down and click "Manual" card
6. Paste the YAML configuration
7. Click Save

Note: Some cards require custom card integrations:
- Mushroom cards: Install via HACS
- Button card: Install via HACS
- If card type shows error, install the required custom card first
