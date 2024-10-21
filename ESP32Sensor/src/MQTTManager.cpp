#include "MQTTManager.h"
#include <LittleFS.h>

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

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
  // Check if the certificates exist
  if (!LittleFS.exists("/ca.crt") || !LittleFS.exists("/client.crt") || !LittleFS.exists("/client.key")) {
    Serial.println("Certificate file(s) missing");
    return;
  }

  String ca_cert_str, client_cert_str, client_key_str;

  // Load certificates from LittleFS
  readCertFile("/ca.crt", ca_cert_str);
  readCertFile("/client.crt", client_cert_str);
  readCertFile("/client.key", client_key_str);

 // Set certificates for secure connection
  wifiClient.setCACert(ca_cert_str.c_str());
  wifiClient.setCertificate(client_cert_str.c_str());
  wifiClient.setPrivateKey(client_key_str.c_str());

  // wifiClient.setInsecure(); // Disable TLS verification (DEBUG)

  mqttClient.setServer(MQTT_HOST, 8883); // Set MQTT hostname for TLS verfication and portnumber

  while (!mqttClient.connected()) {
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
      delay(5000);  // Retry every 5 seconds
    }
  }
}