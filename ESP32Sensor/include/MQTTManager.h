#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "credentials.h"

extern WiFiClientSecure wifiClient;
extern PubSubClient mqttClient;

extern String ca_cert_str;
extern String client_cert_str;
extern String client_key_str;

void connectToMQTT();
void disconnectFromMQTT();
void readCertFile(const char* path, String &dest);

#endif
