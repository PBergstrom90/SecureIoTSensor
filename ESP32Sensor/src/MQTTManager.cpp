#include "MQTTManager.h"
#include <LittleFS.h>

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void readCertFile(const char* path, String &dest) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.print("Failed to open file: ");
    Serial.println(path);
    return;
  }
  
  dest = "";
  while (file.available()) {
    dest += file.readString();
  }
  file.close();
}

void connectToMQTT() {
  String ca_cert_str, client_cert_str, client_key_str;

  // Load certificates from LittleFS
  readCertFile("/ca.crt", ca_cert_str);
  readCertFile("/client.crt", client_cert_str);
  readCertFile("/client.key", client_key_str);

  // Set certificates for secure connection
  wifiClient.setCACert(ca_cert_str.c_str());
  wifiClient.setCertificate(client_cert_str.c_str());
  wifiClient.setPrivateKey(client_key_str.c_str());

  mqttClient.setServer("100.106.177.123", 8883); // Set MQTT server (Raspberry Pi's IP and port)

  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println(" connected");
    } else {
      Serial.print(" failed, rc=");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}
