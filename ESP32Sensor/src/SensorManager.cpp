#include "SensorManager.h"

#define DHTPIN 2 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 

void readSensorData(float &temperature, float &humidity) {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  }
}

void processSensorData(float &temperature, float &humidity) {
  // Check if values are valid (not NaN) and within realistic range
  if (!isnan(temperature) && !isnan(humidity)) {
    if (temperature >= -40 && temperature <= 50 && humidity >= 0 && humidity <= 100) {
      // Buffer for MQTT payloads
      char temperaturePayload[50];
      char humidityPayload[50];

      // Format the payloads using snprintf, to improve memory efficiency
      snprintf(temperaturePayload, sizeof(temperaturePayload), "{\"temperature\":%.2f}", temperature);
      snprintf(humidityPayload, sizeof(humidityPayload), "{\"humidity\":%.2f}", humidity);

      // Publish formatted payloads to MQTT
      mqttClient.publish("sensor/temperature", temperaturePayload);
      mqttClient.publish("sensor/humidity", humidityPayload);
    } else {
      Serial.println("Unrealistic sensor readings detected. Not sending data.");
    }
  } else {
    Serial.println("Invalid sensor readings. Not sending data.");
  }
}

void printSensorData(float &temperature, float &humidity) {
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}
