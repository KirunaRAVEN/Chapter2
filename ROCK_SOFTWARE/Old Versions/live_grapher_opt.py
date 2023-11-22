import matplotlib.pyplot as plt
import matplotlib.animation as animation
import csv
import datetime as dt
import os

plt.rcParams['toolbar'] = 'None'

# load column information from text file
# THE SCRIPT WILL ONLY PLOT COLUMNS THAT ARE LISTED IN THIS FILE
column_info = {}
with open('titles.txt') as f:
    for line in f:
        col_num, info_str = line.split(':')
        title, xlabel, ylabel, ymax, ymin = info_str.strip().split(',')
        column_info[int(col_num.strip())] = {'title': title, 'xlabel': xlabel, 'ylabel': ylabel, 'ymax': int(ymax), 'ymin': int(ymin)}

# initialize figure and subplots
n_cols = len(column_info)
fig, axs = plt.subplots(nrows=n_cols, ncols=1, figsize=(16, 5*n_cols), sharex=True)
if n_cols == 1:
    axs = [axs]
    
# initialize data containers
data = {}
for col_num in column_info.keys():
    data[col_num] = {'xs': [], 'ys': []}

# function is called periodically from FuncAnimation
update_rate = 100  # ms
time_window = 30  # seconds
data_points = int(time_window / (update_rate / 1000))
last_pos = 0

# pre-allocate plot objects to reduce the need to create new objects
lines = []
for i in range(n_cols):
    line, = axs[i].plot([], [])
    lines.append(line)

def animate(i):
    global last_pos
    
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
        updated = False
        for row in csv_reader:
            time = dt.datetime.strptime(row[0], '%H:%M:%S.%f')
            for col_num in column_info.keys():
                val = float(row[col_num])
                if time not in data[col_num]['xs']:
                    data[col_num]['xs'] = data[col_num]['xs'] + [time]
                    data[col_num]['ys'] = data[col_num]['ys'] +  [val]
                    updated = True
        for col_num in column_info.keys():
            data[col_num]['xs'] = data[col_num]['xs'][-data_points:] 
            data[col_num]['ys'] = data[col_num]['ys'][-data_points:]
        last_pos = csv_file.tell()

    # if data has been updated, redraw plots
    if updated:
        for i, col_num in enumerate(column_info.keys()):
            lines[i].set_xdata(data[col_num]['xs'])
            lines[i].set_ydata(data[col_num]['ys'])
            axs[i].set_title(column_info[col_num]['title'])
            axs[i].set_ylabel(column_info[col_num]['ylabel'])
            axs[i].set_ylim([column_info[col_num]['ymin'], column_info[col_num]['ymax']])
            axs[i].set_xlim([data[col_num]['xs'][0], data[col_num]['xs'][-1]])
            if i == n_cols-1:
                axs[i].set_xlabel(column_info[col_num]['xlabel'])
            axs[i].tick_params(axis='x', rotation=45)

    return lines

# call animate() function periodically
ani = animation.FuncAnimation(fig, animate, interval=update_rate)
plt.show()