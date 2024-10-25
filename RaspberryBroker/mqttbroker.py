import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
from datetime import datetime
import json

# InfluxDB configuration
influx_client = InfluxDBClient(url="http://100.106.177.123:8086", 
                               token="O5Ef_HA_dSXF-_a2yOFtj3p-9yXL_9sU4K53ja-5WHVGbPWMadk-ymNxapSWd8ZJpbl5G1LPjCbI08tmp7yfqQ==", 
                               org="SensorOrg")
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

# Since the MQTT broker is running on the same machine, we can use localhost
client.tls_insecure_set(True)
client.connect("127.0.0.1", 8883)

client.subscribe("sensor/temperature")
client.subscribe("sensor/humidity")
client.on_message = on_message
client.loop_forever()