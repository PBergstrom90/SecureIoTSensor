import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
from datetime import datetime
import json

# InfluxDB configuration - Use the Netbird IP address and the token from the InfluxDB UI.
influx_client = InfluxDBClient(url="http://100.106.177.123:8086", 
                               token="ntmN5JXzv1IfobUn6brJOn6k_4DvF5ec9cJIugxB3ffHNHMq4PmR6kaXejT-q784sHk_aY8botnwIAJzda-sHA==", 
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
client.tls_set(ca_certs='/home/pontusbergstrom/Desktop/RaspberryBroker/certs/ca.crt',
               certfile='/home/pontusbergstrom/Desktop/RaspberryBroker/certs/pythonclient.crt',
               keyfile='/home/pontusbergstrom/Desktop/RaspberryBroker/certs/pythonclient.key')
client.tls_insecure_set(True)
client.connect("192.168.50.221", 8883)
client.subscribe("sensor/temperature")
client.subscribe("sensor/humidity")
client.on_message = on_message
client.loop_forever()