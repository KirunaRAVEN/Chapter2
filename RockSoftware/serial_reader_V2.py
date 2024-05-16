import datetime as dt
import serial
import re

# ------------------------------------------
# CONSTANTS USED FOR ADC TO VALUE CONVERSION
# ------------------------------------------

#Which pressure sensors corresponds to which "location"
FEEDING_PRESSURE_OXIDIZER = 0
LINE_PRESSURE = 1
CHAMBER_PRESSURE = 2
FEEDING_PRESSURE_N2 = 3

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
 
#---NOT ATTACHED---
#Pressure sensor calibration data for pressure sensor 4 (Serial No: 1086284) OXIDIZER FEEDING BACKUP
pressureZero4 = -0.005                           #Voltage
pressureSpan4 = 5.035                            #Voltage
pressureLinearity4 = 0.14196                     #in precentage. Not used for calibration
pressureLine_K4 = maxPressure5V_100Bar / pressureSpan4  #Slope of the calibrated data
#Zero offset of the calibrated data
maunalPressureOffset4 = 0                        #How many bars of offset is seen in experimental data
pressureLine_B4 = maxPressure5V_100Bar - pressureLine_K4 * (pressureSpan4 + pressureZero4) - maunalPressureOffset4
 
#Arrays of 5V pressure sensors calibration data
pressureCalibration_K = [pressureLine_K0, pressureLine_K1, pressureLine_K2, pressureLine_K3]
pressureCalibration_B = [pressureLine_B0, pressureLine_B1, pressureLine_B2, pressureLine_B3]
 
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
    * Measurement is repeated TMP36AverageCount times each loop to reduce noise
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

# ----------------
# START OF PROGRAM
# ----------------

ser = serial.Serial()
ser.port = '/dev/ttyACM0'
ser.baudrate = 115200
ser.timeout = 5
ser.open()
if ser.is_open == True:
    print(f'Serial port {ser.port} is open\n')
    print(ser, '\n')

with open("data.csv", "w", newline='') as file:
  while True:
    data = ser.readline()
    data = data.rstrip(b'\n')
    data = data.replace(b'\x00',b'')
    # split on comma and newline
    data_list = re.split(b',', data) # comma-separated

    if data_list[-1] == b' r\n': # Discard restarting lines
        file.write("\n")
        file.flush()
        continue

    try:
        values = []
        #If length not nominal, skip
        if len(data_list) != 5:
            continue

        #Excecute unmushing

        timestamp = int(data_list[0]) * 8 #timestamp is bitshifted >> 3

        dataBit = int(data_list[1])

        combP = readPressure5V(dataBit & 1023, CHAMBER_PRESSURE)
        dataBit = dataBit >> 10
        lineP = readPressure5V(dataBit & 1023, LINE_PRESSURE)
        dataBit = dataBit >> 10
        n2feedP = readPressure5V(dataBit & 1023, FEEDING_PRESSURE_N2)


        dataBit = int(data_list[2])

        botTemp = readTMP36(dataBit & 1023)
        dataBit = dataBit >> 10
        loadC = readLoad(dataBit & 1023)
        dataBit = dataBit >> 10
        n2oFeedP = readPressure5V(dataBit & 1023, FEEDING_PRESSURE_OXIDIZER)


        dataBit = int(data_list[3])

        pipeT = readTemp(dataBit & 16383)
        dataBit = dataBit >> 14
        nozzT = readTemp(dataBit & 16383)

        dataBit = int(data_list[4])

        IR = readIR(dataBit & 1023)
        dataBit = dataBit >> 10
        msgIndex = dataBit & 255
        dataBit = dataBit >> 8
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
        heatButton = dataBit & 1
        dataBit = dataBit >> 1
        dumpButton = dataBit & 1
        #dataBit = dataBit >> 10
        #IR = readIR(dataBit & 1023)

        """
        dataBit = int(data_list[4])
        print("\ndata_list index 4:")
        print(bin(dataBit))

        print(bin(dataBit & 1023))
        print(dataBit & 1023)
        print(readIR(dataBit & 1023))

        dataBit = dataBit >> 10
        print(bin(dataBit & 15))
        dataBit = dataBit >> 4
        print(bin(dataBit & 7))
        dataBit = dataBit >> 3
        print(bin(dataBit & 7))
        dataBit = dataBit >> 3
        print(bin(dataBit & 1))
        dataBit = dataBit >> 1
        print(bin(dataBit & 1))
        dataBit = dataBit >> 1
        print(bin(dataBit & 1))
        dataBit = dataBit >> 1
        print(bin(dataBit & 1))
        dataBit = dataBit >> 1
        print(bin(dataBit & 1))
        dataBit = dataBit >> 1
        print(bin(dataBit & 1))
        dataBit = dataBit >> 1
        print(bin(dataBit & 1))
        """
        #Generate the csv line

        fstring = f'{timestamp}'
        fstring += f',{n2feedP:.2f}'
        fstring += f',{lineP:.2f}'
        fstring += f',{combP:.2f}'
        fstring += f',{n2oFeedP:.2f}'
        fstring += f',{loadC:.2f}'
        fstring += f',{botTemp:.2f}'
        fstring += f',{0}'
        fstring += f',{nozzT:.2f}'
        fstring += f',{pipeT:.2f}'
        fstring += f',{IR:.2f}'
        fstring += f',{dumpButton}'
        fstring += f',{heatButton}'
        fstring += f',{igniButton}'
        fstring += f',{n2Button}'
        fstring += f',{oxButton}'
        fstring += f',{ignStatus}'
        fstring += f',{valveStatus}'
        fstring += f',{swMode}'
        fstring += f',{swSub}'
        fstring += f',{msgIndex}'

        #if int(msgIndex) != 0: print(msgIndex)

        file.write(fstring + "\n")
        file.flush()

    except Exception as exc:
        print(exc)
        #Corrupted data due to reset
        continue
