import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient
from influxdb_client.client.write_api import SYNCHRONOUS
import json

# Correct initialization for InfluxDB 2.x
influx_url = "http://localhost:8086"  # InfluxDB URL
influx_token = "your_token"  # InfluxDB API token
influx_org = "your_org"  # Your InfluxDB organization
influx_bucket = "your_bucket"  # The bucket where data will be stored

# Create InfluxDB client
influx_client = InfluxDBClient(
    url=influx_url, 
    token=influx_token, 
    org=influx_org
)

# Callback when a message is received
def on_message(client, userdata, message):
    print(f"Received message: {message.payload.decode()} on topic: {message.topic}")

    # Parse the received message (assuming JSON format)
    try:
        data = json.loads(message.payload.decode())

        # 4O Logic: Perform validation or transformation based on the 4O logic requirements
        if 'sensor_id' in data and 'temperature' in data and 'humidity' in data:
            # Example 4O logic: Ensuring values are within valid ranges (adjust as needed)
            if 0 <= data['temperature'] <= 50 and 0 <= data['humidity'] <= 100:
                # Prepare data for InfluxDB
                influx_data = [
                    {
                        "measurement": "sensor_data",
                        "tags": {
                            "sensor_id": data["sensor_id"]  # Adjust according to your payload
                        },
                        "fields": {
                            "temperature": data["temperature"],  # Adjust according to your payload
                            "humidity": data["humidity"]  # Adjust according to your payload
                        }
                    }
                ]
                # Write data to InfluxDB
                influx_client.write_points(influx_data)
                print("Data written to InfluxDB.")
            else:
                print(f"Invalid data received: {data}")
        else:
            print("Invalid message format received.")
    except Exception as e:
        print(f"Error processing message: {e}")

# Create a new MQTT client instance
client = mqtt.Client()

# Configure TLS if necessary
client.tls_set('/certs/ca.crt')
client.tls_insecure_set(True)

# Connect to the MQTT broker
client.connect("pobe", 8883)  # Adjust port if necessary

# Subscribe to the topic
client.subscribe("sensor/temperature")  # Replace with your actual topic

# Set the callback for message handling
client.on_message = on_message

# Start the loop
client.loop_forever()