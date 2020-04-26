// Pins

/**
 * PIN of the relay that closes the light-latching-switch.
 */
#define CONFIG_PIN_LIGHT_TOGGLE_RELAY 5

/**
 * PIN of the relay where the current light status can be detected.
 */
#define CONFIG_PIN_LIGHT_DETECTION_RELAY 4

/**
 * Duration for how long the latching switch should be activated.
 */
#define CONFIG_LIGHT_SWITCH_ACTIVATION_DURATION 150

// WiFi
#define CONFIG_WIFI_SSID "{WIFI-SSID}"
#define CONFIG_WIFI_PASS "{WIFI-PASSWORD}"

// MQTT
#define CONFIG_MQTT_HOST "{MQTT-SERVER}"
#define CONFIG_MQTT_PORT 1883 // Usually 1883
#define CONFIG_MQTT_USER "{MQTT-USERNAME}"
#define CONFIG_MQTT_PASS "{MQTT-PASSWORD}"
#define CONFIG_MQTT_CLIENT_ID "ESP_LED" // Must be unique on the MQTT network

// MQTT Topics
#define CONFIG_MQTT_TOPIC_STATE "home/ESP_LED"
#define CONFIG_MQTT_TOPIC_SET "home/ESP_LED/set"

#define CONFIG_MQTT_PAYLOAD_ON "ON"
#define CONFIG_MQTT_PAYLOAD_OFF "OFF"

// Set the mode for the built-in LED on some boards.
// -1 = Do nothing. Leave the pin in its default state.
//  0 = Explicitly set the LED_BUILTIN to LOW.
//  1 = Explicitly set the LED_BUILTIN to HIGH. (Off for Wemos D1 Mini)
#define CONFIG_LED_BUILTIN_MODE -1

// Enables Serial and print statements
#define CONFIG_DEBUG true