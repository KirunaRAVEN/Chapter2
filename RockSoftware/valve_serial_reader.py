import datetime as dt
import serial
import re
from serial.tools import list_ports


ser = serial.Serial()

#Ivar's regarded addition: Retrieves information about the Arduinos connected to the USB-ports
ports = list_ports.comports()


#Finds which one the Arduino Uno is connected to via reading the 'SER' part of hwid
for port, desc, hwid in sorted(ports):    
      if hwid[22:46] == 'SER=24233323435351912251': 
         print('Target acquired')
         ser.port = port
         break
      else: 
        print('Target not acquired')

#Legacy equipment:
#ser.port = '/dev/ttyACM1'

ser.baudrate = 115200
ser.timeout = 5
ser.open()
if ser.is_open == True:
    print(f'Serial port {ser.port} is open\n')
    print(ser, '\n')

with open("data_valve.csv", "w", newline='') as file:
    ser.reset_input_buffer()
    while True:
      data = ser.readline()
      try:
          data = data.decode().rstrip()
      except:
          data = ''
      print(data)
      data = data.replace('\x00','')
      data = data.replace('\r', '')
      file.write(data + '\n')
      file.flush()
    #else:
    #    if data != b'':
    #        file.write(data.decode() + "\n")
    #        file.flush()
