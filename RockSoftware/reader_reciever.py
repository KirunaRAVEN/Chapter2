#!/bin/python3
import datetime as dt
import serial
import time
import re
import serial.tools.list_ports as list_ports
import csv
import socket
import time
import random
import queue
import threading
import struct


# ------------------------------------------
# CONSTANTS USED FOR ADC TO VALUE CONVERSION
# ------------------------------------------

#Which pressure sensors corresponds to which "location"
FEEDING_PRESSURE_OXIDIZER = 0
LINE_PRESSURE = 1
CHAMBER_PRESSURE = 2
FEEDING_PRESSURE_N2 = 3
FEEDING_PRESSURE_OXIDIZER2 = 4  #new N2O pressure sensor on the same pin as the pld N2 pressure sensor. 

#Which thermocouple corresponds to which "location"
NOT_CONNECTED_0 = 0
NOT_CONNECTED_1 = 1
NOZZLE_TC = 2
AMBIENT_TC = 3

#What resolution will the built in ADC use (bit)
resolutionADC = 10
 
#Maximum ADC value
maxADC = 1023.0
 
#Optimal ADC reference voltage
refADC = 5.00
 
#Measured ADC reference voltage
measuredADC = 5.00
 
#ADC calibration multiplier
calibrationADC = measuredADC / refADC
 
#Pressure sensor maximum pressure
maxPressure5V_100Bar = 100
maxPressure5V_25Bar = 25
 
#Pressure sensor calibration data for pressure sensor 0 (Serial No: 667662) OXIDIZER FEEDING
pressureZero0 = -0.003                           #Voltage
pressureSpan0 = 5.003                            #Voltage
pressureLinearity0 = 0.12493                     #in precentage. Not used for calibration
pressureLine_K0 = maxPressure5V_100Bar / pressureSpan0  #Slope of the calibrated data
#Zero offset of the calibrated data
maunalPressureOffset0 = 0                        #How many bars of offset is seen in experimental data
pressureLine_B0 = maxPressure5V_100Bar - pressureLine_K0 * (pressureSpan0 + pressureZero0) - maunalPressureOffset0

#Pressure sensor calibration data for pressure sensor 1 (Serial No: 1073014) LINE
pressureZero1 = 0.01                             #Voltage
pressureSpan1 = 4.997                            #Voltage
pressureLinearity1 = 0.10154                     #in percent. Not used for calibration
pressureLine_K1 = maxPressure5V_100Bar / pressureSpan1  #Slope of the calibrated data
#Zero offset of the calibrated data
maunalPressureOffset1 = 0                        #How many bars of offset is seen in experimental data
pressureLine_B1 = maxPressure5V_100Bar - pressureLine_K1 * (pressureSpan1 + pressureZero1) - maunalPressureOffset1
 
#Pressure sensor calibration data for pressure sensor 2 (Serial No: 1040112) CHAMBER
pressureZero2 = 0.000                            #Voltage
pressureSpan2 = 4.996                            #Voltage
pressureLinearity2 = 0.03146                     #in precentage. Not used for calibration
pressureLine_K2 = maxPressure5V_25Bar / pressureSpan2  #Slope of the calibrated data
#Zero offset of the calibrated data
maunalPressureOffset2 = 0                        #How many bars of offset is seen in experimental data
pressureLine_B2 = maxPressure5V_25Bar - pressureLine_K2 * (pressureSpan2 + pressureZero2) - maunalPressureOffset2
 
#Pressure sensor calibration data for pressure sensor 3 (Serial No: 1086286) NITROGEN FEEDING
pressureZero3 = -0.005                           #Voltage
pressureSpan3 = 5.007                            #Voltage
pressureLinearity3 = 0.03709                     #in precentage. Not used for calibration
pressureLine_K3 = maxPressure5V_100Bar / pressureSpan3  #Slope of the calibrated data
#Zero offset of the calibrated data
maunalPressureOffset3 = 0                        #How many bars of offset is seen in experimental data
pressureLine_B3 = maxPressure5V_100Bar - pressureLine_K3 * (pressureSpan3 + pressureZero3) - maunalPressureOffset3
 

#Pressure sensor calibration data for pressure sensor 4 (Serial No: 1086284) OXIDIZER FEEDING TWO
pressureZero4 = 0.0091                           #Voltage
pressureSpan4 = 5.0001                            #Voltage
pressureLinearity4 = -0.03750                     #in precentage. Not used for calibration
pressureLine_K4 = maxPressure5V_100Bar / pressureSpan4  #Slope of the calibrated data
#Zero offset of the calibrated data
maunalPressureOffset4 = 0                        #How many bars of offset is seen in experimental data
pressureLine_B4 = maxPressure5V_100Bar - pressureLine_K4 * (pressureSpan4 + pressureZero4) - maunalPressureOffset4
 
#Arrays of 5V pressure sensors calibration data
pressureCalibration_K = [pressureLine_K0, pressureLine_K1, pressureLine_K2, pressureLine_K3, pressureLine_K4]
pressureCalibration_B = [pressureLine_B0, pressureLine_B1, pressureLine_B2, pressureLine_B3, pressureLine_B4]
 
#IR sensor minimum and maximum values
minIR = -50
maxIR = 1030
 
#TMP36 ranges
minTMP36 = -40
maxTMP36 = 125
 
#Load cell minimum and maximum values
minLoad = 0
maxLoad = 250 * 4.44822  #Conversion to Newtons
 
#Load cell calibration data.
loadCellZeroPointVoltage = 0.5 #Placeholder value
loadCellSpan = 4.0 #Placeholder value
 
loadCellLine_K = maxLoad / loadCellSpan #Slope of the calibrated data
#Zero offset of the calibrated data
loadCellLine_B = maxLoad - loadCellLine_K * (loadCellSpan + loadCellZeroPointVoltage)


# ----------------------------------------------------------
# FUNCTIONS FOR ADC TO VALUE CONVERSION FOR EACH SENSOR TYPE
# ----------------------------------------------------------

def readPressure5V(pressureVoltage, sensorNum):
    """Measurement to value explanation:
    * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
    * refADV = expected ADC voltage of 5.00V
    * pressureVoltage = measured voltage on the pin, within 0...1023
    * maxADC = 1023, since we are using 10-bit analog to digital converter
    * pressureCalibration_K[sensorNum] = mapping from voltage to bar for this sensor. Includes calibration values
    * pressureCalibration_B[sensorNum] = mapping from voltage to bar for this sensor. Includes calibration values
    * Bars = K * Voltage + B
    """

    pressureVoltage = calibrationADC * refADC * (pressureVoltage / maxADC)
    return pressureCalibration_K[sensorNum] * pressureVoltage + pressureCalibration_B[sensorNum]


def readLoad(sensorValue):
    """Measurement to value explanation:
    * For loop = measure multiple values and take average, load cells are noisy by default.
    * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
    * sensorValue = measured voltage on the pin, within 0...1023
    * maxADC = 1023, since we are using 10-bit analog to digital converter
    * refADV = expected ADC voltage of 5.00V
    * LoadCellLine_K = mapping from voltage to Newtons. Includes calibration values
    * LoadCellLine_B = mapping from voltage to Newtons. Includes calibration values
    * Newtons = K * Voltage + B
    """

    loadVoltage = (sensorValue/maxADC) * refADC * calibrationADC
    return loadCellLine_K * loadVoltage + loadCellLine_B

def readTMP36(sensorValue):
    """Measurement to value explanation:
    * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
    * sensorValue = measured voltage on the pin, within 0...1023
    * maxADC = 1023, since we are using 10-bit analog to digital converter
    * refADV = expected ADC voltage of 5.00V
    * (voltage - 0.5) * 100 = voltage to celsius conversion formula for TMP36 sensors
    """

    temperature = ((calibrationADC * (sensorValue / maxADC)) * refADC - 0.5) * 100
    return temperature

 
def readTemp(temperature):
    return temperature * 0.25
 
def readIR(sensorValue):
    """Measurement to value explanation:
    * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
    * sensorValue = measured voltage on the pin, within 0...1023
    * maxADC = 1023, since we are using 10-bit analog to digital converter
    * maxIR = maximum temperature in C measured by the IR sensor
    * minIR = minimum temperature in C measured by the IR sensor
    * calibratedValue * (maxIR - minIR) + minIR = mapping to the actual temp value
    * Measurement is repeated IrAverageCount times each loop to reduce noise
    """
    temperature = calibrationADC * (sensorValue / maxADC) * (maxIR - minIR) + minIR
    return temperature

# -----------------------------------
# BYTESREAM READING FOR BOTH ARDUINOS
# -----------------------------------
class ByteReader:
    def __init__(self):
        self.buffer = bytearray()
        self.receiving = False
        self.escapebyte = False
        self.packets = []
    def dataprocessing(self, data): 
        for byte in data:
            if byte == 0x7E: #start byte allows us to reset the buffer and all variables at start
                self.buffer = bytearray()
                self.receiving = True
                self.escapebyte = False
                continue
            if not self.receiving:  
                continue
            if byte == 0x7F: #end byte
                self.receiving = False
                self.packets.append(bytes(self.buffer))
                self.buffer = bytearray() #reset buffer for safety
                continue
            if self.escapebyte:
                self.buffer.append(byte ^ 0x20)
                self.escapebyte = False
            elif byte == 0x7D:
                self.escapebyte = True
            else:
                self.buffer.append(byte)
    def read_message(self, ser):
        if ser.in_waiting:
            data = ser.read(ser.in_waiting)
            self.dataprocessing(data)
        packets, self.packets = self.packets, []
        return packets

def changeSocks(listenSock):
    # makes the program wait until a connection to the GS-laptop is made:
    print("listening for new connection ...")
    dataSock = socket.socket(type=socket.SOCK_STREAM)
    debugSock, addr = listenSock.accept() # this is the blocking part
    print(f"connection recieved, setting up datastream to {addr[0]}:6000")
    while True:
        try:
            dataSock.connect((addr[0], 6000))
            break
        except:
            print(".", end="", flush=True)
            time.sleep(1)
    print("connected")
    debugSock.send("connected".encode())
    debugSock.settimeout(0.001) # ooh, i like this hack
    dataSock.settimeout(0.001)
    return (debugSock, dataSock)

def poll(sock):
    try:
        if sock.recv(1) == b'': # times out if connection is alive, but recieves '' if dead
            return False
    except TimeoutError:
        return True
    except socket.timeout: # compatibility with python 3.9
        return True
    except ConnectionResetError:
        return False

#holds packets from the uno and mega
combined_data_queue = queue.Queue()
megaQueue = queue.Queue()
unoQueue = queue.Queue()
#byte readers for the uno and mega
readbytesMega = ByteReader()
readbytesUno = ByteReader()

#thread for the mega
def mega_reader_thread(ser, byte_reader):
    while True:
        packets = byte_reader.read_message(ser)
        for packet in packets:
            megaQueue.put(packet)



#thread for the uno
def uno_reader_thread(ser1, byte_reader):
    while True:
        packets = byte_reader.read_message(ser1)
        for packet in packets:
            unoQueue.put(packet)

#parse_uno_packets
def parse_uno_packets(packet):
    if len(packet) != 12:
        return None 
    timestamp2, n2FeedP , BlankTemp1, BlankTemp2, blanketstatus1, blanketstatus2 = struct.unpack("<IHHHBB", packet)  
    return {
        'timestamp2': timestamp2,
        'n2FeedP': readPressure5V(n2FeedP, FEEDING_PRESSURE_N2),
        'BlankTemp1': readTMP36(BlankTemp1),
        'BlankTemp2': readTMP36(BlankTemp2),
        'blanketstatus1': blanketstatus1,
        'blanketstatus2': blanketstatus2
    }

#thread for combining the data sent from the uno and mega
def combined_data_thread():
    mega_updated = False
    uno_updated = False
    while True:
        if not megaQueue.empty():
            packet = megaQueue.get()
            parsed = parse_mega_packets(packet)
            if parsed:
                with mega_data_LOCKED:
                    mega_data.update(parsed)
                mega_updated = True

        if not unoQueue.empty():
            packet = unoQueue.get()
            parsed = parse_uno_packets(packet)
            if parsed:
                with uno_data_LOCKED:
                    uno_data.update(parsed)
                uno_updated = True
        if not (mega_updated or uno_updated):
            continue
        with mega_data_LOCKED:
            copy_mega_data = mega_data.copy()
        with uno_data_LOCKED:
            copy_uno_data = uno_data.copy()

        CombinedData = (
            copy_mega_data['timestamp'],
            copy_mega_data['lineP'],
            copy_mega_data['combP'],
            copy_mega_data['n2oFeedP2'],
            copy_mega_data['n2oFeedP'],
            copy_mega_data['loadC'],
            copy_mega_data['nozzT'],
            copy_mega_data['pipeT'],
            copy_mega_data['IR'],
            copy_mega_data['dumpButton'],
            copy_mega_data['igniButton'],
            copy_mega_data['n2Button'],
            copy_mega_data['oxButton'],
            copy_mega_data['ignStatus'],
            copy_mega_data['valveStatus'],
            copy_mega_data['swMode'],
            copy_mega_data['swSub'],
            copy_uno_data['timestamp2'],
            copy_uno_data['n2FeedP'],
            copy_uno_data['BlankTemp1'],
            copy_uno_data['BlankTemp2'],
            copy_uno_data['blanketstatus1'],
            copy_uno_data['blanketstatus2'],
            copy_mega_data['msgIndex']
        )
        combined_data_queue.put(CombinedData)

#thread for socket  
def socket_thread():
    #defining format beforehand to increase speeds
    format_string = ("%d,%.2f,%.2f,%.2f,%.2f,%.2f,0,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f,%d,%d,%d\n")
    global debugSock, dataSock
    global dataConn, debugConn
    while True:
        dataConn = poll(dataSock)
        debugConn = poll(debugSock)
        if not (dataConn and debugConn): # if socks are dead, get new ones
            debugSock, dataSock = changeSocks(listenSock)# BLOCKING
            dataConn = poll(dataSock)
            debugConn = poll(debugSock)
        try:
            combined_data = combined_data_queue.get(timeout=0.1)
            dataSock.send((format_string % combined_data).encode())
        except queue.Empty:
            pass
        except:
            # You may want to log this if debugging
            dataConn = False
            debugConn = False

#unpacks all the data the same way as before but does it quicker due to the use of struct.unpack
def parse_mega_packets(packet):
    result = {}
    byteList = list(packet) 
    length = len(packet)

    if (length == 3) or (length == 12) or (length == 20):
        data_list = [0, 0, 0, 0, 0]

        #Reset message index to not send same message multiple times
        result['msgIndex'] = 0


        if length == 3:
            data_list[0] = byteList[0] << 16 | byteList[1] << 8 | byteList[2]

        else:
            data_list[0] = byteList[0] << 24 | byteList[1] << 16 | byteList[2] << 8 | byteList[3]
            data_list[1] = byteList[4] << 24 | byteList[5] << 16 | byteList[6] << 8 | byteList[7]
            data_list[2] = byteList[8] << 24 | byteList[9] << 16 | byteList[10] << 8 | byteList[11]

        if length == 20:
            data_list[3] = byteList[12] << 24 | byteList[13] << 16 | byteList[14] << 8 | byteList[15]
            data_list[4] = byteList[16] << 24 | byteList[17] << 16 | byteList[18] << 8 | byteList[19]

        #Excecute unmushing

        if length == 3:
            #First 32bits, always received
            dataBit = int(data_list[0])

            result['valveStatus'] = dataBit & 1
            dataBit = dataBit >> 1
            result['ignStatus'] = dataBit & 1
            dataBit = dataBit >> 1
            result['loadC'] = readLoad(dataBit & 1023)
            dataBit = dataBit >> 10
            result['combP'] = readPressure5V(dataBit & 1023, CHAMBER_PRESSURE)

        else:
            result['timestamp'] = int(data_list[0]) << 3 #timestamp is bitshifted >> 3 in TSSW

            #First 32bits, always received
            dataBit = int(data_list[1])

            result['heatButton'] = dataBit & 1
            dataBit = dataBit >> 1
            result['dumpButton'] = dataBit & 1
            dataBit = dataBit >> 1
            result['combP'] = readPressure5V(dataBit & 1023, CHAMBER_PRESSURE)
            dataBit = dataBit >> 10
            result['lineP'] = readPressure5V(dataBit & 1023, LINE_PRESSURE)
            dataBit = dataBit >> 10
            result['n2oFeedP2'] = readPressure5V(dataBit & 1023, FEEDING_PRESSURE_OXIDIZER2) 

            #Second 32bits, always received
            dataBit = int(data_list[2])

            result['swSub'] = dataBit & 7
            dataBit = dataBit >> 3
            result['swMode'] = dataBit & 7
            dataBit = dataBit >> 3
            result['valveStatus'] = dataBit & 1
            dataBit = dataBit >> 1
            result['ignStatus'] = dataBit & 1
            dataBit = dataBit >> 1
            result['oxButton'] = dataBit & 1
            dataBit = dataBit >> 1
            result['n2Button'] = dataBit & 1
            dataBit = dataBit >> 1
            result['igniButton'] = dataBit & 1
            dataBit = dataBit >> 1
            result['loadC'] = readLoad(dataBit & 1023)
            dataBit = dataBit >> 10
            result['n2oFeedP'] = readPressure5V(dataBit & 1023, FEEDING_PRESSURE_OXIDIZER)

            if length == 20:
                #Third 32bits, received every ~100ms
                dataBit = int(data_list[3])

                result['msgIndex'] = dataBit & 7
                dataBit = dataBit >> 3  #First part of the msgIndex
                result['pipeT'] = readTemp(dataBit & 16383)
                dataBit = dataBit >> 14
                result['nozzT'] = readTemp(dataBit & 16383)

                #Fourth 32bits, received every ~100ms
                dataBit = int(data_list[4])

                result['IR'] = readIR(dataBit & 1023)
                dataBit = dataBit >> 10
                result['msgIndex'] += (dataBit & 7) << 3 #Second part of the msgIndex
                dataBit = dataBit >> 3
                result['botTemp'] = readTMP36(dataBit & 1023) 
    
    return result if result else None

if __name__ == '__main__':
    # Set up network communication
    listenSock = socket.socket(type=socket.SOCK_STREAM)
    listenSock.bind(("", 5011))
    listenSock.listen(10)
    debugSock, dataSock = changeSocks(listenSock) # BLOCKING
    dataConn = True
    debugConn = True

    # set up data collection:
    normalBaud = 1000000
    ser = serial.Serial()
    ser1 = serial.Serial()
    #Ivar's regarded addition: Retrieves information about the Arduinos connected to the USB-ports
    ports = list_ports.comports()
    #Finds which one the Arduino Mega is connected to via reading the 'SER' part of hwid
    #Now find two
    megaFound = False
    unoFound = False
    for port, desc, hwid in sorted(ports):
        if megaFound == False:
            if hwid[22:46] == 'SER=859373133373515062A1':
                ser.port = port
                megaFound = True
                continue
            else:
                print("Arduino Mega not found!")
                debugSock.send("Arduino Mega not found!".encode())
        if unoFound == False:
            if hwid[22:46] == 'SER=24233323435351912251':
                ser1.port = port
                unoFound = True
                continue
            else:
                print("Arduino Uno not found!")
                debugSock.send("Arduino Uno not found!".encode())
    ser.baudrate =  normalBaud
    ser1.baudrate = 115200
    ser.timeout = 5 
    ser1.timeout = 5 
    ser.open()
    ser1.open()
    if ser.is_open and ser1.is_open:
        print(f'Serial port {ser.port} and {ser1.port} is open\n')
        debugSock.send(f'Serial port {ser.port} and {ser1.port} is open\n'.encode())
        print(ser, '\n')
    
    #Header to the csv data file
    dataSock.send("ArduinoMegaTime,LinePressure,CharmberPressure,N2OFeedingPressure2, N2OFeedingPressure1,LoadCell,NULL,NozzleTemperature,PipingTemperature,IR sensor,DumpValveButtonStatus, IgnitionButtonStatus,NitrogenFeedingButtonStatus,OxidizerValveButtonStatus,IgnitionSwState,ValveSwSstate,CurrentSwMode,CurrentSwSubstate,ArduinoUNOTime,NitrogenPressure,BottleTemp1,BottleTemp2,BottleStatus1,BottleStatus2,MessageIndex".encode())

    #Init values that aren't received always
    mega_data = {
    'timestamp': 0,
    'lineP': 0.0,
    'combP': 0.0,
    'n2oFeedP': 0.0,
    'n2oFeedP2': 0.0,
    'loadC': 0.0,
    'dumpButton': 0,
    'heatButton': 0,
    'igniButton': 0,
    'n2Button': 0,
    'oxButton': 0,
    'ignStatus': 0,
    'valveStatus': 0,
    'swMode': 0,
    'swSub': 0,
    'msgIndex': 0,
    'pipeT': 0.0,
    'nozzT': 0.0,
    'IR': 0.0,
    'botTemp': 0.0
    }
    mega_data_LOCKED = threading.Lock()
    uno_data= {
        'timestamp2':0,
        'n2FeedP':0,
        'BlankTemp1':0,
        'BlankTemp2':0,
        'blanketstatus1':0, 
        'blanketstatus2':0
    }
    uno_data_LOCKED = threading.Lock()

    ser.reset_input_buffer()
    #no data processing for second arduno yet, just a simple grab
    ser1.reset_input_buffer()   
    threading.Thread(target=mega_reader_thread, args=(ser, readbytesMega), daemon=True).start()
    threading.Thread(target=uno_reader_thread, args=(ser1, readbytesUno), daemon=True).start()
    threading.Thread(target=combined_data_thread, daemon=True).start()
    threading.Thread(target=socket_thread,daemon=True).start()
