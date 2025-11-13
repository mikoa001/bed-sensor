#pragma once

// WiFi credentials
const char* ssid = "Telia-1D84C8";
const char* password = "aPKkYr3d3PPGHWmJ";

// MQTT Broker settings
const char* mqtt_server = "192.168.1.118";             // IP of your Home Assistant device, found in the network settings in the HA UI
const int mqtt_port = 1883;                            // Default MQTT port, change it if you have manually changed the port in the MQTT add-on (Mosquitto)
const char* mqtt_user = "mqtt";                        // The HA user (not person, there is a difference) where the mqtt comms will happen on, for limiting the access in case of an cyberattack
const char* mqtt_password = "Hjju7ytfv4%%43ibpplpwf";  // A strong password for the mqtt user so that it is hard to hack
const char* mqtt_client_id = "bed_sensor";             // the id_of this single client (the microcontroller)