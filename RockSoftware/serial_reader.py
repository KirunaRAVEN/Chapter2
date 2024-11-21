import datetime as dt
import serial
import time
import re
import serial.tools.list_ports as list_ports
import csv

#import time

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

def readLM235(sensorValue):
    """
    * calibration ADC = ratio of how much the internal voltage is off from 5.00V
    * sensorValue = measured voltage on the pin, within 0...1023
    * maxADC = TBD, since we are using 10 bit analog to digital converter with a voltage divider in the temp-sens circuit
    * refADC = expected ADC voltage (5V)
    * 10mV/Kelvin 
    """

    return (sensorValue/maxADC)*refADC/0.010 -273.15 + 4.7
 
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


# -----------------
# BYTESREAM READING
# -----------------

END_MARKER = 0x7F
ESCAPE_BYTE = 0x7D
ESCAPE_XOR = 0x20

def read_message(ser):
    data = bytearray(20)
    index = 0
    while True:
        byte = ser.read(1)[0]
        if byte == END_MARKER or index == 20:
            if index == 0: return [], 0
            else: return bytes(data), index
        elif byte == ESCAPE_BYTE:
            next_byte = ser.read(1)[0]
            data[index] = next_byte ^ ESCAPE_XOR
            index += 1
        else:
            data[index] = byte
            index += 1
            
                       
# ----------------
# START OF PROGRAM
# ----------------

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
    if unoFound == False:
        if hwid[22:46] == 'SER=24233323435351912251':
            ser1.port = port
            unoFound = True
            continue
        else:
            print("Arduino Uno not found!")

#Legacy equipment:
#ser.port = '/dev/ttyACM0'

ser.baudrate =  normalBaud
ser1.baudrate = 115200
ser.timeout = ser1.timeout = 5
ser.open()
ser1.open()

if ser.is_open == True & ser1.is_open == True:
    print(f'Serial port {ser.port} and {ser1.port} is open\n')
    print(ser, '\n')

with open("data.csv", "w", newline='') as file:
    writer = csv.writer(file)
    #Header to the csv data file
    writer.writerow(["ArduinoMegaTime", "LinePressure", "CharmberPressure", "N2OFeedingPressure2", "N2OFeedingPressure1",
                     "LoadCell", "NULL", "NozzleTemperature", "PipingTemperature", "IR sensor", "DumpValveButtonStatus", 
                     "IgnitionButtonStatus", "NitrogenFeedingButtonStatus", "OxidizerValveButtonStatus", 
                     "IgnitionSwState", "ValveSwSstate", "CurrentSwMode", "CurrentSwSubstate",
                     "ArduinoUNOTime","NitrogenPressure","BottleTemp1","BottleTemp2","BottleStatus1","BottleStatus2", "MessageIndex"])

    #Init values that aren't received always
    nozzT = 0
    pipeT = 0
    IR = 0
    BlankTemp1 = 0
    BlankTemp2 = 0

    timestamp = 0
    lineP = 0
    combP = 0
    n2oFeedP = 0
    n2oFeedP2 = 0
    loadC = 0
    dumpButton = 0
    heatButton = 0
    igniButton = 0
    n2Button = 0
    oxButton = 0
    ignStatus = 0
    valveStatus = 0
    swMode = 0
    swSub = 0
    msgIndex = 0

    oldTime = 0
    #two values for the second arduino
    timestamp2 = 0
    n2FeedP = 0
    BlankTemp1  = 0
    BlankTemp2 = 0
    blanketstatus1 = 0
    blanketstatus2 = 0

    dataPointCount = 0
    maxBufferWait = 0

    ser.reset_input_buffer()
    #no data processing for second arduno yet, just a simple grab
    ser1.reset_input_buffer()

    while True:
        """bufferWait = ser.inWaiting()
        if bufferWait >= maxBufferWait:
            maxBufferWait = bufferWait
            #print(f'Bytes in Waiting: {ser.inWaiting()} at index {dataPointCount}')

        if bufferWait == 0: maxBufferWait = 0
        """

        data, length = read_message(ser)
        byteList = list(data)
        
        #print(byteList)
        dataPointCount += 1
        newTime = time.time()
        if newTime - oldTime > 1:
            #print(f'Sampling rate:{int(dataPointCount / (newTime - oldTime))}Hz')
            oldTime = newTime
            dataPointCount = 0

        """
        if data_list[-1] == b' r\n': # Discard restarting lines
            file.write("\n")
            file.flush()
            continue
        """

        if (length == 3) or (length == 12) or (length == 20):
            data_list = [0, 0, 0, 0, 0]

            #Reset message index to not send same message multiple times
            msgIndex = 0


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

                valveStatus = dataBit & 1
                dataBit = dataBit >> 1
                ignStatus = dataBit & 1
                dataBit = dataBit >> 1
                loadC = readLoad(dataBit & 1023)
                dataBit = dataBit >> 10
                combP = readPressure5V(dataBit & 1023, CHAMBER_PRESSURE)

            else:
                timestamp = int(data_list[0]) << 3 #timestamp is bitshifted >> 3 in TSSW

                #First 32bits, always received
                dataBit = int(data_list[1])

                heatButton = dataBit & 1
                dataBit = dataBit >> 1
                dumpButton = dataBit & 1
                dataBit = dataBit >> 1
                combP = readPressure5V(dataBit & 1023, CHAMBER_PRESSURE)
                dataBit = dataBit >> 10
                lineP = readPressure5V(dataBit & 1023, LINE_PRESSURE)
                dataBit = dataBit >> 10
                n2oFeedP2 = readPressure5V(dataBit & 1023, FEEDING_PRESSURE_OXIDIZER2) 

                #Second 32bits, always received
                dataBit = int(data_list[2])

                swSub = dataBit & 7
                dataBit = dataBit >> 3
                swMode = dataBit & 7
                dataBit = dataBit >> 3
                valveStatus = dataBit & 1
                dataBit = dataBit >> 1
                ignStatus = dataBit & 1
                dataBit = dataBit >> 1
                oxButton = dataBit & 1
                dataBit = dataBit >> 1
                n2Button = dataBit & 1
                dataBit = dataBit >> 1
                igniButton = dataBit & 1
                dataBit = dataBit >> 1
                loadC = readLoad(dataBit & 1023)
                dataBit = dataBit >> 10
                n2oFeedP = readPressure5V(dataBit & 1023, FEEDING_PRESSURE_OXIDIZER)

                if length == 20:
                    #Third 32bits, received every ~100ms
                    dataBit = int(data_list[3])

                    msgIndex = dataBit & 7
                    dataBit = dataBit >> 3  #First part of the msgIndex
                    pipeT = readTemp(dataBit & 16383)
                    dataBit = dataBit >> 14
                    nozzT = readTemp(dataBit & 16383)

                    #Fourth 32bits, received every ~100ms
                    dataBit = int(data_list[4])

                    IR = readIR(dataBit & 1023)
                    dataBit = dataBit >> 10
                    msgIndex += (dataBit & 7) << 3 #Second part of the msgIndex
                    dataBit = dataBit >> 3
                    botTemp = readTMP36(dataBit & 1023) 
            
            
            #--------------
            #Second arduino
            #--------------
            InWaiting = ser1.inWaiting()
            if InWaiting > 0:
                data1 = ser1.readline()
                try:
                    if b'\x00' in data1 or b'\r' in data1:
                        
                        data1 = data1.decode().rstrip()
                        data1 = data1.replace('\x00','')
                        data1 = data1.replace('\r', '')
                        #takes in full string from second arduino and makes it into an array.
                        splitdata = data1.split(", ")
                        splitdata = [int(val) for val in splitdata]
                        if len(splitdata) == 6:
                            timestamp2 = (splitdata[0] << 3) #resolution is +- 8us
                            n2FeedP = readPressure5V(splitdata[1], FEEDING_PRESSURE_N2)
                            BlankTemp1  = readTMP36(splitdata[2])
                            BlankTemp2 = readLM235(splitdata[3])
                            blanketstatus1 = splitdata[4]
                            blanketstatus2 = splitdata[5]
                except:
                    #data1 = ''
                    pass
                """
                data1 = data1.replace('\x00','')
                data1 = data1.replace('\r', '')
                #takes in full string from second arduino and makes it into an array.
                splitdata = data1.split(", ")
                splitdata = [int(val) for val in splitdata]
                if len(splitdata) == 6:
                    timestamp2 = (splitdata[0] << 3) #resolution is +- 8us
                    n2FeedP = readPressure5V(splitdata[1], FEEDING_PRESSURE_N2)
                    BlankTemp1  = readTMP36(splitdata[2])
                    BlankTemp2 = readLM235(splitdata[3])
                    print(BlankTemp1, BlankTemp2)
                    blanketstatus1 = splitdata[4]
                    blanketstatus2 = splitdata[5]
                """
            #Generate the csv line, where splitdata is the second arduino
            # live reader requires msgIndex to be the last row element
            writer.writerow([timestamp, f'{lineP:.2f}', f'{combP:.2f}', f'{n2oFeedP2:.2f}', f'{n2oFeedP:.2f}',
                             f'{loadC:.2f}', 0, f'{nozzT:.2f}', f'{pipeT:.2f}', f'{IR:.2f}', 
                             dumpButton, igniButton, n2Button, oxButton, ignStatus, valveStatus, 
                             swMode, swSub, timestamp2, f'{n2FeedP:.2f}', f'{BlankTemp1:.2f}', f'{BlankTemp2:.2f}',
                             blanketstatus1, blanketstatus2]  + [msgIndex])
            ser1.flush()
            file.flush()