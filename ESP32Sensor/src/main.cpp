#include <Arduino.h>
#include <WiFiManager.h>
#include <MQTTManager.h>
#include <OTAManager.h>
#include <SensorManager.h>
#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  
  connectToWiFi();
  connectToMQTT();
}

void loop() {
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

  // Publish data to MQTT
  String payload = String("{\"temperature\":") + temperature + ", \"humidity\":" + humidity + "}";
  mqttClient.publish("sensor/temperature", payload.c_str());
  
  // Check for OTA updates at the end
  checkForOTAUpdate();

  delay(5000);
}