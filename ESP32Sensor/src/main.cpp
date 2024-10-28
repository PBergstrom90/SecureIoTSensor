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

  // Check if the certificates exist
  if (!LittleFS.exists("/ca.crt") || !LittleFS.exists("/client.crt") || !LittleFS.exists("/client.key")) {
    Serial.println("ERROR: Certificate file(s) missing");
    ESP.restart();
    return;
  }

  // Load certificates from LittleFS
  readCertFile("/ca.crt", ca_cert_str);
  readCertFile("/client.crt", client_cert_str);
  readCertFile("/client.key", client_key_str);

  connectToMQTT();
}

void loop() {
  static unsigned long lastRunTime = 0; // Variable to store last execution time
  const unsigned long interval = 5000;  // 5-second interval
  if (millis() - lastRunTime >= interval) {
    lastRunTime = millis();  // Update last run time
    Serial.println("FIRMWARE VERSION: " FIRMWARE_VERSION);

    float temperature, humidity;
    readSensorData(temperature, humidity);

    // Serial printout for debug
    printSensorData(temperature, humidity);

    // Connect to MQTT if disconnected
    if (!mqttClient.connected()) {
      connectToMQTT();
    }
    mqttClient.loop();

    // Process sensor data and publish to MQTT
    processSensorData(temperature, humidity);
    
    handleOTAUpdate();
  }
  mqttClient.loop();  // Keep MQTT connection alive outside the interval
}