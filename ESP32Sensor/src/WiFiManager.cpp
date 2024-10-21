#include "WiFiManager.h"

void connectToWiFi() {
  const char* ssid = WIFI_SSID;
  const char* password = WIFI_PASSWORD;

  WiFi.disconnect(true); // Clear any existing WiFi configuration
  delay(1000);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Try 20 times (~10 seconds)
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" connected");

    // Print the ESP32's IP address
    Serial.print("WiFi connected with IP: ");
    Serial.println(WiFi.localIP());

    // Print the RSSI (signal strength)
    Serial.print("Signal strength (RSSI): ");
    Serial.println(WiFi.RSSI());

    // Optionally, print the network gateway
    Serial.print("Gateway IP: ");
    Serial.println(WiFi.gatewayIP());
  } else {
    Serial.println("Failed to connect to WiFi. Check SSID and password.");
  }
}

void testPing() {
  IPAddress brokerIp(192, 168, 50, 221); // Use your broker IP
  Serial.print("Pinging MQTT Broker: ");
  Serial.println(brokerIp);
  int pingResult = Ping.ping(brokerIp);
  if (pingResult >= 0) {
    Serial.println("Ping test successful");
  } else {
    Serial.println("Ping failed. Check network connectivity.");
  }
}