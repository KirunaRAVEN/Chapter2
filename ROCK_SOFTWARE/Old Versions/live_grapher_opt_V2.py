import matplotlib.pyplot as plt
import matplotlib.animation as animation
import csv
import datetime as dt
import os
import math

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

fig, axs = plt.subplots(nrows, ncols, sharex=True)
for n in range(axs.size): # disable axes (will be enabled for used subplots)
    i = math.floor(n / ncols)
    j = n % ncols
    axs[i,j].axis('off')
if ncols == 1:
    axs = [axs]
    
# initialize data and time containers
timespan = []
data = []
labels = []
for i, col_num in enumerate(column_info.keys()):
    data.append([])

# Used to move to an estimated position within the long file
lineLengthEstimate = 120
# function is called periodically from FuncAnimation
update_rate =  1  #ms As fast as possible
data_rate = 16 #ms
time_window = 10  # seconds
data_points = int(time_window * (1000/data_rate))

if os.path.getsize('data.csv') < data_points * lineLengthEstimate:
    last_pos = 0
else:
    with open('data.csv', 'rb') as csv_file:
        last_pos = csv_file.seek(-data_points * lineLengthEstimate, 2)
        csv_file.readline()
        last_pos = csv_file.tell()

# pre-allocate plot objects to reduce the need to create new objects
lines = []
for i, col_num in enumerate(column_info.keys()):
    for n in range(axs.size):
        i = math.floor(n / ncols)
        j = n % ncols
        axs[i,j].axis('on')
        line, = axs[i,j].plot([], [])
        lines.append(line)
        axs[i,j].set_title(column_info[col_num]['title'])
        axs[i,j].set_ylabel(column_info[col_num]['ylabel'])
        axs[i,j].set_ylim([column_info[col_num]['ymin'],
                     column_info[col_num]['ymax']])
        axs[i,j].tick_params(axis='x', rotation=45)

############################# convert indices in animate func from i to i,j (same as above)
def animate(frameNum):
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
        return lines
    
    # otherwise, read the new data from the file
    with open('data.csv', 'r') as csv_file:
        csv_reader = csv.reader(csv_file)
        csv_file.seek(last_pos)
        #updated = False
        for row in csv_reader:
            time = float(row[1])/1000 #Slightly faster
            #time = dt.datetime.strptime(row[0], '%H:%M:%S.%f')
            timespan += [time]
            for i, col_num in enumerate(column_info.keys()):
                val = float(row[int(col_num)])
                #if time not in data[col_num]['xs']:
                #data[col_num]['xs'] = data[col_num]['xs'] + [time]
                data[i] = data[i] + [val]
                #updated = True
                
        for i in range(len(column_info)):
            #data[col_num]['xs'] = data[col_num]['xs'][-data_points:] 
            data[i] = data[i][-data_points:]
        
        timespan = timespan[-data_points:]
            
        last_pos = csv_file.tell()

    # if data has been updated, redraw plots
    #if updated:
    for i in range(len(column_info)):
        lines[i].set_xdata(timespan)
        lines[i].set_ydata(data[i])
        axs[i].set_xlim([timespan[0], timespan[-1]])
        axs[i].grid()

    return lines

# call animate() function periodically
ani = animation.FuncAnimation(fig, animate, interval=update_rate, 
                              blit=True, cache_frame_data=False)
plt.tight_layout() # Remove unnecessary whitespace
plt.show()
