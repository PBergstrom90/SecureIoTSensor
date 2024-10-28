#include "WiFiManager.h"

void connectToWiFi() {
  const char* ssid = WIFI_SSID;
  const char* password = WIFI_PASSWORD;

  // Clear any existing WiFi configuration
  WiFi.disconnect(true);
  delay(1000);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  const int maxAttempts = 20;
  const unsigned long attemptInterval = 500; // 0.5 seconds
  unsigned long lastAttemptTime = 0;
  int totalRetries = 0;

  // Continue trying until connected or reaching a max retry count
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - lastAttemptTime >= attemptInterval) {
      lastAttemptTime = millis();
      Serial.print(".");
      totalRetries++;

      if (totalRetries >= maxAttempts) {
        // Reset retries for the next cycle
        totalRetries = 0;
        Serial.println("ERROR: Max connection attempts reached. Restarting...");
        delay(3000);  
        ESP.restart();
      }
    }
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
  IPAddress brokerIp(BROKER_IP);
  Serial.print("Pinging MQTT Broker: ");
  Serial.println(brokerIp);
  int pingResult = Ping.ping(brokerIp);
  if (pingResult >= 0) {
    Serial.println("Ping test successful");
  } else {
    Serial.println("Ping failed. Check network connectivity.");
  }
}