import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point, WritePrecision

# InfluxDB configuration
bucket = "sensor_data"
org = "your_org"
token = "your_token"
url = "http://localhost:8086"

client = InfluxDBClient(url=url, token=token, org=org)
write_api = client.write_api(write_options=WritePrecision.S)

# MQTT settings
mqtt_broker = "localhost"  # Raspberry Pi's IP
mqtt_port = 8883

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe("sensor/temperature")

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    print(f"Received '{payload}' from '{msg.topic}' topic")

    # Parse payload and write to InfluxDB
    data = eval(payload)  # assuming payload is a dict-like string
    temperature = data.get("temperature")
    humidity = data.get("humidity")
    
    point = Point("sensor_data") \
        .field("temperature", temperature) \
        .field("humidity", humidity)
    
    write_api.write(bucket=bucket, org=org, record=point)

mqtt_client = mqtt.Client()
mqtt_client.tls_set("/path/to/ca_cert.pem", "/path/to/client_cert.pem", "/path/to/client_key.pem")
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

mqtt_client.connect(mqtt_broker, mqtt_port)
mqtt_client.loop_forever()