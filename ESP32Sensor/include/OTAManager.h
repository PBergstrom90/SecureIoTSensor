#ifndef OTAMANAGER_H
#define OTAMANAGER_H

#include "credentials.h"
#include "MQTTManager.h"
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <LittleFS.h>
#include <HTTPClient.h>
#include <Update.h>

#define OTA_URL "https://192.168.50.221:8070/firmware/esp32/firmware.bin"
#define VERSION_URL "https://192.168.50.221:8070/firmware/esp32/version.txt"

extern WiFiClientSecure otaClient;

enum OTAStatus {
  OTA_VERSION_CHECK_FAILED,
  OTA_UP_TO_DATE,
  OTA_NEW_VERSION_AVAILABLE,
  OTA_DOWNLOAD_FAILED,
  OTA_UPDATE_COMPLETED,
  OTA_UPDATE_FAILED,
};

String getLatestVersion();  
OTAStatus checkForOTAUpdate();  
void handleOTAUpdate();

#endif