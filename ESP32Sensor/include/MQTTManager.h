#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "credentials.h"

extern WiFiClientSecure wifiClient;
extern PubSubClient mqttClient;

void connectToMQTT();
void readCertFile(const char* path, String &dest);

#endif
