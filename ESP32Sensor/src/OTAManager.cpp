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
    latestVersion.trim();  // Trim any leading/trailing whitespace or newline characters
    Serial.print("Latest firmware version fetched: ");
    Serial.println(latestVersion); // Debugging output to see what version is retrieved
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
      delay(1000);  // Delay to allow the message to be printed before reboot
      ESP.restart();  // Reboot after successful OTA
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

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return OTA_VERSION_CHECK_FAILED;
  }

  Serial.println("Reading certificates...");
  String clientCert;
  String clientKey;
  String caCert;

  readCertFile("/client.crt", clientCert);
  readCertFile("/client.key", clientKey);
  readCertFile("/ca.crt", caCert);

  // Set up WiFiClientSecure with certificates
  otaClient.setCertificate(clientCert.c_str());
  otaClient.setPrivateKey(clientKey.c_str());
  otaClient.setCACert(caCert.c_str());

  // Fetch the latest firmware version
  String latestVersion = getLatestVersion();

  // Version check: is it up-to-date or is a new version available?
  if (latestVersion == "") {
    Serial.println("ERROR: Latest version is empty, failed to fetch.");
    return OTA_VERSION_CHECK_FAILED;  // Could not retrieve the version
  } else {
    Serial.print("Current firmware version: ");
    Serial.println(FIRMWARE_VERSION);
    Serial.print("Latest firmware version: ");
    Serial.println(latestVersion);
      if (latestVersion == FIRMWARE_VERSION) {
        Serial.println("Firmware is up-to-date.");
        return OTA_UP_TO_DATE;  // No update needed, firmware is already the latest version
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
  https.end();  // Close HTTPS connection

  if (written != contentLength) {
    return OTA_UPDATE_FAILED;  // Not all bytes were written
  }

  if (!Update.end(true)) {
    return OTA_UPDATE_FAILED; 
  }
  // OTA update successful, reboot the ESP32
  return OTA_UPDATE_COMPLETED;
}