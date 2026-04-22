from collections import deque, defaultdict
import math

# Collecting all logic values
class telemetry_processor: 
    def __init__(self, window_size = 5):
        self.window_size = window_size
        self.buffers = defaultdict(lambda: deque(maxlen = window_size))

        # calibrated value = raw data * gain + offset
        self.calibration = {
            "OxBottle1_Pres": {"gain": 1.0, "offset": 0.0},
            "OxBottle2_Pres": {"gain": 1.0, "offset": 0.0},
            "Line_Pres": {"gain": 1.0, "offset": 0.0},
            "Chamber_Pres": {"gain": 1.0, "offset": 0.0},
            "N2Bottle_Pres": {"gain": 1.0, "offset": 0.0},
            "OxBottle1_temp": {"gain": 1.0, "offset": 0.0},
            "OxBottle2_temp": {"gain": 1.0, "offset": 0.0},
            "IRsensor": {"gain": 1.0, "offset": 0.0},
            "NozzleTemp": {"gain": 1.0, "offset": 0.0},
            "engineTemp": {"gain": 1.0, "offset": 0.0},
            "LoadCell": {"gain": 1.0, "offset": 0.0},
        }

        # List of fields to filter
        self.fields_to_filter = {
            "OxBottle1_Pres",
            "OxBottle2_Pres",
            "Line_Pres",
            "Chamber_Pres",
            "N2Bottle_Pres",
            "OxBottle1_temp",
            "OxBottle2_temp",
            "IRsensor",
            "NozzleTemp",
            "engineTemp",
            "LoadCell"
        }


        # Function to find calibrated values
        def calibrate(self, field_name, value):
            if isinstance(value, float) and math.isnan(value):
                return value
            
            # Looking fo calibration settings
            params = self.calibration.get(field_name)
            if params is None:
                return value
            
            return value*params["gain"] + params["offset"]
        
        # Function to filter values
        def filter_value(self, field_name, value):
            if isinstance(value, float) and math.isnan(value):
                return value
        
            # Looking for fields that aren't supposed to be filtered
            if field_name not in self.fields_to_filter:
                return value
            
            # Mean values of the latest values in the buffer
            self.buffers[field_name].append(value)
            return sum(self.buffers[field_name])/len(self.buffers[field_name])
        
        # Main function
        def process(self, data):
            processed = {}

            for field_name, value in vars(data).items():
                if isinstance(value, (int, float)):
                    value = self.calibrate(field_name, value)
                    value = self.filter_value(field_name, value)

                    processed[field_name] = value

            return type(data)(**processed)
        
