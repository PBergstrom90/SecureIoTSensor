# IoT Temperature Monitoring System with Secure MQTT (WIP)

## Overview
This project outlines an IoT system that securely transmits temperature data from a sensor to a Raspberry Pi for storage and visualization. The architecture uses mutual TLS (mTLS) for secure communication between devices, and adheres to the Cyber Resilience Act requirements for ensuring secure connections.

![Application Screenshot](resources/SecureIoT-Flowchart.jpg)

## System Architecture

1. **Temperature Sensor**: A wired temperature sensor sends data to the ESP32 for processing.
2. **ESP32**: The ESP32 interprets the data from the sensor and sends the processed data over Wi-Fi using secure MQTT (with mTLS) to the Raspberry Pi 5.
3. **Raspberry Pi 5**: Acts as an MQTT broker and accepts the data from the ESP32. The Raspberry Pi enters the data into InfluxDB, a time-series database.
4. **InfluxDB**: Stores the sensor data securely.
5. **Grafana**: Visualizes the temperature data, allowing real-time monitoring and analysis.

## Security and Compliance with Cyber Resilience Act

### Mutual TLS (mTLS) Setup
- **mTLS** is used for the secure communication between the ESP32 and the Raspberry Pi over MQTT. This ensures that:
  - The ESP32 (client) and Raspberry Pi (server) authenticate each other using certificates.
  - Data transmission is encrypted to prevent eavesdropping and tampering.

## TO DO

#### Steps for mTLS:
1. Generate certificates for both the ESP32 and Raspberry Pi using OpenSSL:
    - CA certificate
    - Server certificate (for Raspberry Pi)
    - Client certificate (for ESP32)
2. Configure the Raspberry Pi MQTT broker (e.g., Mosquitto) to require client authentication with the certificates.
3. Load the certificates into the ESP32 firmware and establish an mTLS connection using the `WiFiClientSecure` class.

### InfluxDB Security
- **Data Encryption**: InfluxDB must be configured to use SSL/TLS for data transmission. Ensure that both data in transit and data at rest are encrypted.
- **Authentication**: Use authentication mechanisms in InfluxDB to control who can access the database and ensure that only authorized users/devices can read/write data.

### Grafana Security
- **Access Control**: Set up authentication in Grafana using OAuth or other strong authentication methods. Restrict access to dashboards based on user roles.
- **HTTPS for Grafana**: Ensure that Grafana is served over HTTPS, to protect data displayed on the dashboards.

## Technologies Used
- **ESP32**: Microcontroller for reading sensor data and sending it over MQTT.
- **Raspberry Pi 5**: Acts as the secure server for receiving data and storing it in a database.
- **Mosquitto**: MQTT broker running on the Raspberry Pi with mTLS support.
- **InfluxDB**: Time-series database for storing sensor data.
- **Grafana**: Visualization tool for real-time monitoring of the sensor data.

## Future Improvements
- **Scalability**: The system can be expanded by adding more sensors or integrating with cloud platforms.
- **Security Enhancements**: Consider integrating multi-factor authentication (MFA) for Grafana and further hardening the Raspberry Pi.
- **Advanced Monitoring**: Implement predictive analytics for the temperature data using machine learning models integrated with Grafana.

## Conclusion
This project establishes a secure and resilient IoT system that is well-suited for professional environments. The use of mTLS ensures that the system complies with modern cybersecurity standards, while tools like InfluxDB and Grafana provide robust data handling and visualization.
