import csv
from collections import deque, defaultdict
import dearpygui.dearpygui as dpg
from ui_helpers import update_pressure_line
from ui_helpers import update_display, update_graph, update_relay, update_log, assumed_pressure, temperature_change, pressure_drop
import os

# Dict of all indices
INDEX = {
    "MegaTime": 0,
    "Line_Pres": 1,
    "Chamber_Pres": 2,
    "OxBottle1_Pres": 3,
    "OxBottle2_Pres": 4,
    "LoadCell": 5,
    "NozzleTemp":7,
    "IRsensor": 9,
    "Dump_Relay": 10,
    "N2_Relay": 12,
    "IgnRelay": 14,
    "Ox1Valve_Realy":15,
    "N2Bottle_Pres": 19,
    "OxBottle1_temp": 20,
    "OxBottle2_temp": 21,
    "OxBottle1_Relay": 22,
    "OxBottle2_Relay": 23,
    "MsgIndex": 24, 
}

# Full message string list
messageStrings = [
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
class Recived_Data_Dict:
    def __init__(self, **kwargs):
        for k, v in kwargs.items():
            setattr(self, k, v)

    def __repr__(self):
        return f"<Recived_Data_Dict {vars(self)}>"


class TelemetryReader:
    # Reads one CSV row at a time, parses into MegaData, and caches per frame
    def __init__(self, filename, index_map, message_strings):
        base_dir = os.path.dirname(os.path.abspath(__file__))
        self.filename = os.path.join(base_dir, filename) # ensuring we have a relative path to the file
        self.INDEX = index_map
        self.messageStrings = message_strings
        self.last_file_position = 0
        self.cache_fresh = False
        self.cached_row = None
        self.last_row = None
        self.fp = open(self.filename, "r", newline="")
        self.fp.seek(0, os.SEEK_SET)
    
    def invalidate_cache(self):
        # Mark the cached data as stale so it will be refreshed on next call.
        self.cache_fresh = False

    def read_next_row(self):
        try:
            pos_before = self.fp.tell() # save position in file
            line = self.fp.readline()
            if not line:
                return None  # missing new data

            # Guard against partial line 
            if not line.endswith("\n"):
                self.fp.seek(pos_before)         # roll back to previous position
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
                msg_idx = int(parsed.get("MsgIndex", -1))
            except (ValueError, TypeError):
                msg_idx = -1

            parsed["message"] = (
                self.messageStrings[msg_idx]
                if 0 <= msg_idx < len(self.messageStrings)
                else f"Unknown({msg_idx})"
            )
            return Recived_Data_Dict(**parsed)

        except FileNotFoundError:
            return None


    def get_next_data(self):
        # Return the next row of data, caching it for the current frame.

        if self.cache_fresh:
            return self.cached_row

        MAX_ROWS_PER_FRAME = 2000

        latest = None
        saw_any = False

        for _ in range(MAX_ROWS_PER_FRAME):
            row = self.read_next_row()
            if row is None:
                break  # EOF
            self.last_row = row
            saw_any = True
            # Return message rows immediately
            msg_idx = getattr(row, "MsgIndex", float("nan"))
            if msg_idx == msg_idx and msg_idx > 0:  # NaN check
                self.cached_row = row
                return self.cached_row
            latest = row

        if saw_any and latest is not None:
            # return the newest
            self.cached_row = latest
            self.cache_fresh = True
            return self.cached_row

        # If there are no new rows, return the latest 
        self.cached_row = self.last_row
        self.cache_fresh = True
        return self.cached_row



# Creating buffers for storing pressure values and creating tabel for ease of use
PRESSURE_CHANNELS = [
    # name,            dpg_line_tag,        pretty_label,      data_attr
    ("ox1",            "ox1_pressure_line", "Ox1",             "OxBottle1_Pres"),
    ("ox2",            "ox2_pressure_line", "Ox2",             "OxBottle2_Pres"),
    ("line",           "line_pressure_line","Line",            "Line_Pres"),
    ("n2",             "n2_pressure_line",  "N2",              "N2Bottle_Pres"),
]

buffer_size_pd = 900
DROP_OK      = -0.33
DROP_WARN    = 1.0 # Everything above this point is critical
pipe_time_buf = deque(maxlen=buffer_size_pd)
pressure_buffer = defaultdict(lambda: deque(maxlen=buffer_size_pd))

def classify_drop(d):
    if d < DROP_OK:
        return "ok"
    if d < DROP_WARN: 
        return "warn"
    return "critical"
    
def update_piping_diagram(data, t):
    #returning if there is no pressure drop test needs to be fixed before deployment
    #if not (data.Dump_Relay == 0 and data.N2_Relay == 0):
    #    dpg.set_value("pipe_warning_msg", "")
    #    return

    # Appending pressure and time values to buffer
    for name, _, _, attr in PRESSURE_CHANNELS:
        pressure_buffer[name].append(float(getattr(data, attr, float("nan"))))
    pipe_time_buf.append(float(t))
    # Returning if we dont have more then 2 data points in the piping buffer
    if len(pipe_time_buf) < 2:
        return
    
    messageString = []

    for name, tag, label, _ in PRESSURE_CHANNELS:
        if len(pressure_buffer[name]) < 2:
            continue
        # Obtaining the pressure drops
        drop = pressure_drop(pipe_time_buf, pressure_buffer[name])
        # Appending all warning messages to the message string
        messageString.append(update_pressure_line(classify_drop(drop), tag, label))

        dpg.set_value("pipe_warning_msg", "".join(messageString).strip())

# updating plots
def update_all_plots(data, t):

    update_graph("Ox1TempPlot", t, data.OxBottle1_temp, "Ox1TempXAxis")
    update_graph("Ox2TempPlot", t, data.OxBottle2_temp, "Ox2TempXAxis")
    update_graph("Ox1PressurePlot", t, data.OxBottle1_Pres, "Ox1PresXAxis")
    update_graph("Ox2PressurePlot", t, data.OxBottle2_Pres, "Ox2PresXAxis")
    update_graph("LinePressurePlot", t, data.Line_Pres, "LinePresXAxis")
    update_graph("N2PressurePlot", t, data.N2Bottle_Pres, "N2PresXAxis")

# Tabel for temp channels + declaration of buffer
TIME_TEMP_BUFFER_SIZE = 300
TEMP_CHANNELS = [
    # name,               data_attr                 Tag
    ("Temp1",            "OxBottle1_temp",          "DeltaTempOx1PrintOut"),
    ("Temp2",            "OxBottle2_temp",          "DeltaTempOx2PrintOut"),
]
Temp_buffer = defaultdict(lambda: deque(maxlen=TIME_TEMP_BUFFER_SIZE))
time_buffer = deque(maxlen=TIME_TEMP_BUFFER_SIZE)

# updating data displays
def update_all_data_displays(data, t):
    # updating the data displays, could be done with a similar method to temp channels but might be unneded
    update_display("data_display:_ox._bottle_1_pressure:", data.OxBottle1_Pres)
    update_display("data_display:_ox._bottle_2_pressure:", data.OxBottle2_Pres)
    update_display("data_display:_line_pressure:", data.Line_Pres)
    update_display("data_display:_nitrogen_pressure:", data.N2Bottle_Pres)
    update_display("data_display:_chamber_pressure:", data.Chamber_Pres)
    update_display("data_display:_plume_temperature:", data.IRsensor)
    update_display("data_display:_nozzle_temperature:", data.NozzleTemp)
    update_display("data_display:_load_cell:", data.LoadCell)
    update_display("data_display:_mega_time:", t)

    update_display("Ox1TempPrintout", data.OxBottle1_temp)
    update_display("Ox2TempPrintout", data.OxBottle2_temp)
    update_display("AssumedPressureOx1PrintOut", assumed_pressure(data.OxBottle1_temp))
    update_display("AssumedPressureOx2PrintOut", assumed_pressure(data.OxBottle2_temp))

    # appending values to the temp and time buffer
    time_buffer.append(t)
    for name, attr,_ in TEMP_CHANNELS:
        Temp_buffer[name].append(getattr(data, attr))
    
    # updating the data display for temperature change
    for name,_, tag in TEMP_CHANNELS:
        if len(Temp_buffer[name]) >= 30:
            update_display(tag, temperature_change(Temp_buffer[name], time_buffer))

# Updating all the relays indivdually using the update relay helper
def update_all_relays(data, t):

    update_relay("HeatingRelay1StateText", "HeatingRelay1StatusCircle", data.OxBottle1_Relay)
    update_relay("HeatingRelay2StateText", "HeatingRelay2StatusCircle", data.OxBottle2_Relay)
    update_relay("OxRelay1StateText", "OxRelay1StatusCircle", data.Ox1Valve_Realy)
    update_relay("OxRelay2StateText", "OxRelay2StatusCircle", data.Ox1Valve_Realy)
    update_relay("IgnRelayStateText", "IgnRelayStatusCircle", data.IgnRelay)

# Logs all new messages in the SW log, avoids null messages (Index = 0)
def update_all_logs(data, t):
    if data.MsgIndex == 0:
        return
    else:
        update_log("software_log", data.message + str(t))



# Update function for the main loop, ensures we update all data within the data display and then mark the cache as stale
reader = TelemetryReader("data.csv", INDEX, messageStrings)

def update_frame():
    # Ensures we dont run updatre function without having data
    data = reader.get_next_data()
    if not data:
        return
    t = data.MegaTime * 1e-6 #Converting to seconds
    try: 
        update_piping_diagram(data, t)
        update_all_plots(data, t)
        update_all_data_displays(data, t)
        update_all_relays(data, t)
        update_all_logs(data, t)
    finally:
        reader.invalidate_cache()

