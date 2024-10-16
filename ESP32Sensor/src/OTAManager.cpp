#include "OTAManager.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

// WORK IN PROGRESS

#define OTA_URL "http://your-ota-server.com/firmware.bin"
extern WiFiClientSecure wifiClient;

void checkForOTAUpdate() {
  HTTPClient https;
  
  https.begin(wifiClient, OTA_URL);  // HTTPS connection with client certificate
  
  int httpCode = https.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = https.getSize();
    if (contentLength > 0) {
      if (Update.begin(contentLength)) {
        Serial.println("Start OTA update...");
        WiFiClient *stream = https.getStreamPtr();
        
        size_t written = Update.writeStream(*stream);
        if (written == contentLength) {
          Serial.println("OTA update completed!");
          if (Update.end(true)) {
            Serial.println("Update successfully written. Rebooting...");
            ESP.restart();
          } else {
            Serial.printf("Update failed. Error: %s\n", Update.errorString());
          }
        } else {
          Serial.printf("Written only %d/%d bytes. OTA failed.\n", written, contentLength);
        }
      } else {
        Serial.println("Not enough space for OTA update.");
      }
    }
  } else {
    Serial.printf("HTTP request failed, error: %s\n", https.errorToString(httpCode).c_str());
  }
  
  https.end();  // Close HTTPS connection
}
