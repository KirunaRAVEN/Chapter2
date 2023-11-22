import matplotlib as mpl
import matplotlib.style as mplstyle
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import matplotlib.gridspec as gridspec
import matplotlib.image as mpimg

# Hide the figure toolbar
plt.rcParams['toolbar'] = 'None'

# load column information from text file
# THE SCRIPT WILL ONLY PLOT COLUMNS THAT ARE LISTED IN THIS FILE
column_info = {}
with open('titles.txt') as f:
    for line in f:
        col_num, info_str = line.split(':')
        title, xlabel, ylabel, ymax, ymin = info_str.strip().split(',')
        column_info[int(col_num.strip())] = {'title': title, 'xlabel': xlabel, 'ylabel': ylabel, 'ymax': int(ymax), 'ymin': int(ymin)}

# Set suitable nrows and ncols
entries = len(column_info)
max_vert = 4
ncols = math.ceil(entries/max_vert)
nrows = math.ceil(entries/ncols)
infoColumns = 2

# Define the widths of the columns
column_widths = [4] * ncols + [1.2] * ninfo # Adjust the values as desired

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
# Total width of plot in seconds
plotTime = 10
datapoints = math.ceil(plotTime * 1000 / msPerPoint)
# How many ticks in axis
tickCount = 6
# Corresponding csv positions
dataIndices = [col_num for i, col_num in enumerate(column_info.keys())]

#How much the plots are smoothed, 0 to <1, higher has more smoothing
smoothingFactor = 0.5

lineLengthEstimate = 120

# Estimating the pointer position in a long file
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

# Make an iterable of plots and update the info columns
lines = []
timePlots = []
dataIndex = 0
for i in range(nrows):
    for j in range(ncols):
        ax = plt.subplot(gs[i, j])
        if j < ncols - infoColumns: 
            ax.set_ylim([column_info[dataIndices[dataIndex]]["ymin"], 
                         column_info[dataIndices[dataIndex]]["ymax"]])
            ax.set_title(column_info[dataIndices[dataIndex]]["title"]])
            dataIndex += 1
            ax.set_xlim([0, datapoints * (msPerPoint/1000)])
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


def update(frame):  

    global last_pos
    global data
    global timespan
    # get the size of the file
    file_size = os.path.getsize('data.csv')

    # if the file hasn't grown since last time, do nothing
    if file_size <= last_pos:
        for line in lines:
            line.set_xdata([])
            line.set_ydata([])
        return []
    
    # otherwise, read the new data from the file
    with open('data.csv', 'r') as csv_file:
        csv_reader = csv.reader(csv_file)
        csv_file.seek(last_pos)
        #updated = False
        for row in csv_reader:
            #time = float(row[1])/1000 #Slightly faster
            #time = dt.datetime.strptime(row[0], '%H:%M:%S.%f')
            #timespan += [time]
            for i, col_num in enumerate(column_info.keys()):
                val = float(row[int(col_num)])
                #Only smooth sensor data, not time or binary values
                if 2 <= i <= 10:
                    data[i].append(smoothingFactor * data[i][-1] + (1-smoothingFactor) * val)
                else:
                    data[i].append(val)
                    
        for i in range(len(column_info)):
            data[i] = data[i][-data_points:]
                    
        last_pos = csv_file.tell()  
        
    dataIndex = 0
    for i in range(nrows):
        for j in range(ncols):
            if j < ncols - infoColumns:
                lines[dataIndex].set_ydata(data[dataIndices[dataIndex]])
                
                dataIndex += 1
    
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


# Make the window fullscreen
manager = plt.get_current_fig_manager()
manager.full_screen_toggle()

# call animate() function periodically
ani = animation.FuncAnimation(fig, animate, interval=update_rate, 
                              blit=True, cache_frame_data=False)
#plt.tight_layout() # Remove unnecessary whitespace
plt.show()