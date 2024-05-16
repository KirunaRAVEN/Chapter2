import datetime as dt
import serial
import re

ser = serial.Serial()
ser.port = '/dev/ttyACM0'
ser.baudrate = 115200
ser.timeout = 5
ser.open()
if ser.is_open == True:
    print(f'Serial port {ser.port} is open\n')
    print(ser, '\n')

file = open("data.csv", "w")

#Header to the csv data file
file.write("Time, Aurdino time, Nitrogen pressure, Line pressure, Chamber pressure, Oxidizer feeding pressure, Load cell, Heating blanket temperature, Not connected, Nozzle temperature, Ambient temperature, Plumb temperature, Dump valve button status, Heating button status, Ignition button status, N2 Feeding button status, Oxidizer valve button status, Ignition SW state, Valve SW state, Current SW mode, Current SW substate, Message field")
file.flush()

while True:
    data = ser.readline()
    
    #Remove null characters
    data = data.replace(b'\x00', '')

    #print(data)
    # split on comma and newline
    data_list = re.split(b' |\n', data) # space-separated
    #print(data_list)
    #if data_list[-2] == b'r': # Discard restarting lines
    #    continue
    # convert to float
    #print(data_list[0])

    dataLine = False
    try:
        if data_list[0].decode() == "d":
            dataLine = True
    except UnicodeDecodeError:
        #Corrupted data due to reset
        continue

    if data_list[0].decode() == "d":
        values = []
        for x in data_list:
            if x != b'':
                try:
                    #print(float(x), end='') # DEBUG
                    values.append(float(x))
                except:
                    #print('ERR ' + x + ' ERR', end='') # DEBUG
                    values.append(x.decode())
        #print('')

        # get timestamp
        timestamp = dt.datetime.now().time()
        fstring = f'{timestamp}'
        for i in range(len(values)):
            fstring += f',{values[i]}'
        file.write(fstring + "\n")
        file.flush()

    else:
        if data != b'':
            file.write(data.decode() + "\n")
            file.flush()
