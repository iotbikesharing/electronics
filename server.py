import serial
import paho.mqtt.client as mqtt

ser = serial.Serial('/dev/ttyUSB0',9600)
broker_address = '172.16.72.59'
client = mqtt.Client("P1")
client.connect(broker_address)

while True:
    data = ser.readline()
    
    if "UID:" in data:
        client.publish('rfid', data[5:].strip())
    elif "Bike 1" in data:
        client.publish('lora_location', data[7:])
    else:
        client.publish('lora_gps', data[7:])

    print data
