#include <Arduino.h>

// https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h>

// EDO8266 Wifi Client
#include <ESP8266WiFi.h>

// http://pubsubclient.knolleary.net/
#include <PubSubClient.h>

#include "config.h"
#include "wifi.cpp"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/**
 * Store the last detected light status.
 */
bool LAST_DETECTED_LIGHT_STATUS = false;

const int JSON_DOCUMENT_SIZE = JSON_OBJECT_SIZE(2);

/**
 * Activates the light toggle relay.
 */
void activateLightRelay() {
  digitalWrite(CONFIG_PIN_LIGHT_TOGGLE_RELAY, HIGH);
  delay(CONFIG_LIGHT_SWITCH_ACTIVATION_DURATION);
  digitalWrite(CONFIG_PIN_LIGHT_TOGGLE_RELAY, LOW);
}

void publishLightStatus(bool isOn) {
  StaticJsonDocument<JSON_DOCUMENT_SIZE> jsonDocument;

  jsonDocument["state"] = isOn ? CONFIG_MQTT_PAYLOAD_ON : CONFIG_MQTT_PAYLOAD_OFF;

  char payload[20];
  serializeJson(jsonDocument, payload);
  
  mqttClient.publish(CONFIG_MQTT_TOPIC_STATE, payload, true);
}

/**
 * Check the current status and update the status when it changes.
 */
bool detectLightStatus() {
  // When the relay is closed (lights on) then the pin is connected to GND and therefore detected as LOW.
  const bool currentLightStatus = digitalRead(CONFIG_PIN_LIGHT_DETECTION_RELAY) == LOW;

  if(currentLightStatus != LAST_DETECTED_LIGHT_STATUS) {
    publishLightStatus(currentLightStatus);
    LAST_DETECTED_LIGHT_STATUS = currentLightStatus;
  }

  return currentLightStatus;
}

/**
 * Gets called when a subscribed message is received.
 */
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  StaticJsonDocument<JSON_DOCUMENT_SIZE> jsonDocument;

  auto deserializeError = deserializeJson(jsonDocument, message);

  if (deserializeError) {
    Serial.print("deserializeJson() failed with code ");
    Serial.println(deserializeError.c_str());

    return;
  }

  const bool currentLightStatus = LAST_DETECTED_LIGHT_STATUS;
  const bool requestedLightStatus = jsonDocument.containsKey("state") && strcmp(jsonDocument["state"], CONFIG_MQTT_PAYLOAD_ON);

  if(requestedLightStatus != currentLightStatus) {
    activateLightRelay();
  }
}

void setup() {
  // Set the LED_BUILTIN based on the CONFIG_LED_BUILTIN_MODE
  switch (CONFIG_LED_BUILTIN_MODE) {
    case 0:
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LOW);
      break;
    case 1:
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    default: // Other options (like -1) are ignored.
      break;
  }

  if (CONFIG_DEBUG) {
    Serial.begin(115200);
  }

  setupWifi();

  mqttClient.setServer(CONFIG_MQTT_HOST, CONFIG_MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

void mqttLoop() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(CONFIG_MQTT_CLIENT_ID, CONFIG_MQTT_USER, CONFIG_MQTT_PASS)) {
      Serial.println("connected");
      mqttClient.subscribe(CONFIG_MQTT_TOPIC_SET);
      // TODO: ADD DISCOVERY HERE
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  mqttClient.loop();
}

void loop() {
  mqttLoop();
  detectLightStatus();
}