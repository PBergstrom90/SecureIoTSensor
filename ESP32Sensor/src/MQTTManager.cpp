#include "MQTTManager.h"
#include <LittleFS.h>

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Global certificate variables
String ca_cert_str;
String client_cert_str;
String client_key_str;

void readCertFile(const char* path, String &dest) {
  File file = LittleFS.open(path, "r");
  Serial.print("Reading file: ");
  Serial.println(path);
    if (!file) {
      Serial.print("Failed to open file: ");
      Serial.println(path);
      return;
    }
  dest = "";
  while (file.available()) {
    dest += char(file.read()); 
  }
  file.close();
}

void connectToMQTT() {
 // Set certificates for secure connection
  wifiClient.setCACert(ca_cert_str.c_str());
  wifiClient.setCertificate(client_cert_str.c_str());
  wifiClient.setPrivateKey(client_key_str.c_str());

  // Set MQTT hostname for TLS verfication and portnumber
  mqttClient.setServer(MQTT_HOST, 8883);
  
  // 5 seconds retry interval
  unsigned long retryInterval = 5000;
  unsigned long lastAttemptTime = 0; 

  while (!mqttClient.connected()) {
    if (millis() - lastAttemptTime >= retryInterval) { 
      lastAttemptTime = millis();
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.println(mqttClient.state());
      // Handle specific error states
      if (mqttClient.state() == MQTT_CONNECT_BAD_PROTOCOL) {
        Serial.println("Bad Protocol - Unsupported protocol version");
      } else if (mqttClient.state() == MQTT_CONNECT_BAD_CLIENT_ID) {
        Serial.println("Bad Client ID - The server rejected the client ID");
      } else if (mqttClient.state() == MQTT_CONNECT_UNAVAILABLE) {
        Serial.println("Server Unavailable - The MQTT server is not available");
      } else if (mqttClient.state() == MQTT_CONNECT_BAD_CREDENTIALS) {
        Serial.println("Bad Credentials - Authentication failed");
      } else if (mqttClient.state() == MQTT_CONNECT_UNAUTHORIZED) {
        Serial.println("Unauthorized - Client is not authorized to connect");
      } else {
        Serial.println("UNKOWN ERROR");
      }
    }
    }
    // Keep MQTT responsive while trying to connect
    mqttClient.loop();
  }
}

void disconnectFromMQTT() {
  if (mqttClient.connected()) {
    Serial.println("Disconnecting from MQTT... ");
    mqttClient.disconnect();
    delay(100);
  }
}