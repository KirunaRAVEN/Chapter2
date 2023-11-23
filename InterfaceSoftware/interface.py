'''
    AUTHOR: Louis-Hendrik Barboutie
    CONTACT: louis.barboutie@gmail.com, loubar-3@student.ltu.se
    
    SUMMARY: The script runs a matplotlib animation to plot out the data received by the RAVEN test stand. The incoming data comes from a csv file generated by the
        serial_reader.py script, which itself receives the data from the SerialComms.cpp script. The interface is manually designed and an animation function
        handles the updating of the animated artists.

    SCRIPT STRUCTURE:

    DIRECTORY STRUCTURE:
        |
        +-- interface.py
        +-- indicatorClass.py
        +-- dataDisplayClass.py
        +-- data.csv

    CHANGELOG: 
        - 15/11/23: Added escape key press handling, header comment
        - 08/11/23: Successful implementation of animation with dummy data

    TODO:
        - Add function to automatically save file upon closing
        - read data properly from csv file
        - add message box for software mode, substate and arduino time
        - add function to compute the rolling average
        - add display of latest value on graphs
        - improve artist list in 
'''

# ================= #
# === LIBRARIES === #
# ================= #

# external
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.animation as ani
import numpy as np
import tkinter
import csv

# internal (functions)
# from update import update 
import indicatorClass
import dataDisplayClass




# ================= #
# === VARIABLES === #
# ================= #

dataQuantities = ['Chamber Pressure',
                  'Bottle Pressure',
                  'Line Pressure',
                  'Bottle Temperature',
                  'Nozzle Temperature',
                  'Load Cell Force',
                  'Ambient Temperature',
                  'Plume Temperature']
dataLabels = ['Pressure',
              'Pressure',
              'Pressure',
              'Temperature',
              'Temperature',
              'Force',
              'Temperature',
              'Temperature']
dataUnits = []

nData = len(dataQuantities)

csvDataCount = 18
csvData = []

# ======================= #
# === INTERFACE SETUP === #
# ======================= #

# --- find out screen height and width --- #
root = tkinter.Tk() # create window
screenWidth, screenHeight = root.winfo_screenmmwidth(), root.winfo_screenmmheight() # fetch dimensions
root.destroy() # destroy window

# remove toolbar
plt.rcParams['toolbar'] = 'None'

# select backend
mpl.use("qtagg")

# create figure interface and separate panels for graphs, displays and indicators
interface = plt.figure(figsize=(screenWidth, screenHeight), facecolor='silver')
dataPanel, infoPanel = interface.subfigures(1, 2, wspace=0.7, width_ratios=[3,1]) 
indicatorPanel, streamPanel = infoPanel.subfigures(2, 1, height_ratios=[3,2])
displayPanel, lightsPanel = indicatorPanel.subfigures(1,2)

# adjust background colors of the subfigures
dataPanel.set_facecolor('silver')
lightsPanel.set_facecolor('slategrey')
displayPanel.set_facecolor('green')
streamPanel.set_facecolor('red')

# ------------------ #
# --- DATA PANEL --- #
# ------------------ #

# create subplot layout for the graphs
graphs = dataPanel.subplots(4, 2)

# adjust spacing so that graphs don't overlap and fill out whole panel
dataPanel.subplots_adjust(left=0.04, right=0.98, bottom=0.05, top=0.97, wspace=0.15, hspace=0.4)

# fontsize definitions for axis labeling
axesFontSize = 8

# add titles and axis labels to the graphs
for i in range(4):
    for j in range(2):
        graphs[i,j].set_title(dataQuantities[i+j*4])
        graphs[i,j].set_xlabel('time', fontsize=axesFontSize)
        graphs[i,j].set_ylabel('yLabel', fontsize=axesFontSize)
        graphs[i,j].tick_params(labelsize=axesFontSize)

# ------------------ #
# --- INFO PANEL --- #
# ------------------ #

# create subplots for the info panel
indicatorObjects = lightsPanel.subplots(3, 1)

lightsPanel.subplots_adjust(left=0.04, right=0.98, bottom=0.05, top=0.97, wspace=0.15, hspace=0.1)

# create indicators
heatingBlanketIndicator = indicatorClass.indicatorLight('Heating\nBlanket')
mainValveIndicator      = indicatorClass.indicatorLight('Main\nValve')
ignitionRelayIndicator  = indicatorClass.indicatorLight('Ignition\nRelay')

# create indicator list for easy access
indicators = [heatingBlanketIndicator, mainValveIndicator, ignitionRelayIndicator]
indicatorStates = [1, 1, 0]
nIndicators = len(indicators)
for i in range(3):
    indicators[i].setState(indicatorStates[i])

# adjust appearance of info panel subplots
for i in range(3):
    # set the aspect ratio to be a square
    indicatorObjects[i].set_aspect('equal', 'box')
    
    # remove graph background
    indicatorObjects[i].axis('off')

# add indicators to the info panel
for i in range(3):
    # add indicator objects
    for j in range(len(indicators[i].objects)):
        indicatorObjects[i].add_artist(indicators[i].objects[j])
    # set indicator text
    indicatorObjects[i].text(0.5, 0.48, indicators[i].title, fontsize=indicators[i].fontSize, ha='center', va='center')

# --------------------- #
# --- DATA DISPLAYS --- #
# --------------------- #

# create subplots for the info panel
displayObjects = displayPanel.subplots(4, 1)

#displayObjects.subplots_adjust(left=0.04, right=0.98, bottom=0.05, top=0.97, wspace=0.15, hspace=0.1)

bottlePressureDisplay    = dataDisplayClass.dataDisplay('Bottle Pressure')
bottleTemperatureDisplay = dataDisplayClass.dataDisplay('Bottle Temperature')
loadCellForceDisplay     = dataDisplayClass.dataDisplay('Load Cell Force')

dataDisplays = [bottlePressureDisplay, bottleTemperatureDisplay, loadCellForceDisplay]
dataDisplayValues = [0, 0, 0]
nDataDisplays = len(dataDisplays)

for i in range(nDataDisplays):
    # set the aspect ratio to be a rectangle
    displayObjects[i].set_aspect(aspect=0.4)
    
    # remove graph background
    displayObjects[i].axis('off')

for i in range(nDataDisplays):
    # add data display objects
    for j in range(len(dataDisplays[i].objects)):
        displayObjects[i].add_artist(dataDisplays[i].objects[j])

    # set data display title
    displayObjects[i].set_title(dataDisplays[i].title, fontsize=dataDisplays[i].fontSize)

    # set data display value
    displayObjects[i].text(0.5, 0.48, dataDisplays[i].value, fontsize=dataDisplays[i].fontSize, ha='center', va='center')



# =========================== #
# === INTERFACE ANIMATION === #
# =========================== #

# --------------------------------- #
# --- ADD ARTISTS TO THE GRAPHS --- #
# --------------------------------- #

lines = []
for i in range(4):
    for j in range(2):
        lines.append(graphs[i][j].plot([],[])[0])

artists = []
for i in range(nData):
    artists.append(lines[i])

for i in range(4):
    for j in range(2):
        graphs[i][j].set_ylim(0,1)
        graphs[i][j].set_xlim(0,25)

x_ticks = []
x_data = []
y_data = []
for i in range(8):
    y_data.append([])
for j in range(25):
    x_ticks.append('')
    x_data.append(j)
    for i in range(8):
        y_data[i].append(0)

for i in range(nData):
        artists[i].set_xdata(x_data)

lastFilePosition = 0
def update(frame):
    global lastFilePosition
    global indicatorStates

    # ------------------------------- #
    # --- READ DATA FROM CSV FILE --- #
    # ------------------------------- #

    with open('dummyData.csv', 'r', newline='') as dataFile:
        dataFile.seek(lastFilePosition)
        csvReader = csv.reader(dataFile)
        for row in csvReader:
            for i in range(8):
                x_ticks.pop(0)
                x_ticks.append(row[0])
                y_data[i].pop(0)
                y_data[i].append(float(row[i+1]))
        lastFilePosition = dataFile.tell()

    # ---------------------------- #
    # --- UPDATE DATA IN PLOTS --- #
    # ---------------------------- #
    for i in range(nData):
        artists[i].set_ydata(y_data[i])
        
    for i in range(4):
        for j in range(2):
            graphs[i][j].set_xticklabels(x_ticks)

    # ----------------------------------------- #
    # --- DUMMY UPDATE THE INDICATOR LIGHTS --- #
    # ----------------------------------------- #

    # dummy check to update
    if frame % 10 == 0:
        # cycle indicator list around
        item = indicatorStates[0]
        indicatorStates.pop(0)
        indicatorStates.append(item)

    # update the indicator state
    for i in range(3):
        indicators[i].setState(indicatorStates[i])

    # Build the return argument of the function, the list of artists to keep track of for the animation 
    returnList = artists + indicators[0].objects + indicators[1].objects + indicators[2].objects
    for i in range(4):
        for j in range(2):
            returnList.append(graphs[i][j].xaxis)
    
    return  returnList

# toggle fullscreen
plt.get_current_fig_manager().full_screen_toggle()

# animation settings
isUsingBlit = True
isCachingFrameData = False
updateRate = 10 # milli seconds

# function to add escape key press to exit figure
def escape(esc):
    if esc.key == 'escape':
        plt.close()

# link the key press event to the interface
interface.canvas.mpl_connect('key_press_event', escape)

# animate the interface
interfaceAnimation = ani.FuncAnimation(interface, update, interval=updateRate, blit=isUsingBlit, cache_frame_data=isCachingFrameData)
plt.show()