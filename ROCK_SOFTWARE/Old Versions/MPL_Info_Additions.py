import matplotlib as mpl
import matplotlib.style as mplstyle
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import matplotlib.gridspec as gridspec
import matplotlib.image as mpimg

"""
NO NEED TO CARE OF THESE
start
"""
# Random for testing
import random
import csv
import math
  
# Hide the figure toolbar
plt.rcParams['toolbar'] = 'None'
""" end """



"""
INCORPORATE INTO LIVE_GRAPHER
start
"""
# Calculate the number of rows and columns for the subplot
nrows = 4
ncols = 4
infoColumns = 2

# Define the widths of the columns
column_widths = [4, 4, 1.2, 1.2]  # Adjust the values as desired

# Create the subplots
fig, axs = plt.subplots(nrows, ncols, figsize=(18, 10))
gs = gridspec.GridSpec(nrows, ncols, width_ratios=column_widths,
                       left=0.02, bottom=0.03, right=0.98, top=0.97,
                       wspace = 0.1, hspace = 0.1)

# Prevent automatically hiding overlapping subplots
plt.clf()   
plt.autoscale(False)
mplstyle.use('fast')
"""
mpl.rcParams['path.simplify'] = True
mpl.rcParams['path.simplify_threshold'] = 1
mpl.rcParams['agg.path.chunksize'] = 10
"""
""" end """



"""
THESE SHOULD BE ABLE TO JUST BE SLAPPED ON
start
"""
#Mode and substate names
modes = ["INIT",
         "TEST",
         "WAIT",
         "HEATING",
         "SEQUENCE",
         "SAFE",
         "SHUTDOWN"]
  
substates = ["ALL_OFF",
             "IGNIT_ON",
             "VALVE_ON",
             "IGNIT_OFF",
             "VALVE_OFF",
             "FINISHED"]

# Colors of the indicator LEDs
ledColors = [['dimgray', 'gray'], ['orangered', 'lightcoral']]

#Font sizes for the info panels
infoBoxFont = 32
indicatorFont = 20

# How long average is used for the info boxes
infoBoxAverageCount = 20
msPerPoint = 16.0
plotTime = 10
datapoints = math.ceil(plotTime * 1000 / msPerPoint)
tickCount = 6
dataIndices = [2,3,4,5,6,8,9,10]
dataLimits = [10, 10, 10, 100, 25, 25, 25, 300]
dataTitles = ["Bottle Pressure", "Line Pressure", "Chamber Pressure", "Load Cell", "Bottle Temperature", "Nozzle Temperature", "Bottle temperature", "Plume Temperature"]
smoothingFactor = 0.5


def msToTime(ms):
    s = ms/1000
    
    hours = int(s // 3600)
    s -= hours * 3600
    minutes = int(s // 60)
    s -= minutes * 60
    seconds = int(s // 1)
    
    timeString = ""
    if hours > 0:
        timeString += "{:d}h \n".format(hours)
    if minutes > 0:
        timeString += "{:d}m ".format(minutes)
    
    
    
    return timeString + "{:d}s".format(seconds)
    
    
class indicatorLED:
    def __init__(self, text, pos):
        self.outer = plt.Circle((0.5, 0.5), 0.49, color=ledColors[0][0])
        self.highlight = plt.Circle((0.5, 0.5), 0.45, color=ledColors[0][1])
        self.inner = plt.Circle((0.5, 0.5), 0.4, color=ledColors[0][0])
        self.text = text
        self.pos = pos
        self.state = 0
    
    def set_state(self, state):
        self.state = state
        self.outer.set_color(ledColors[state][0])
        self.highlight.set_color(ledColors[state][1])
        self.inner.set_color(ledColors[state][0])
    
    
class infoBox:
    def __init__(self, title, unit, pos, dataType, fontSize = infoBoxFont, formatFunc = None):
        self.title = title
        self.unit = unit
        self.value = 0
        self.text = None
        self.pos = pos
        self.dataType = dataType
        self.fontsize = fontSize
        self.formatFunc = formatFunc
    
    def set_value(self, value):
        self.value = value
        
        if self.formatFunc == None:
            self.text.set_text(self.value + " " + self.unit)
        else:
            self.text.set_text(self.formatFunc(value) + " " + self.unit)


# Create named info boxes
bottlePressureInfo = infoBox("Bottle pressure:", "Bar", (0, ncols-2), "f")
bottleTempInfo = infoBox("Bottle temperature:", "°C", (1, ncols-2), "f")
loadCellInfo = infoBox("Load cell:", "N", (2, ncols-2), "f")
swInfo = infoBox("Software mode:", "", (3, ncols-2), "s", fontSize = 24)
testTimeInfo = infoBox("Arduino time:", "", (3, ncols-1), "s", fontSize = 24, formatFunc = msToTime)

# Add into list and set up all
infoBoxes = [bottlePressureInfo, bottleTempInfo, loadCellInfo, swInfo, testTimeInfo]
infoTexts = []
for box in infoBoxes:
    ax = plt.subplot(gs[box.pos[0], box.pos[1]])
    ax.set_title(box.title, y = 1.0, pad = -25)
    box.text = ax.text(0.5, 0.48, str(box.value) + " " + box.unit, ha='center',
                       va='center', fontsize = box.fontsize, fontweight='bold')
    infoTexts.append(box.text)
    
# Create named indicator LEDs
heatingIndicator = indicatorLED("Heating\nBlanket", (0, ncols-1))
valveIndicator = indicatorLED("Main\nValve", (1, ncols-1))
#ignitionButtonIndicator = indicatorLED("Ignition\nButton", (2, ncols-1))
ignitionRelayIndicator = indicatorLED("Ignition\nRelay", (2, ncols-1))

# Add into list and set up all
indicators = [heatingIndicator, valveIndicator, ignitionRelayIndicator]#, ignitionButtonIndicator]
ledCircles = []

for led in indicators:
    ax = plt.subplot(gs[led.pos[0], led.pos[1]])
    ax.add_artist(led.outer)
    ax.add_artist(led.highlight)
    ax.add_artist(led.inner)
    ledCircles.append(led.outer)
    ledCircles.append(led.highlight)
    ledCircles.append(led.inner)
    led.text = ax.text(0.5, 0.48, led.text, ha='center', va='center', fontsize=indicatorFont, fontweight='bold')
    infoTexts.append(led.text)    
""" end """



"""
INCORPORATE INTO LIVE_GRAPHER
start
"""
# Make an iterable of plots and update the info columns
lines = []
timePlots = []
dataIndex = 0
for i in range(nrows):
    for j in range(ncols):
        ax = plt.subplot(gs[i, j])
        """ """
        # FROM TESTING 
        if j < ncols - infoColumns: 
            ax.set_ylim([-1, dataLimits[dataIndex]])
            ax.set_title(dataTitles[dataIndex])
            dataIndex += 1
            ax.set_xlim([0, datapoints * (msPerPoint/1000)])
            """ """
            line, = ax.plot([], [], animated=True)
            line.set_xdata([i * (msPerPoint/1000) for i in range(datapoints)])
            
            labels = ["+" + "{:d}".format(int((datapoints - i) * (msPerPoint/1000)))
                      for i in range(0, datapoints+1, math.floor(datapoints/(tickCount-1)))]
            ax.set_xticklabels(labels)
            line.set_ydata([0] * datapoints)
            
            lines.append(line)
            ax.grid()
            
        if j >= ncols - infoColumns:
            ax.axis('off')
            ax.set_aspect('equal')  # Set the aspect ratio to 'equal'
            
            ax.set_xlim(0, 1)  # Adjust the x-axis limits for padding
            ax.set_ylim(0, 1)  # Adjust the y-axis limits for padding
            
        elif i < nrows - 1:
            ax.set_xticklabels([]) #ax.get_xaxis().set_visible(False)
            
        else: timePlots.append(ax)

""" end """



""" """
# FROM TESTING
csv_file = open('data.csv', 'r')
csv_reader = csv.reader(csv_file)
data = []
for i in range(18):
    data.append([])
    data[-1].append(0)

for j in range(datapoints):
    datacolumns = next(csv_reader)[0].rsplit(";")
    for k, column in enumerate(datacolumns):
        if k > 0:
            data[k].append(smoothingFactor * data[k][-1] + (1-smoothingFactor) * float(column))
        else:
            data[k].append(0)
""" """

background = None

"""
ADD THIS STUFF TO UPDATE FUNCTION IN LIVE_GRAPHER
start
"""
def update(frame):    
    """ """
    # FROM TESTING
    #for k in range(1):
    datacolumns = next(csv_reader)[0].rsplit(";")
    for i, column in enumerate(datacolumns):
        if i > 0:
            data[i].append(smoothingFactor * data[i][-1] + (1-smoothingFactor) * float(column))
       
    for i, column in enumerate(datacolumns):
        data[i] = data[i][-datapoints:]
    
    #timespanZero = [(i - data[1][0]) / 1000 for i in data[1]]
    #timespan = [i / 1000 for i in data[1]]

    dataIndex = 0
    for i in range(nrows):
        for j in range(ncols):
            if j < ncols - infoColumns:
                lines[dataIndex].set_ydata(data[dataIndices[dataIndex]])

                #if i == nrows - 1:
                    #lines[dataIndex].set_xdata(timespan)
                    #plt.subplot(gs[i, j]).set_xlim([timespan[0], timespan[-1]])
                #else:
                    #lines[i*ncols + j].set_xdata(timespanZero)
                          
                dataIndex += 1
    """ """
               
    
    # Get states from csv after updating the list
    # Use last value from the list
    h_state = int(data[12][-1])  # CSV column 12
    v_state = int(data[15][-1])  # CSV column 15
    ib_state = int(data[13][-1]) # CSV column 13
    ir_state = int(data[14][-1]) # CSV column 14
    
    heatingIndicator.set_state(h_state)
    valveIndicator.set_state(v_state)
    #ignitionButtonIndicator.set_state(ib_state)
    ignitionRelayIndicator.set_state(ir_state)
        
    # Get values from csv after updating the list
    # Calculate a rolling average with sum(data[column_number][-infoBoxAverageCount:]) / infoBoxAverageCount 
    # Smooths the data making it easier to read    
    bp_value = "{:4.1f}".format(sum(data[2][-infoBoxAverageCount:]) / infoBoxAverageCount) #CSV column 2
    bt_value = "{:4.1f}".format(sum(data[6][-infoBoxAverageCount:]) / infoBoxAverageCount) #CSV column 6
    lc_value = "{:4.1f}".format(sum(data[5][-infoBoxAverageCount:]) / infoBoxAverageCount) #CSV column 5
    
    # No smoothing for mode/substate strings or time
    # To get string use modes[data[16][-1]] & substates[data[17][-1]]
    sw_value = modes[int(data[16][-1])] + "\n" + substates[int(data[17][-1])]  #CSV columns 16 & 17 respectively
    tt_value = int(data[1][-1])
    
    bottlePressureInfo.set_value(bp_value)
    bottleTempInfo.set_value(bt_value)
    loadCellInfo.set_value(lc_value) 
    swInfo.set_value(sw_value)
    testTimeInfo.set_value(tt_value)
    
    #for plot in timePlots:
        #plot.figure.canvas.blit(plot.bbox.expanded(1.1, 1.2))
        #plot.set_xticklabels([val for val in plot.get_xticks()])
        #plot.set_yticklabels([val for val in plot.get_yticks()])
    

    return lines + infoTexts + ledCircles# + timePlots
""" end """



"""
THESE SHOULD BE ABLE TO JUST BE SLAPPED ON
start
"""
# Make the window fullscreen
manager = plt.get_current_fig_manager()
manager.full_screen_toggle()
""" end """



"""
NO NEED TO CARE OF THESE
start
"""
# Create the animation
animation = FuncAnimation(fig, update, interval=0, blit = True, cache_frame_data=False)

# Display the animation
plt.show()
""" end """