#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP32Ping.h>
#include "credentials.h"

void connectToWiFi();
void testPing();

#endif