import matplotlib as mpl
import matplotlib.style as mplstyle
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.gridspec as gridspec
import math
import os
import csv
import time

# Trying out different backends
mpl.use("qtagg")

# Hide the figure toolbar
plt.rcParams['toolbar'] = 'None'

# load column information from text file
# THE SCRIPT WILL ONLY PLOT COLUMNS THAT ARE LISTED IN THIS FILE
column_info = {}
with open('titles.txt') as f:
    for line in f:
        col_num, info_str = line.split(':')
        title, xlabel, ylabel, unit, ymax, ymin, warn = info_str.strip().split(',')
        
        if warn == "*": warn = None
        else:           warn = float(warn)
        
        if "deg" in unit: unit = unit.replace("deg", "°")
        
        column_info[int(col_num.strip())] = {'title': title, 'xlabel': xlabel, 'ylabel': ylabel, 'unit': unit, 'ymax': int(ymax), 'ymin': int(ymin), 'warn': warn}

# Set suitable nrows and ncols
entries = len(column_info)
max_vert = 4
ndatacols = math.ceil(entries/max_vert)
nrows = math.ceil(entries/ndatacols)
ninfo = 2
#Total amount of columns
ncols = ndatacols + ninfo

# Define the widths of the columns
column_widths = [4] * ndatacols + [1.2] * ninfo # Adjust the values as desired

# Create the subplots
fig, axs = plt.subplots(nrows, ncols, figsize=(18, 10))
gs = gridspec.GridSpec(nrows, ncols, width_ratios=column_widths,
                       left=0.0125, bottom=0.025, right=0.995, top=0.975,
                       wspace = 0.05, hspace = 0.125)

# Prevent automatically hiding overlapping subplots
plt.clf()   

# Visual settings
fig.patch.set_color("darkgray")
plt.autoscale(False)
mplstyle.use('fast')

"""
mpl.rcParams['path.simplify'] = True
mpl.rcParams['path.simplify_threshold'] = 1
mpl.rcParams['agg.path.chunksize'] = 10
"""

#Mode and substate names
modes = ["INIT",
         "TEST",
         "WAIT",
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

# Is blitting used
useBlitting = True

# How long average is used for the info boxes
infoBoxAverageCount = 10
msPerPoint = 16.0
update_rate = 0 #ms, as fast as possible, timing is done with sleep()
# Total width of plot in seconds
plotTime = 10
data_points = math.ceil(plotTime * 1000 / msPerPoint)
# How many ticks in axis
tickCount = 6
# Corresponding csv positions
dataIndices = [col_num for i, col_num in enumerate(column_info.keys())]
warningLimits = [column_info[key]['warn'] for key in dataIndices]
warningMargin = 0.8

chamberPressure_Ind = dataIndices[0]
bottlePressure_Ind = dataIndices[1]
linePressure_Ind = dataIndices[2]
bottleTemperature_Ind = dataIndices[3]
nitrogenPressure_Ind = dataIndices[4]
loadCell_Ind = dataIndices[5]
nozzleTemperature_Ind = dataIndices[6]
plumeTemperature_Ind = dataIndices[7]

# Set up the data list
csvDataCount = 23
data = []
for i in range(csvDataCount):
    # Set the initial values
    data.append([0] * data_points)
    
#How much the plots are smoothed, 0 to <1, higher has more smoothing
smoothingFactor = 0.5

# Global time variable to track loop length
loopTime = 0

# Estimating the pointer position in a long file
lineLengthEstimate = 150
last_pos = 0
if os.path.getsize('data.csv') >= data_points * lineLengthEstimate:
    with open('data.csv', 'rb') as csv_file:
        last_pos = csv_file.seek(-data_points * lineLengthEstimate, 2)
        csv_file.readline()
        last_pos = csv_file.tell()

def msToTime(ms):
    s = ms/1000   
    hours = int(s // 3600)
    s -= hours * 3600
    minutes = int(s // 60)
    s -= minutes * 60
    seconds = int(s // 1)
    timeString = ""
    if hours > 0:
        timeString += "{:d}h ".format(hours)
    if minutes > 0:
        timeString += "{:d}m ".format(minutes)

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
            if float(self.value) < warningLimits[self.listInd] * warningMargin:
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
bottlePressureInfo = infoBox("Bottle pressure:", " Bar", (0, ncols-2), bottlePressure_Ind)
bottleTempInfo = infoBox("Bottle temperature:", " °C", (1, ncols-2), bottleTemperature_Ind)
loadCellInfo = infoBox("Load cell:", " N", (2, ncols-2), loadCell_Ind)
swInfo = infoBox("Software mode:\n\n\nSoftware substate:\n\n\nArduino time", "", (3, ncols-2), None, fontSize = 20)

maxMessageCount = 19
messageList = []
messageInfo = infoBox("", "", (3, ncols-1), None, fontSize = 8)

# Add into list and set up all
infoBoxes = [bottlePressureInfo, bottleTempInfo, loadCellInfo, swInfo]
infoTexts = []
for box in infoBoxes:
    ax = plt.subplot(gs[box.pos[0], box.pos[1]])
    ax.set_title(box.title, y = 1.0, pad = -25)
    box.text = ax.text(0.5, 0.48, str(box.value) + box.unit, ha='center',
                       va='center', fontsize = box.fontsize, fontweight='bold')
    infoTexts.append(box.text)

#Add and fine tune message info box
infoBoxes.append(messageInfo)
ax = plt.subplot(gs[messageInfo.pos[0], messageInfo.pos[1]])
ax.set_title(messageInfo.title, y = 1.0, pad = -25)
messageInfo.text = ax.text(0.02, 0.98, str(messageInfo.value) + messageInfo.unit, ha='left',
                           va='top', fontsize = messageInfo.fontsize)#, fontweight='bold')
infoTexts.append(messageInfo.text)


# Fine tune Software Mode & substate infobox
ax = plt.subplot(gs[swInfo.pos[0], swInfo.pos[1]])
ax.set_title(swInfo.title, y = 0.45, pad = -25)
swInfo.text.set_position((0.5, 0.45))

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

# Make an iterable of plots and update the info columns
lines = []
dataTexts = []
dataIndex = 0
for i in range(nrows):
    for j in range(ncols):
        ax = plt.subplot(gs[i, j])
        if j < ncols - ninfo: 
            ax.set_ylim([column_info[dataIndices[dataIndex]]["ymin"], 
                         column_info[dataIndices[dataIndex]]["ymax"]])
            ax.set_title(column_info[dataIndices[dataIndex]]["title"])
            
            paddingAmount = (len(str(column_info[dataIndices[dataIndex]]["ymax"]))-2) * (-1) + 4
            ax.set_ylabel(column_info[dataIndices[dataIndex]]["ylabel"] + " (" + column_info[dataIndices[dataIndex]]["unit"] + ")" , labelpad = paddingAmount)

            ax.set_xlim([0, data_points * (msPerPoint/1000)])
            line, = ax.plot([], [], animated=useBlitting)
            
            line.set_xdata([i * (msPerPoint/1000) for i in range(data_points)])
            
            labels = ["+" + "{:d}".format(int((data_points - i) * (msPerPoint/1000)))
                      for i in range(0, data_points+1, math.floor(data_points/(tickCount-1)))]
            
            ax.set_xticklabels(labels)
                        
            ax.tick_params(axis="y",direction="in", pad=-30, grid_color = "lightgray", labelsize = 12)
            ax.tick_params(axis="x", grid_color = "lightgray")
            
            line.set_ydata([0] * data_points)
            
            lines.append(line)
            ax.grid(True)
            if warningLimits[dataIndex] != None:
                plt.axhline(y = warningLimits[dataIndex], color = 'r', linestyle = ':')
            
            #ax.legend()
            dataText = ax.text(0.94 * data_points * (msPerPoint/1000),
                               0.93 * (column_info[dataIndices[dataIndex]]["ymax"] - column_info[dataIndices[dataIndex]]["ymin"]),
                               str(box.value) + box.unit, ha='center', va='center', fontsize = dataTextFont, fontweight='bold')
           
            dataTexts.append(dataText)
            
            dataIndex += 1
            
        if j >= ncols - ninfo:
            ax.get_xaxis().set_visible(False)
            ax.get_yaxis().set_visible(False)
            if i < nrows - 1: 
                ax.axis('off')
                ax.set_aspect('equal')  # Set the aspect ratio to 'equal'
            else:
                ax.patch.set_color("gray")
            
        elif i < nrows - 1:
            ax.set_xticklabels([]) #ax.get_xaxis().set_visible(False)
            
        #else: timePlots.append(ax)


def update(frame):
    global last_pos
    global loopTime
    global lines
    global data
    global messageList
    messageListUpdated = False
    
    #This seemed to cause performance issues
    """
    # get the size of the file
    file_size = os.path.getsize('data.csv')
    # if the file hasn't grown since last time, do nothing
    if file_size <= last_pos:
        #for line in lines:
            #line.set_xdata([])
            #line.set_ydata([])
        #return [] #lines + infoTexts + ledCircles
    """
    # otherwise, read the new data from the file
    with open('data.csv', 'r') as csv_file:
        csv_reader = csv.reader(csv_file)
        csv_file.seek(last_pos)
        for row in csv_reader:
            #print(row)
            #If it is a data line
            #Rock timestamp = index 0
            #Data indicator = index 1
            if len(row) > 1 and row[1] == "d":
                #Remove the "d"
                #row = [row[0]] + row[2:]
                #Continue noramlly
                for i, column in enumerate(row):
                    try: val = float(column)
                    except: val = column
                    #Only smooth sensor data, not time or binary values
                    if 3 <= i <= 11:
                        data[i] += [smoothingFactor * data[i][-1] + (1-smoothingFactor) * val]
                    else:
                        data[i] += [val]
            #Else it is a message line --> Print out
            #else:
            if row[22] != " ":
                #print(messageList)
                messageListUpdated = True
                splitRow = row[22].split("\\n")
                for newRow in splitRow:
                    messageList += [newRow]
                messageList = messageList[-maxMessageCount:]
                                
        last_pos = csv_file.tell()  
        
    for i in range(csvDataCount):
        data[i] = data[i][-data_points:]
                
    dataIndex = 0
    for i in range(nrows):
        for j in range(ncols):
            if j < ncols - ninfo:
                lines[dataIndex].set_ydata(data[dataIndices[dataIndex]])
                
                roundedValue = sum(data[dataIndices[dataIndex]][-infoBoxAverageCount:]) / infoBoxAverageCount
                
                textOffset = 0.1 * (column_info[dataIndices[dataIndex]]["ymax"] - column_info[dataIndices[dataIndex]]["ymin"])
                
                if roundedValue < column_info[dataIndices[dataIndex]]["ymin"]:
                   roundedValue = column_info[dataIndices[dataIndex]]["ymin"] 
                elif roundedValue + textOffset > column_info[dataIndices[dataIndex]]["ymax"]:
                    roundedValue = column_info[dataIndices[dataIndex]]["ymax"] - textOffset
                    
                dataTexts[dataIndex].set_text("{:3.1f} {:s}".format(roundedValue, column_info[dataIndices[dataIndex]]["unit"]))
                dataTexts[dataIndex].set_position((0.94 * data_points * (msPerPoint/1000), textOffset + roundedValue))
                
                dataIndex += 1
    
    # Get states from csv after updating the list
    # Use last value from the list
    h_state = int(data[14][-1])  # CSV column 14
    v_state = int(data[19][-1])  # CSV column 19
    ign_state = int(data[18][-1]) # CSV column 18
    
    heatingIndicator.set_state(h_state)
    valveIndicator.set_state(v_state)
    ignitionRelayIndicator.set_state(ign_state)
        
    # Get values from csv after updating the list
    # Calculate a rolling average with sum(data[column_number][-infoBoxAverageCount:]) / infoBoxAverageCount 
    # Smooths the data making it easier to read    
    bp_value = "{:4.1f}".format(sum(data[bottlePressure_Ind][-infoBoxAverageCount:]) / infoBoxAverageCount) #CSV column 3
    bt_value = "{:4.1f}".format(sum(data[bottleTemperature_Ind][-infoBoxAverageCount:]) / infoBoxAverageCount) #CSV column 7
    lc_value = "{:4.1f}".format(sum(data[loadCell_Ind][-infoBoxAverageCount:]) / infoBoxAverageCount) #CSV column 6
    
    # No smoothing for mode/substate strings or time
    # To get string use modes[data[19][-1]] & substates[data[20][-1]]
    sw_value = modes[int(data[20][-1])] + "\n\n" + substates[int(data[21][-1])] + "\n\n" + msToTime(int(data[2][-1]))  #CSV columns 20 & 21 & 2 respectively
    
    if messageListUpdated:
        mi_value = ""
        for message in messageList:
            message = message.replace("\\n", "\n")
            mi_value += message
            mi_value += "\n"
        messageInfo.set_value(mi_value)
     
    bottlePressureInfo.set_value(bp_value)
    bottleTempInfo.set_value(bt_value)
    loadCellInfo.set_value(lc_value) 
    swInfo.set_value(sw_value)
    
    waitTime = 0 #1.0/60 - (time.time() - loopTime)

    if waitTime > 0:
        time.sleep(waitTime)
    
    loopTime = time.time()
    
    return lines + infoTexts + ledCircles + dataTexts

# Make the window fullscreen
manager = plt.get_current_fig_manager()
manager.full_screen_toggle()


# call animate() function periodically
ani = animation.FuncAnimation(fig, update, interval=update_rate, 
                    blit=useBlitting, cache_frame_data=False)
#plt.tight_layout() # Remove unnecessary whitespace

plt.show()
