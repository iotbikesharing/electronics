
import serial
import paho.mqtt.client as mqtt

ser = serial.Serial('/dev/ttyUSB0',9600)

broker_address = '172.16.72.59'

while True:
    data = ser.readline() 
    print data
    client = mqtt.Client("P1")
    client.connect(broker_address)
    client.publish('lora_gps', data)
