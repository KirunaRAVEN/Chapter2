"""
plot_feeding_pressure.py

Author: Francesca Ciacci
Date: October 6, 2025
Version: 1.0
Description:
    This script visualizes the oxidizer feeding and line pressure profiles
    for a hybrid rocket engine test.
"""

import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path
import pandas as pd
import helper_functions
import colors

# -------------
# CONFIGURATION
# -------------
print_all = False       # If True, shows all data without zoom around ignition
save_figures = False     # If True, saves the plots
test_id = 'HFT21'        # Test ID from Prop_mass.xlsx
output_folder = None    # If None, saves in the test folder or 'Files/'

# ---------
# LOAD DATA
# ---------

# GET COLUMN NAMES FOR CSV FILE
# Note that the current file is only for the new test bench format
# if you want the old one, you need to change the file 'csv_layout.txt'
config_file = Path(__file__).resolve().parent.parent/ "Configuration" / "csv_layout.txt"

with open(config_file, "r", encoding="utf-8") as f:
    column_names = [line.strip() for line in f if line.strip()]

script_dir = Path(__file__).resolve().parent.parent
prop_mass = pd.read_excel(script_dir/ "DATA" / "Prop_mass.xlsx")

# DATA from csv
data = helper_functions.read_csv(test_id,column_names,all=False)

# Column numbers
time_col = helper_functions.get_column_number(column_names, 'ArduinoMegaTime')
N2O_2_col = helper_functions.get_column_number(column_names, 'N2OFeedingPressure2')
N2O_1_col = helper_functions.get_column_number(column_names, 'N2OFeedingPressure1')
line_col = helper_functions.get_column_number(column_names, 'LinePressure')
valve_col = helper_functions.get_column_number(column_names, 'ValveSwState')

# Maximum value for plotting
max_pressure = data.iloc[:, N2O_2_col:N2O_1_col].max().item()

# --------------
# SET PLOT STYLE
# --------------
SMALL_SIZE = 7
sns.set(style=None, rc={
    'font.size': SMALL_SIZE,
    'axes.titlesize': SMALL_SIZE,
    'axes.titleweight': 'bold',
    'axes.labelsize': SMALL_SIZE+2,
    'xtick.labelsize': SMALL_SIZE,
    'ytick.labelsize': SMALL_SIZE,
    'legend.fontsize': SMALL_SIZE+2
})

# -----------------
# PLOTTING FUNCTION
# -----------------
def plot_feeding_pressure(data, test_id=None, print_all=False, save=True, output_folder=None):
    fig, ax0 = plt.subplots(figsize=(10, 5))
    ax0.set_xlabel('Time (s)')
    ax0.set_ylabel('Pressure (bar)')
    ax0.yaxis.set_major_locator(plt.MultipleLocator(10))
    ax0.yaxis.set_minor_locator(plt.MultipleLocator(2))
    ax0.grid(True, 'major')

    ax1 = ax0.twinx()
    ax1.get_yaxis().set_visible(False)

    if not print_all:
        ax0.xaxis.set_major_locator(plt.MultipleLocator(1))
        ax0.xaxis.set_minor_locator(plt.MultipleLocator(0.25))
        ax0.set_xlim(-1, 7)

    # Main plots
    ax0.plot(data.iloc[:, time_col], data.iloc[:, N2O_2_col], color=colors.color[8], label='N$_2$O Feeding Pressure 2')
    ax0.plot(data.iloc[:, time_col], data.iloc[:, N2O_1_col], color=colors.color[7], label='N$_2$O Feeding Pressure 1')
    ax0.plot(data.iloc[:, time_col], data.iloc[:, line_col], color=colors.color[9], label='Line Pressure')

    # Open valve window
    ax1.plot(data.iloc[:, time_col], data.iloc[:, valve_col], linestyle='--', color='grey', label='Open valve window')
    ax1.fill_between(x=data.iloc[:, time_col], y1=data.iloc[:, valve_col], where=data.iloc[:, valve_col] > 0, color='grey', alpha=0.07)

    # Mass flow rate
    ax2 = ax0.twinx()
    if not print_all and test_id is not None:
        ax2.set_ylim(0, max_pressure*1.05/100)
        ax2.set_ylabel('Mass flow rate (kg/s)')
        ox_flow = helper_functions.mass_flow_rate(data, test_id, prop_mass, column_names)[0]
        ax2.plot(ox_flow[0], ox_flow[1], label='Oxidizer flow rate')

    # Combined legend
    handles = ax0.get_legend_handles_labels()[0] + ax1.get_legend_handles_labels()[0] + ax2.get_legend_handles_labels()[0]
    labels  = ax0.get_legend_handles_labels()[1] + ax1.get_legend_handles_labels()[1] + ax2.get_legend_handles_labels()[1]
    ax0.legend(handles, labels, loc='lower center', markerscale=5)

    # Save figures
    if save:
        if output_folder is None:
            output_folder = helper_functions.find_test_folder(test_id) + '/' if test_id is not None else 'Files/'
        plt.savefig(f"{output_folder}plot_feeding_pressure.svg", bbox_inches='tight', transparent=True)
        plt.savefig(f"{output_folder}plot_feeding_pressure.png", bbox_inches='tight', transparent=False)

    plt.show()

# ---------
# EXECUTION
# ---------
plot_feeding_pressure(data, test_id, print_all, save_figures, output_folder)
