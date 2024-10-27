# IoT Temperature Monitoring System with Secure MQTT, Nginx, and OTA Updates

## Overview
This project showcases a secure IoT system that transmits temperature data from a sensor to a Raspberry Pi, where it is stored in InfluxDB and visualized using Grafana. Developed in alignment with the Cyber Resilience Act (CRA) requirements, this Proof of Concept (PoC) demonstrates secure communication, robust infrastructure, and adherence to modern cybersecurity standards.

![Application Screenshot](resources/SecureIoT-Flowchart.jpg)

## Quick Start

For setup instructions, please refer to the [QUICKSTART.md](QUICKSTART.md) file.

---

## System Architecture
1. **Temperature Sensor**: The DHT11 sensor gathers temperature and humidity data, which is sent to the ESP32 for processing.
2. **ESP32 (Client)**: Interprets the sensor data and transmits it via MQTT over Wi-Fi to the Raspberry Pi using mutual TLS (mTLS) for secure, authenticated communication.
3. **Raspberry Pi 5 (Server)**: Acts as the MQTT broker, receiving data from the ESP32 and storing it in InfluxDB. Nginx serves as a secure server for hosting OTA firmware files for the ESP32.
4. **Nginx**: Provides secure HTTPS access for the ESP32 to download firmware files during OTA updates.
5. **InfluxDB**: A time-series database that securely stores sensor data.
6. **Grafana**: Provides real-time monitoring and visualization of the temperature data.

---

## Communications and Security Model
- **Secure Data Flow**: All data transfers between the ESP32 and Raspberry Pi are encrypted using mTLS to prevent interception or tampering.
- **Certificate Authentication**: The ESP32 and Raspberry Pi authenticate each other through certificates, ensuring only authorized devices can communicate.
- **Data Integrity and Certificate Management**: Certificates for the ESP32’s `MQTTManager`, `OTAManager`, and `Nginx` are securely stored in **LittleFS** to ensure data security and authenticity.

---

## Compliance with Cyber Resilience Act (CRA)

### Security-by-Design
- End-to-end encryption and certificate-based authentication were implemented from the start to ensure data privacy and integrity.
- The design meets CRA’s expectations for secure-by-default IoT systems.

### Update Capability
- **OTA Updates**: The system includes OTA update functionality for the ESP32, hosted through Nginx on the Raspberry Pi. Future versions will support enhanced OTA mechanisms to ensure continuous security updates and patch management.

### Vulnerability Management
- **Logging and Error Handling**: Error handling for MQTT connections, OTA updates, and certificate management is implemented to detect and handle issues proactively.
- **Logging and Monitoring**: Planned logging and monitoring features will enhance threat detection and enable faster responses to vulnerabilities, aligning with CRA’s vulnerability management requirements.

---

## System Specification

### Architecture
- **Components**:
  - **Sensor**: DHT11, connected to ESP32.
  - **Microcontroller**: ESP32 S3 WROOM for processing and secure data transmission.
  - **Server/Gateway**: Raspberry Pi 5, hosting the Mosquitto MQTT broker and Nginx for OTA file serving.
  - **Database**: InfluxDB for secure storage of time-series data.
  - **Visualization**: Grafana for monitoring and analyzing sensor data.

### Communication Flow
1. The sensor reads data and sends it to the ESP32.
2. ESP32 transmits data over Wi-Fi using mTLS to the Raspberry Pi’s MQTT broker.
3. The Raspberry Pi stores incoming data in InfluxDB.
4. Grafana visualizes the data, making it accessible for analysis and real-time monitoring.
5. Nginx securely hosts OTA firmware files for the ESP32 to access.

### Security Measures
- **mTLS Configuration**:
  - **Certificates**:
    - Generated CA, server, and client certificates for mutual authentication.
    - `WiFiClientSecure` on the ESP32 enables mTLS connections.
  - **Mosquitto**:
    - Configured on the Raspberry Pi to enforce client authentication.
  - **Nginx**:
    - Configured on the Raspberry Pi to enforce HTTPS for OTA updates.
- **Data Security**:
  - Encryption of data in transit (mTLS) and at rest (SSL/TLS for InfluxDB and Nginx).
  - **Role-based Access**: Plans for user-specific permissions in Grafana will restrict dashboard access.
  - **Firewall and Network Security**: Further network security configurations to prevent unauthorized access.

---

### Secure OTA Updates
This system includes OTA updates for the ESP32 using HTTPS with certificate-based verification, hosted through Nginx. This approach ensures data integrity and security compliance with CRA standards.

#### OTA Update Process
1. **Version Check**: The ESP32 queries the Nginx server for the latest firmware version and compares it to its current version.
2. **Certificate-Based Authentication**: The ESP32 uses certificates stored in LittleFS for server verification.
3. **Firmware Download**: If a newer version is detected, the ESP32 securely downloads and installs the update before rebooting.

---

## Technologies Used
- **ESP32**: Microcontroller for data acquisition and secure transmission.
- **Raspberry Pi 5**: Acts as a secure server and MQTT broker.
- **Mosquitto**: mTLS-enabled MQTT broker for secure message transfer.
- **Nginx**: Securely hosts firmware files for OTA updates.
- **InfluxDB**: Time-series database for secure data storage.
- **Grafana**: Visualization dashboard for monitoring data.

---

## Future Improvements
- **Scalability**: Increase sensor count and consider cloud integration for broader data access.
- **Enhanced Security**: Implement multi-factor authentication for Grafana and strengthen network security configurations.
- **TLS Certificate Validation**: Currently, both InfluxDB and MQTT connections use self-signed certificates. While the MQTT client uses `tls_insecure_set(True)` due to localhost validation issues, future improvements should enable full certificate validation for both InfluxDB and MQTT clients to secure connections effectively in production.
- **Secure Credential Storage**: InfluxDB tokens and other sensitive credentials are now managed with environment variables, improving security. Future updates could further enhance this with encrypted storage or a secrets management tool.
- **System Health Monitoring and Alerts**: Future versions of this PoC may integrate system health checks and alerts in Grafana to notify users of connection issues, certificate expiration, or OTA failures.
- **Predictive Analytics**: Explore machine learning models within Grafana for predictive monitoring.

---

## Conclusion
This PoC demonstrates a secure IoT solution for remote monitoring that meets Cyber Resilience Act standards. The system’s secure communication, data handling, and update capabilities provide a solid foundation for further development into a production-ready IoT product.