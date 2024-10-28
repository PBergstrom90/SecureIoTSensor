#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <dht.h>
#include <Adafruit_Sensor.h>
#include "MQTTManager.h"
#include "OTAManager.h"

void readSensorData(float &temperature, float &humidity);
void processSensorData(float &temperature, float &humidity);
void printSensorData(float &temperature, float &humidity);

#endif
