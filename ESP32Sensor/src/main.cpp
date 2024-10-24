#include <Arduino.h>
#include <WiFiManager.h>
#include <MQTTManager.h>
#include <OTAManager.h>
#include <SensorManager.h>
#include <credentials.h>
#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  LittleFS.begin();
  connectToWiFi();
  testPing();
  connectToMQTT();
}

void loop() {
  Serial.println("FIRMWARE VERSION: " FIRMWARE_VERSION);
  // Access the DHT sensor
  float temperature, humidity;
  readSensorData(temperature, humidity);

  // Serial printout for debug
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  // Connect to MQTT
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.loop();

  // Check if values are valid (not NaN)
  if (!isnan(temperature) && !isnan(humidity)) {
    // Publish valid data to MQTT
    String temperaturePayload = String("{\"temperature\":") + temperature + "}";
    String humidityPayload = String("{\"humidity\":") + humidity + "}";
    mqttClient.publish("sensor/temperature", temperaturePayload.c_str());
    mqttClient.publish("sensor/humidity", humidityPayload.c_str());
  } else {
    Serial.println("Invalid sensor readings. Not sending data.");
  }
  
  // Check for OTA updates at the end (WIP)
  handleOTAUpdate();
  delay(5000);
}