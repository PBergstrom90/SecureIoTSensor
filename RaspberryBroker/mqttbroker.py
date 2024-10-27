import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
from datetime import datetime
import json
import os

# InfluxDB configuration
influx_token = os.getenv("INFLUXDB_TOKEN")

influx_client = InfluxDBClient(
	url="https://100.106.177.123:8086", 
	token=influx_token, # Stored locally for security, in ~/.bashrc 
	org="SensorOrg",
	verify_ssl=False  # Use False if self-signed, True if certificate is trusted
)
write_api = influx_client.write_api(write_options=SYNCHRONOUS)

# Callback for MQTT messages
def on_message(client, userdata, message):
    sensor_id = client._username
    data = json.loads(message.payload.decode())
    point = Point("sensor_data") \
        .tag("sensor_id", sensor_id) \
        .time(datetime.utcnow(), WritePrecision.NS)

    if message.topic == "sensor/temperature":
        point.field("temperature", data["temperature"])
    elif message.topic == "sensor/humidity":
        point.field("humidity", data["humidity"])

    write_api.write("SensorBucket", "SensorOrg", point)
    print(f"Data written: {message.payload.decode()}")

# MQTT setup
client = mqtt.Client()
client.tls_set(ca_certs='/etc/mosquitto/certs/ca.crt',
		certfile='/etc/mosquitto/certs/pythonclient.crt',
		keyfile='/etc/mosquitto/certs/pythonclient.key')
client.tls_insecure_set(True)
client.connect("127.0.0.1", 8883)
client.subscribe("sensor/temperature")
client.subscribe("sensor/humidity")
client.on_message = on_message
client.loop_forever()