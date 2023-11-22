import matplotlib as mpl
import matplotlib.style as mplstyle
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import matplotlib.gridspec as gridspec
import matplotlib.image as mpimg

import time

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
fig.patch.set_color("darkgray")
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
dataTextFont = 12

# How long average is used for the info boxes
infoBoxAverageCount = 20
msPerPoint = 16.0
plotTime = 10
datapoints = math.ceil(plotTime * 1000 / msPerPoint)
tickCount = 6

bp_Ind = 2
lp_Ind = 3
cp_Ind = 4
lc_Ind = 5
bt_Ind = 6
nt_INd = 8
at_Ind = 9
ir_Ind = 10

ac_Ind = 1

dataIndices = [cp_Ind, bp_Ind, 
               lp_Ind, bt_Ind, 
               nt_INd, lc_Ind, 
               at_Ind, ir_Ind]
               
dataLimits = [15, 10, 
              10, 25, 
              25, 100, 
              25, 300]
              
warningLimits = [12, 6,
                 None, 20,
                 20,None,
                 None,None]
                 
dataTitles = ["Chamber Pressure", "Bottle Pressure",
              "Line Pressure", "Bottle Temperature",
              "Nozzle Temperature", "Load Cell",
              "Ambient temperature", "Plume Temperature"]
              
smoothingFactor = 0.5


def msToTime(ms):
    s = ms/1000 
    hours = int(s // 3600)
    s -= hours * 3600
    minutes = int(s // 60)
    s -= minutes * 60
    seconds = int(s // 1)
    
    timeString = ""
    if hours > 0: timeString += "{:d}h \n".format(hours)
    if minutes > 0: timeString += "{:d}m ".format(minutes)
    
    return timeString + "{:d}s".format(seconds)
    
    
class indicatorLED:
    def __init__(self, text, pos):
        self.outline = plt.Circle((0.5, 0.5), 0.49, color="black")
        self.outer = plt.Circle((0.5, 0.5), 0.48, color=ledColors[0][0])
        self.highlight = plt.Circle((0.5, 0.5), 0.44, color=ledColors[0][1])
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
    def __init__(self, title, unit, pos, dataInd, fontSize = infoBoxFont, formatFunc = None):
        self.title = title
        self.unit = unit
        self.value = 0
        self.text = None
        self.pos = pos
        self.dataInd = dataInd
        # Testing
        self.listInd = None
        if self.dataInd: self.listInd = dataIndices.index(self.dataInd)
        #
        self.fontsize = fontSize
        self.formatFunc = formatFunc
    
    def set_value(self, value):
        self.value = value
        if self.listInd and warningLimits[self.listInd]:
            if float(self.value) < warningLimits[self.listInd] * 0.75:
                self.text.set_color('darkgreen')
            elif float(self.value) < warningLimits[self.listInd]:
                self.text.set_color('darkorange')
            elif float(self.value) >= warningLimits[self.listInd]:
                self.text.set_color('red')
                
        if self.formatFunc == None:
            self.text.set_text(self.value + self.unit)
        else:
            self.text.set_text(self.formatFunc(value) + self.unit)


# Create named info boxes
bottlePressureInfo = infoBox("Bottle pressure:", " Bar", (0, ncols-2), bp_Ind)
bottleTempInfo = infoBox("Bottle temperature:", " °C", (1, ncols-2), bt_Ind)
loadCellInfo = infoBox("Load cell:", " N", (2, ncols-2), lc_Ind)
swInfo = infoBox("Software mode:", "", (3, ncols-2), None, fontSize = 24)
testTimeInfo = infoBox("Arduino time:", "", (3, ncols-1), at_Ind, fontSize = 24, formatFunc = msToTime)

# Add into list and set up all
infoBoxes = [bottlePressureInfo, bottleTempInfo, loadCellInfo, swInfo, testTimeInfo]
infoTexts = []
for box in infoBoxes:
    ax = plt.subplot(gs[box.pos[0], box.pos[1]])
    ax.set_title(box.title, y = 1.0, pad = -25)
    box.text = ax.text(0.5, 0.48, str(box.value) + box.unit, ha='center',
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
    ax.add_artist(led.outline)
    ax.add_artist(led.outer)
    ax.add_artist(led.highlight)
    ax.add_artist(led.inner)
    ledCircles.append(led.outline)
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
dataTexts = []
#timePlots = []
dataIndex = 0
for i in range(nrows):
    for j in range(ncols):
        ax = plt.subplot(gs[i, j])
        """ """
        # FROM TESTING 
        if j < ncols - infoColumns: 
            ax.set_ylim([-1, dataLimits[dataIndex]])
            ax.set_title(dataTitles[dataIndex])

            ax.set_xlim([0, (datapoints-1) * (msPerPoint/1000)])
            """ """
            line, = ax.plot([], [], animated=True)
            line.set_xdata([i * (msPerPoint/1000) for i in range(datapoints)])
            
            labels = ["+" + "{:d}".format(int((datapoints - i) * (msPerPoint/1000)))
                      for i in range(0, datapoints+1, math.floor(datapoints/(tickCount-1)))]
            ax.set_xticklabels(labels)
            line.set_ydata([0] * datapoints)
            
            lines.append(line)
            ax.grid(True)
            if warningLimits[dataIndex] != None:
                plt.axhline(y = warningLimits[dataIndex], color = 'r', linestyle = ':')
            
            #ax.legend()
            dataText = ax.text(0.95 * datapoints * (msPerPoint/1000), 0.93 * dataLimits[dataIndex], str(box.value) + box.unit, ha='center',
                               va='center', fontsize = dataTextFont, fontweight='bold')#, backgroundcolor = "lightgray")
           
            dataTexts.append(dataText)
            
            dataIndex += 1
            
        if j >= ncols - infoColumns:
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
            if i < nrows - 1: 
                ax.axis('off')
                ax.set_aspect('equal')  # Set the aspect ratio to 'equal'
            else:
                ax.patch.set_color("gray")

            ax.set_xlim(0, 1)  # Adjust the x-axis limits for padding
            ax.set_ylim(0, 1)  # Adjust the y-axis limits for padding
            
        elif i < nrows - 1:
            ax.set_xticklabels([]) #
            
        #else: timePlots.append(ax)

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
    datacolumns = next(csv_reader)#[0].rsplit(",")
    for k, column in enumerate(datacolumns):
        if k > 0:
            data[k].append(smoothingFactor * data[k][-1] + (1-smoothingFactor) * float(column))
        else:
            data[k].append(0)
""" """

"""
ADD THIS STUFF TO UPDATE FUNCTION IN LIVE_GRAPHER
start
"""

loopTime = 0

def update(frame):    
    
    startTime = time.time()
    global loopTime

    """ """
    # FROM TESTING
    #for k in range(1):
    datacolumns = next(csv_reader)#[0].rsplit(",")
    for i, column in enumerate(datacolumns):
        if i in dataIndices:
            data[i].append(smoothingFactor * data[i][-1] + (1-smoothingFactor) * float(column))
        elif i > 0:
            data[i].append(float(column))
       
    #for i, column in enumerate(datacolumns):
        data[i] = data[i][-datapoints:]
    
    #timespanZero = [(i - data[1][0]) / 1000 for i in data[1]]
    #timespan = [i / 1000 for i in data[1]]

    dataIndex = 0
    for i in range(nrows):
        for j in range(ncols):
            if j < ncols - infoColumns:
                lines[dataIndex].set_ydata(data[dataIndices[dataIndex]])
                roundedValue = sum(data[dataIndices[dataIndex]][-infoBoxAverageCount:]) / infoBoxAverageCount
                textOffset = 0.1 * dataLimits[dataIndex]
                dataTexts[dataIndex].set_text("{:3.1f}".format(roundedValue))
                dataTexts[dataIndex].set_position((0.95 * datapoints * (msPerPoint/1000), textOffset + roundedValue))
                
                
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
    #ib_state = int(data[13][-1]) # CSV column 13
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
    
    #print(time.time() - startTime)
    #time.sleep(0.0166667 - (time.time() - startTime))
    
    waitTime = 1.0/60 - (time.time() - loopTime)
    if waitTime > 0:
        time.sleep(waitTime)
        
    #print(time.time() - loopTime)
    loopTime = time.time()
    return lines + infoTexts + ledCircles + dataTexts
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