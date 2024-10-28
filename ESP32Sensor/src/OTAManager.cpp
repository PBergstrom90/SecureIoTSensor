#include "OTAManager.h"

// Create a new WiFiClientSecure-object for OTA updates, separate from the MQTT connection
WiFiClientSecure otaClient;

// Function to fetch the latest version from the server
String getLatestVersion() {
  Serial.println("Fetching latest firmware version...");
  HTTPClient https;
  https.begin(otaClient, VERSION_URL);
  int httpCode = https.GET();
  if (httpCode == HTTP_CODE_OK) {
    String latestVersion = https.getString();
    latestVersion.trim();
    Serial.print("Latest firmware version fetched: ");
    Serial.println(latestVersion);
    https.end();
    return latestVersion;
  } else {
    Serial.printf("Failed to fetch version.txt, error: %s\n", https.errorToString(httpCode).c_str());
    https.end();
    return "";
  }
}

void handleOTAUpdate() {
  OTAStatus status = checkForOTAUpdate();
  Serial.print("OTA Status: ");
  Serial.println(status);

  switch (status) {
    case OTA_VERSION_CHECK_FAILED:
      Serial.println("ERROR: Failed to check firmware version.");
      break;
    case OTA_UP_TO_DATE:
      Serial.println("Firmware is up-to-date.");
      break;
    case OTA_NEW_VERSION_AVAILABLE:
      Serial.println("New firmware version available. Proceeding with OTA...");
      break;
    case OTA_DOWNLOAD_FAILED:
      Serial.println("ERROR: Firmware download failed.");
      break;
    case OTA_UPDATE_COMPLETED:
      Serial.println("SUCCESS: OTA update completed! Rebooting...");
      disconnectFromMQTT();
      delay(1000);
      ESP.restart(); 
      break;
    case OTA_UPDATE_FAILED:
      Serial.println("ERROR: OTA update failed.");
      break;
    default:
      Serial.println("ERROR: Unknown OTA status.");
      break;
  }
}

// Function to check for OTA updates and manage the OTA process
OTAStatus checkForOTAUpdate() {
  Serial.println("Checking for OTA updates...");

  // Minimum free memory requirement for OTA, e.g., 100 KB
  const size_t minFreeMemory = 100 * 1024; 

  // Disconnect from MQTT to free up memory temporarily
  disconnectFromMQTT();
  // Force garbage collection of String heap memory
  String().reserve(0);

  // Check if there’s enough free memory before proceeding with OTA
  size_t freeMemory = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
  if (freeMemory < minFreeMemory) {
    Serial.println("ERROR: Insufficient memory for OTA update.");
    return OTA_UPDATE_FAILED;
  }

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return OTA_VERSION_CHECK_FAILED;
  }

  // Set up WiFiClientSecure with certificates  
  Serial.println("Reading certificates...");
  otaClient.setCertificate(client_cert_str.c_str());
  otaClient.setPrivateKey(client_key_str.c_str());
  otaClient.setCACert(ca_cert_str.c_str());

  // Fetch the latest firmware version
  String latestVersion = getLatestVersion();
  // Trim whitespace to avoid memory fragmentation
  latestVersion.trim();

  // Version check: is it up-to-date or is a new version available?
  if (latestVersion == "") {
    Serial.println("ERROR: Latest version is empty, failed to fetch.");
    return OTA_VERSION_CHECK_FAILED;
  } else {
    Serial.print("Current firmware version: ");
    Serial.println(FIRMWARE_VERSION);
    Serial.print("Latest firmware version: ");
    Serial.println(latestVersion);
      if (latestVersion.equals(FIRMWARE_VERSION)) {
        Serial.println("Firmware is up-to-date.");
        return OTA_UP_TO_DATE;
      }
    }

  // Proceed with OTA download if a new version is available
  Serial.println("New firmware version available, proceeding with OTA...");
  HTTPClient https;
  https.begin(otaClient, OTA_URL);

  int httpCode = https.GET();
  if (httpCode != HTTP_CODE_OK) {
    return OTA_DOWNLOAD_FAILED;
  }

  int contentLength = https.getSize();
  if (contentLength <= 0) {
    https.end();
    Serial.println("Invalid content size.");
    return OTA_DOWNLOAD_FAILED;
  }

  // Start OTA update
  if (!Update.begin(contentLength)) {
    https.end();
    return OTA_UPDATE_FAILED;
  }

  WiFiClient *stream = https.getStreamPtr();
  size_t written = Update.writeStream(*stream);
  https.end();

  if (written != contentLength) {
    // Not all bytes were written
    return OTA_UPDATE_FAILED;
  }

  if (!Update.end(true)) {
    return OTA_UPDATE_FAILED; 
  }
  // OTA update successful, reboot the ESP32
  connectToMQTT();
  return OTA_UPDATE_COMPLETED;
}