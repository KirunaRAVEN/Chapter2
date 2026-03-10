import csv
from collections import deque, defaultdict
import dearpygui.dearpygui as dpg
from ui_helpers import updateDisplay, updateGraph, updateRelay, updateLog, assumedPressure, temperatureChange,classifyDrop, pressureDrop, updatePressureLine, updateSoftwareMode, updateSoftwareSubstrate

import os

# Dict of all indices
INDEX = {
    "MegaTime": 0,
    "OxBottle1_Pres": 1,
    "OxBottle2_Pres": 2,
    "Line_Pres": 3,
    "Chamber_Pres": 4,
    "N2Bottle_Pres": 5,
    "OxBottle1_temp": 6,
    "OxBottle2_temp": 7,
    "NozzleTemp": 8,
    "IRsensor":8,
    "Software_mode": 9,
    "Software_substrate": 10,
    "MsgIndex": 11,
    "Dump_Relay": 12,
    "OxBottle1_Relay": 13,
    "OxBottle2_Relay": 14,
    "IgnRelay": 15,
    "N2_Relay": 16,
    "Ox1Valve_Realy": 17,
    "valveActive": 18,
    "ignitionActive": 18,
}

# Full message string list
MESSAGE_STRINGS = [
    " ",  
    "Running testing sequence\nTesting all OFF-states...\nRelease all buttons!\n",
    "No button presses detected\n",
    "Please release the oxidizer valve button.\n",
    "Please release the ignition button.\n",
    "Please release the heating blanket button.\n",
    ">>>  PASSED  <<<\n",
    ">>>  FAILED  <<<\n",
    "Ignition 24V relay OFF-state:\n",
    "Ignition GND relay OFF-state:\n",
    "Ignition SW relay OFF-state:\n",
    "Heating relay OFF-state:\n",
    "Oxidizer Valve OFF-state:\n",
    "Testing heating relay ON-state...\nPress the Heating button!\n",
    "Heating button press detected\n",
    "Heating relay ON-state:\n",
    "Release the Heating button\n",
    "Testing Oxidizer Valve ON-state...\nPress the Oxidizer Valve button!\n",
    "Oxidizer Valve button press detected\n",
    "Oxidizer Valve ON-state:\n",
    "Release the Oxidizer Valve button\n",
    "Testing ignition relays ON-state...\nPress the Ignition button!\n",
    "Ignition button press detected\n",
    "Ignition Power relay ON-state:\n",
    "Ignition Ground relay ON-state:\n",
    "Ignition Software relay ON-state:\n",
    "Release the Ignition button\n",
    "Actuator testing completed...\nVerification status:\n",
    "All tests passed!\nStarting up software...\n",
    "Fault detected!\nFind and fix the issue!\n",
    "Starting SW in 10 seconds\n",
    "Warning:\nCannot begin sequence\nwith dump valve open.\n",
    "Warning:\nCannot begin sequence\nwith N2 feeding valve open.\n",
    "Warning:\nCannot begin sequence\nwith Oxidizer valve open.\n"
]

# A quite nice way of creating a data dict
class recivedDataDict:
    def __init__(self, **kwargs):
        for k, v in kwargs.items():
            setattr(self, k, v)

    def __repr__(self):
        return f"<recivedDataDict {vars(self)}>"


class telemetryReader:
    # Reads one CSV row at a time, parses into MegaData, and caches per frame
    def __init__(self, filename, indexMap, messageStrings):
        base_dir = os.path.dirname(os.path.abspath(__file__))
        self.filename = os.path.join(base_dir, filename) # ensuring we have a relative path to the file
        self.INDEX = indexMap
        self.MESSAGE_STRINGS = messageStrings
        self.cacheFresh = False
        self.cachedRow = None
        self.lastRow = None
        self.fp = open(self.filename, "r", newline="")
        self.fp.seek(0, os.SEEK_SET)
    
    def invalidateCache(self):
        # Mark the cached data as stale so it will be refreshed on next call.
        self.cacheFresh = False

    def readNextRow(self):
        try:
            posBefore = self.fp.tell() # save position in file
            line = self.fp.readline()
            if not line:
                return None  # missing new data

            # Guard against partial line 
            if not line.endswith("\n"):
                self.fp.seek(posBefore)         # roll back to previous position
                return None

            # Parse CSV
            row = next(csv.reader([line]))
            if not row or len(row) <= max(self.INDEX.values()):
                return None

            # Build object
            parsed = {}
            for name, i in self.INDEX.items():
                try:
                    parsed[name] = float(row[i])
                except (ValueError, IndexError):
                    parsed[name] = float("nan")

            try:
                msgIdx = int(parsed.get("MsgIndex", -1))
            except (ValueError, TypeError):
                msgIdx = -1

            parsed["message"] = (
                self.MESSAGE_STRINGS[msgIdx]
                if 0 <= msgIdx < len(self.MESSAGE_STRINGS)
                else f"Unknown({msgIdx})"
            )
            return recivedDataDict(**parsed)

        except FileNotFoundError:
            return None


    def getNextData(self):
        # Return the next row of data, caching it for the current frame.


        if self.cacheFresh and self.cachedRow != None:
            return self.cachedRow


        MAX_ROWS_PER_FRAME = 2000

        latest = None
        sawAny = False

        for _ in range(MAX_ROWS_PER_FRAME):
            row = self.readNextRow()
            if row is None:
                break  # EOF
            self.lastRow = row
            sawAny = True
            # Return message rows immediately
            msgIdx = getattr(row, "MsgIndex", float("nan"))
            if msgIdx == msgIdx and msgIdx > 0:  # NaN check
                self.cachedRow = row
                return self.cachedRow
            latest = row

        if sawAny and latest is not None:
            # return the newest
            self.cachedRow = latest
            self.cacheFresh = True
            return self.cachedRow

        # If there are no new rows, return the latest 
        self.cachedRow = self.lastRow
        self.cacheFresh = True
        return self.cachedRow



# Creating buffers for storing pressure values and creating tabel for ease of use
PRESSURE_CHANNELS = [
    # name,            dpg_line_tag,        pretty_label,      data_attr
    ("ox1",            "ox1_pressure_line", "Ox1",             "OxBottle1_Pres"),
    ("ox2",            "ox2_pressure_line", "Ox2",             "OxBottle2_Pres"),
    ("line",           "line_pressure_line","Line",            "Line_Pres"),
    ("n2",             "n2_pressure_line",  "N2",              "N2Bottle_Pres"),
]

BUFFER_SIZE_PD = 900
pipeTimeBuf = deque(maxlen=BUFFER_SIZE_PD)
pressureBuffer = defaultdict(lambda: deque(maxlen=BUFFER_SIZE_PD))


    
def updatePipingDiagram(data, t):
    #returning if there is no pressure drop test needs to be fixed before deployment
    if not (data.Dump_Relay == 0 and data.N2_Relay == 0):
        dpg.set_value("pipe_warning_msg", "")
        return

    # Appending pressure and time values to buffer
    for name, _, _, attr in PRESSURE_CHANNELS:
        pressureBuffer[name].append(float(getattr(data, attr, float("nan"))))
    pipeTimeBuf.append(float(t))
    # Returning if we dont have more then 2 data points in the piping buffer
    if len(pipeTimeBuf) < 2:
        return
    
    messageStrings = []

    for name, tag, label, _ in PRESSURE_CHANNELS:
        if len(pressureBuffer[name]) < 2:
            continue
        # Obtaining the pressure drops
        drop = pressureDrop(pipeTimeBuf, pressureBuffer[name])
        # Appending all warning messages to the message string
        messageStrings.append(updatePressureLine(classifyDrop(drop), tag, label))

        dpg.set_value("pipe_warning_msg", "".join(messageStrings).strip())

# updating plots
def updateAllPlots(data, t):
    updateGraph("Ox1TempPlot", t, data.OxBottle1_temp, "Ox1TempXAxis")
    updateGraph("Ox2TempPlot", t, data.OxBottle2_temp, "Ox2TempXAxis")
    updateGraph("Ox1PressurePlot", t, data.OxBottle1_Pres, "Ox1PresXAxis")
    updateGraph("Ox2PressurePlot", t, data.OxBottle2_Pres, "Ox2PresXAxis")
    updateGraph("LinePressurePlot", t, data.Line_Pres, "LinePresXAxis")
    updateGraph("N2PressurePlot", t, data.N2Bottle_Pres, "N2PresXAxis")


# Tabel for temp channels + declaration of buffer
TIME_TEMP_BUFFER_SIZE = 300
TEMP_CHANNELS = [
    # name,               data_attr                 Tag
    ("Temp1",            "OxBottle1_temp",          "DeltaTempOx1PrintOut"),
    ("Temp2",            "OxBottle2_temp",          "DeltaTempOx2PrintOut"),
]
tempBuffer = defaultdict(lambda: deque(maxlen=TIME_TEMP_BUFFER_SIZE))
timeBuffer = deque(maxlen=TIME_TEMP_BUFFER_SIZE)

# updating data displays
def updateAllDataDisplays(data, t):
    # updating the data displays, could be done with a similar method to temp channels but might be unneded
    updateDisplay("data_display:_ox._bottle_1_pressure:", data.OxBottle1_Pres)
    updateDisplay("data_display:_ox._bottle_2_pressure:", data.OxBottle2_Pres)
    updateDisplay("data_display:_line_pressure:", data.Line_Pres)
    updateDisplay("data_display:_nitrogen_pressure:", data.N2Bottle_Pres)
    updateDisplay("data_display:_chamber_pressure:", data.Chamber_Pres)
    updateDisplay("data_display:_plume_temperature:", data.IRsensor)
    updateDisplay("data_display:_nozzle_temperature:", data.NozzleTemp)
#    updateDisplay("data_display:_load_cell:", data.LoadCell)
    updateDisplay("data_display:_mega_time:", t)

    updateDisplay("Ox1TempPrintout", data.OxBottle1_temp)
    updateDisplay("Ox2TempPrintout", data.OxBottle2_temp)
    updateDisplay("AssumedPressureOx1PrintOut", assumedPressure(data.OxBottle1_temp))
    updateDisplay("AssumedPressureOx2PrintOut", assumedPressure(data.OxBottle2_temp))

    # appending values to the temp and time buffer
    timeBuffer.append(t)
    for name, attr,_ in TEMP_CHANNELS:
        tempBuffer[name].append(getattr(data, attr))
    
    # updating the data display for temperature change
    for name,_, tag in TEMP_CHANNELS:
        if len(tempBuffer[name]) >= 30:
            updateDisplay(tag, temperatureChange(tempBuffer[name], timeBuffer))

# Updating all the relays indivdually using the update relay helper
def updateAllRelays(data):

    updateRelay("HeatingRelay1StateText", "HeatingRelay1StatusCircle", data.OxBottle1_Relay)
    updateRelay("HeatingRelay2StateText", "HeatingRelay2StatusCircle", data.OxBottle2_Relay)
    updateRelay("OxRelay1StateText", "OxRelay1StatusCircle", data.Ox1Valve_Realy)
    updateRelay("OxRelay2StateText", "OxRelay2StatusCircle", data.Ox1Valve_Realy)
    updateRelay("IgnRelayStateText", "IgnRelayStatusCircle", data.IgnRelay)

# Logs all new messages in the SW log, avoids null messages (Index = 0)
def updateAllLogs(data, t):
    if data.MsgIndex == 0:
        return
    else:
        updateLog("software_log", data.message, t)

def updateSoftwareModeAndSubstrate(data):
    updateSoftwareMode("softwareModeText", data.Software_mode)
    updateSoftwareSubstrate("softwareSubstrateText", data.Software_substrate)



# Update function for the main loop, ensures we update all data within the data display and then mark the cache as stale
reader = telemetryReader("data.csv", INDEX, MESSAGE_STRINGS)

lastTime = 0
timeOffset = 0
def updateFrame():
    global lastTime, timeOffset
    # Ensures we dont run updatre function without having data
    data = reader.getNextData()
    if not data:
        return
    t = data.MegaTime * 1e-3 #Converting to seconds
    if t < lastTime:
        timeOffset += lastTime
    try:
        updatePipingDiagram(data, t)
        updateAllPlots(data, t+timeOffset)
        updateAllDataDisplays(data, t)
        updateAllRelays(data)
        updateAllLogs(data, t)
        updateSoftwareModeAndSubstrate(data)
    finally:
        reader.invalidateCache()
    lastTime = t
