import dearpygui.dearpygui as dpg
import numpy as np
from scipy.interpolate import interp1d

# Updates data displays
def update_display(tag, new_value):
    # Checks if the tag exists, if it does we update said tag with a new value
    if dpg.does_item_exist(tag):
        dpg.set_value(tag, f"{new_value:.2f}") 
    else:
        print(f"[Warning] Tried to update missing item: {tag}")
        
# Updates graph data
def update_graph(tag, new_x, new_y, x_axis_tag=None, window_seconds=5.0):
    # Checks if the specified object does not have a specified attribute  
    if not hasattr(update_graph, "data_buffers"):
        # Appending a buffer to the function, works similar to a gloabal variable
        update_graph.data_buffers = {}

    # Check if I have a place to store data for a specific tag
    if tag not in update_graph.data_buffers:
        # Creating a place to store data for a given tag
        update_graph.data_buffers[tag] = {"x": [], "y": []}

    # Appending data to the buffer
    buf = update_graph.data_buffers[tag]
    buf["x"].append(new_x)
    buf["y"].append(new_y)

    # Only keep data within the last window_seconds
    cutoff_time = new_x - window_seconds
    while buf["x"] and buf["x"][0] < cutoff_time:
        buf["x"].pop(0)
        buf["y"].pop(0)

    # Update the plot
    dpg.set_value(tag, [buf["x"], buf["y"]])

    # Update X-axis limits to match the window_seconds
    if x_axis_tag and dpg.does_item_exist(x_axis_tag):
        dpg.set_axis_limits(x_axis_tag, new_x - window_seconds, new_x)



# Update the relay data
def update_relay(texttag, buttontag, new_data):
    # Validating that the button we are trying to update exsists, so button and corresponding text exists
    if dpg.does_item_exist(texttag) and dpg.does_item_exist(buttontag):
        # Updating based on the sate of the button (so on and off)
        if new_data == 0:
            dpg.set_value(texttag, "OFF")
            dpg.configure_item(buttontag, color=(255, 0, 0, 255), fill=(255, 0, 0, 255))
        elif new_data == 1:
            dpg.set_value(texttag, "ON")
            dpg.configure_item(buttontag, color=(0, 255, 0, 255), fill=(0, 255, 0, 255))
        else:
            print(f"Invalid data for {texttag}")
    else:
        print(f"[Warning] Tried to update missing items: {texttag}, {buttontag}")



# Piping visuals update function
def update_pressure_line(status, line_tag, label):
    # Creating a dict for the color map
    color_map = {
        "critical": (255, 0, 0, 255),
        "warn":     (255, 165, 0, 255),
        "ok":       (128, 128, 128, 255)
    }
    # Creating a dict for the messages 
    messages = {
        "critical": f"{label.upper()} CRITICAL ERROR: Leak detected.\n",
        "warn":     f"{label.upper()} WARNING: Possible leak detected.\n",
        "ok":       ""
    }
    # Updating color and message based on given status
    dpg.configure_item(line_tag, color=color_map[status])
    return messages[status]

# Updating the data log
def update_log(tag, new_data):
    # Obtaining the old messages
    prev_log = dpg.get_value(tag)
    # Adding the new message to the old messages (ensures we dont erase old messages)
    new_log = prev_log + str(new_data) + "\n"
    # Updating the log
    dpg.set_value(tag, new_log)





def assumed_pressure(tempdata):
    # Data from airliquid
    temperature_k = np.array([
    182.33, 182.65, 183.15, 183.65, 184.15, 184.65, 185.15, 185.65, 186.15, 186.65,
    187.15, 187.65, 188.15, 188.65, 189.15, 189.65, 190.15, 190.65, 191.15, 191.65,
    192.15, 192.65, 193.15, 193.65, 194.15, 194.65, 195.15, 195.65, 196.15, 196.65,
    197.15, 197.65, 198.15, 198.65, 199.15, 199.65, 200.15, 200.65, 201.15, 201.65,
    202.15, 202.65, 203.15, 203.65, 204.15, 204.65, 205.15, 205.65, 206.15, 206.65,
    207.15, 207.65, 208.15, 208.65, 209.15, 209.65, 210.15, 210.65, 211.15, 211.65,
    212.15, 212.65, 213.15, 213.65, 214.15, 214.65, 215.15, 215.65, 216.15, 216.65,
    217.15, 217.65, 218.15, 218.65, 219.15, 219.65, 220.15, 220.65, 221.15, 221.65,
    222.15, 222.65, 223.15, 223.65, 224.15, 224.65, 225.15, 225.65, 226.15, 226.65,
    227.15, 227.65, 228.15, 228.65, 229.15, 229.65, 230.15, 230.65, 231.15, 231.65,
    232.15, 232.65, 233.15, 233.65, 234.15, 234.65, 235.15, 235.65, 236.15, 236.65,
    237.15, 237.65, 238.15, 238.65, 239.15, 239.65, 240.15, 240.65, 241.15, 241.65,
    242.15, 242.65, 243.15, 243.65, 244.15, 244.65, 245.15, 245.65, 246.15, 246.65,
    247.15, 247.65, 248.15, 248.65, 249.15, 249.65, 250.15, 250.65, 251.15, 251.65,
    252.15, 252.65, 253.15, 253.65, 254.15, 254.65, 255.15, 255.65, 256.15, 256.65,
    257.15, 257.65, 258.15, 258.65, 259.15, 259.65, 260.15, 260.65, 261.15, 261.65,
    262.15, 262.65, 263.15, 263.65, 264.15, 264.65, 265.15, 265.65, 266.15, 266.65,
    267.15, 267.65, 268.15, 268.65, 269.15, 269.65, 270.15, 270.65, 271.15, 271.65,
    272.15, 272.65, 273.15, 273.65, 274.15, 274.65, 275.15, 275.65, 276.15, 276.65,
    277.15, 277.65, 278.15, 278.65, 279.15, 279.65, 280.15, 280.65, 281.15, 281.65,
    282.15, 282.65, 283.15, 283.65, 284.15, 284.65, 285.15, 285.65, 286.15, 286.65,
    287.15, 287.65, 288.15, 288.65, 289.15, 289.65, 290.15, 290.65, 291.15, 291.65,
    292.15, 292.65, 293.15, 293.65, 294.15, 294.65, 295.15, 295.65, 296.15, 296.65,
    297.15, 297.65, 298.15, 298.65, 299.15, 299.65, 300.15, 300.65, 301.15, 301.65,
    302.15, 302.65, 303.15, 303.65, 304.15, 304.65, 305.15, 305.65, 306.15, 306.65,
    307.15, 307.65, 308.15, 308.65, 309.15, 309.52
    ])

    pressure_bar = np.array([
    0.8785, 0.8958, 0.9236, 0.9521, 0.9813, 1.011, 1.042, 1.073, 1.105, 1.138,
    1.172, 1.206, 1.241, 1.277, 1.314, 1.351, 1.39, 1.429, 1.469, 1.51,
    1.552, 1.595, 1.639, 1.683, 1.729, 1.775, 1.823, 1.871, 1.921, 1.971,
    2.023, 2.075, 2.129, 2.184, 2.239, 2.296, 2.354, 2.413, 2.474, 2.535,
    2.597, 2.661, 2.726, 2.792, 2.859, 2.928, 2.998, 3.069, 3.141, 3.215,
    3.29, 3.366, 3.444, 3.523, 3.603, 3.685, 3.768, 3.853, 3.939, 4.026,
    4.115, 4.206, 4.298, 4.391, 4.486, 4.582, 4.681, 4.78, 4.881, 4.984,
    5.089, 5.195, 5.303, 5.412, 5.523, 5.636, 5.751, 5.867, 5.985, 6.105,
    6.226, 6.35, 6.475, 6.602, 6.731, 6.862, 6.994, 7.129, 7.266, 7.404,
    7.544, 7.687, 7.831, 7.977, 8.126, 8.276, 8.429, 8.583, 8.74, 8.899,
    9.06, 9.223, 9.388, 9.555, 9.725, 9.897, 10.07, 10.25, 10.43, 10.61,
    10.79, 10.97, 11.16, 11.35, 11.55, 11.74, 11.94, 12.14, 12.34, 12.54,
    12.75, 12.96, 13.17, 13.39, 13.6, 13.82, 14.05, 14.27, 14.5, 14.73,
    14.96, 15.2, 15.44, 15.68, 15.92, 16.17, 16.42, 16.67, 16.92, 17.18,
    17.44, 17.7, 17.97, 18.24, 18.51, 18.79, 19.07, 19.35, 19.63, 19.92,
    20.21, 20.5, 20.8, 21.1, 21.4, 21.7, 22.01, 22.33, 22.64, 22.96,
    23.28, 23.61, 23.93, 24.27, 24.6, 24.94, 25.28, 25.62, 25.97, 26.32,
    26.68, 27.04, 27.4, 27.77, 28.14, 28.51, 28.89, 29.27, 29.65, 30.04,
    30.43, 30.82, 31.22, 31.62, 32.03, 32.44, 32.85, 33.27, 33.69, 34.12,
    34.55, 34.98, 35.42, 35.86, 36.3, 36.75, 37.2, 37.66, 38.12, 38.59,
    39.06, 39.53, 40.01, 40.49, 40.98, 41.47, 41.97, 42.47, 42.97, 43.48,
    44.0, 44.51, 45.04, 45.56, 46.1, 46.63, 47.17, 47.72, 48.27, 48.83,
    49.39, 49.95, 50.53, 51.1, 51.68, 52.27, 52.86, 53.46, 54.06, 54.66,
    55.28, 55.89, 56.52, 57.15, 57.78, 58.42, 59.07, 59.72, 60.38, 61.04,
    61.71, 62.39, 63.08, 63.77, 64.46, 65.17, 65.88, 66.6, 67.32, 68.06,
    68.8, 69.55, 70.31, 71.08, 71.86, 72.45
    ])

    #interpolate between temperature and pressure

    n2o_pressure = interp1d(temperature_k, pressure_bar, kind='linear', bounds_error=False, fill_value='extrapolate')

    temp_k = tempdata + 273.15

    # Calculating pressure based on temp data
    pressure = n2o_pressure(temp_k)

    return pressure

# Function to calculate delta T
def temperature_change(timebuffer, tempbuffer):
    # Ensures we have more then 2 data point, if not we know the change is 0
    if len(timebuffer) < 2:
        return 0.0

    # Convert to numpy arrays
    t = np.array(timebuffer)
    y = np.array(tempbuffer)

    # Linear fit: slope = daöltaT/deltat, it is basically the slope of a first deg function
    slope, _ = np.polyfit(t, y, 1)
    return slope

# Function to calculate the loss of pressure
def pressure_drop(timebuffer, pressurebuffer):
    # Ensures we have more then 2 data point, if not we know the change is 0
    if len(timebuffer) < 2:
        return 0.0
    
    # Convert to numpy arrays
    t = np.array(timebuffer)
    p = np.array(pressurebuffer)

    # Linear fit: slope = deltaP/deltat, it is basically the slope of a first deg function
    slope, _ = np.polyfit(t, p, 1)  # slope in bar/sec
    return slope
