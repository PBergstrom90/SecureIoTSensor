# Quickstart Guide for IoT Temperature Monitoring System

This guide provides instructions for setting up the secure IoT system to transmit temperature data from an ESP32 sensor to a Raspberry Pi, where it is stored in InfluxDB and visualized with Grafana.

---

## Software Requirements

Ensure the latest versions of the following software are installed on the Raspberry Pi:

- **Mosquitto** (MQTT Broker)
- **InfluxDB** (Time-series database)
- **Grafana** (Data visualization)
- **Nginx** (Web server for OTA and firmware storage)
- **OpenSSL** (for generating and managing certificates)

---

## Configuration Steps

### 1. Certificate Authority (CA) Creation

1. **Generate the CA Private Key**  
   Generate a 2048-bit private key for the Certificate Authority (CA).

2. **Create the CA Certificate**  
   Generate a self-signed CA certificate valid for 365 days.

---

## 2. Generate Server Certificates

### Mosquitto Server Certificate
1. Generate a 2048-bit private key.
2. Create a Certificate Signing Request (CSR) with the Common Name (CN) as "mosquitto".
3. Sign the CSR with the CA certificate to create the server certificate.

### InfluxDB and Nginx Server Certificate
1. Generate a 2048-bit private key.
2. Create a CSR with the Common Name (CN) as "influxdb".
3. Sign the CSR with the CA certificate to create the `influxdb-server.crt` server certificate.

---

## 3. Generate Client Certificates for ESP32

1. Generate a 2048-bit private key.
2. Create a CSR with the Common Name (CN) as "ESP32Client".
3. Sign the CSR with the CA certificate to create the client certificate.

---

## 4. Configure Mosquitto with mTLS

1. Place the `ca.crt`, `server.crt`, and `server.key` files in the Mosquitto configuration directory (e.g., `/etc/mosquitto/certs`).
2. Edit the Mosquitto configuration file (`mosquitto.conf`) to enforce mTLS, specifying paths to the certificate files.
3. Restart the Mosquitto service to apply changes.

---

## 5. Configure InfluxDB for HTTPS

1. Place the `influxdb-server.crt` and `influxdb-server.key` in the InfluxDB configuration directory (e.g., `/etc/ssl/certs`).
2. Update the InfluxDB service file (`influxdb.service`) to include the certificate paths:
   - Example configuration:
     ```plaintext
     ExecStart=/usr/bin/influxd --tls-cert /etc/ssl/certs/influxdb-server.crt --tls-key /etc/ssl/private/influxdb-server.key
     ```
3. Restart the InfluxDB service to enable HTTPS.

---

## 6. Configure Grafana for HTTPS

1. Place the `influxdb-server.crt` as the CA certificate in the Grafana Data Source settings.
2. In the `grafana.ini` file:
   - Set `protocol = https`.
   - Add paths to `cert_file` and `cert_key` for Grafana’s HTTPS configuration.
3. Restart Grafana.

---

## 7. Configure Nginx for OTA

1. Place the `influxdb-server.crt` and `influxdb-server.key` in the Nginx configuration directory.
2. Update the Nginx configuration file to specify the certificate and key file paths for OTA updates.
3. Restart Nginx to enable secure HTTPS OTA support for the ESP32.

---

## 8. Testing the Setup

1. **ESP32 to Raspberry Pi Communication**: Ensure the ESP32 can securely transmit data via MQTT to the Raspberry Pi.
2. **InfluxDB HTTPS**: Verify that InfluxDB accepts HTTPS requests.
3. **Grafana Visualization**: Check that Grafana can connect to InfluxDB and visualize data over HTTPS.

---

## Troubleshooting

If any issues arise during setup:

- Ensure permissions on certificate files allow read access for respective services.
- Verify that all server and client certificates are correctly signed by the CA.
- Check firewall settings to ensure required ports are open.

---