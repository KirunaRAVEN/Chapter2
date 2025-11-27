"""
main_performance.py

Author: Francesca Ciacci
Date: October 6, 2025
Version: 1.0.0
Description:
    Main script for computing hybrid rocket performance parameters.
    It loads test data, computes propellant mass flow rates, 
    and calculates performance indicators.
    The script also saves the processed results to a txt file.
"""

import helper_functions
import design_parameters as d_param
import analysis_functions
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from pathlib import Path

# ----------------------------------
# DATA acquisition and configuration
# ----------------------------------

# ENTER A VALID TEST ID
test_id = 'HFT19'

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

# Mass changes of each test in kg [ox, fu, prop]
mass_change = analysis_functions.get_mass_changes(test_id,prop_mass)

# Get start and end point of line pressure and chamber pressure
line_index = helper_functions.get_line_indices(data, column_names)

if 'HFT' in test_id:
    press_index = helper_functions.get_pressure_index(data,column_names)
else:
    press_index = line_index[0]

# ------------------------
# PERFORMANCE computations
# ------------------------

# Compute burn time
valve_time = data.iloc[line_index[-1], 0] - data.iloc[line_index[0], 0]
burn_time = data.iloc[line_index[-1], 0] - data.iloc[press_index, 0]

# Compute approximate mass flow rate
m_dot_ox, ox_flow = helper_functions.mass_flow_rate(data,test_id,prop_mass,column_names)

# Compute istantaneous fuel flow rate
row = prop_mass[prop_mass['Test ID'] == test_id]
mfuel_i = float(row['Fuel before'].iloc[0])
mfuel_f = float(row['Fuel after'].iloc[0])
dfuel = float(row['delta Fuel'].iloc[0])
fuel_time = m_dot_ox[0][m_dot_ox[0] > data.iloc[press_index,0]]

# Get design parameters
port_i = d_param.port_initial
port_max = d_param.port_max
length = d_param.length

total_fuel, m_dot_fuel = analysis_functions.compute_fuel_flow(data,mfuel_i,m_dot_ox,press_index,dfuel,port_i,port_max,length)
print('Total fuel:', total_fuel[-2])

# Correct burn rate coefficient based on actual fuel consumption
a = d_param.a
correct_a = a * dfuel / (mfuel_i - total_fuel[-2])
print('Correct a:', correct_a)

fuel_rate = np.median(analysis_functions.compute_fuel_flow(data,mfuel_i,m_dot_ox,press_index,dfuel,port_i,port_max,length,correct_a)[1])

# Compute performance
results = analysis_functions.compute_performance(data,column_names,line_index,press_index,mass_change,ox_flow,fuel_rate,burn_time)

# -------------
# PRINT RESULTS
# -------------

p = results["pressures"]

print(f"\nOx1 pressure = {p['ox1']:.1f} bar\n"
      f"Ox2 pressure = {p['ox2']:.1f} bar\n"
      f"Line pressure = {p['line']:.1f} bar\n"
      f"Chamber pressure = {p['chamber']:.1f} bar\n")

print(f"Burn time = {burn_time:.2f} s\n"
      f"Oxidizer mass flow = {results['mdot_ox'] * 1000:.0f} g/s\n"
      f"Fuel mass flow = {results['mdot_fuel'] * 1000:.1f} g/s\n"
      f"Propellant mass flow = {results['mdot_prop'] * 1000:.0f} g/s\n"
      f"OF ratio = {results['OF_ratio']:.2f}\n"
      f"Injector Cd = {results['cd']:.3f}\n")

print(f"Average thrust = {results['thrust']:.1f} N\n"
      f"Thrust oscillation σ = {results['sigma']:.1f} N "
      f"({results['sigma'] / results['thrust'] * 100:.1f}% of thrust)\n"
      f"Total impulse = {results['impulse']:.1f} Ns\n"
      f"Specific impulse = {results['isp']:.1f} s\n"
      f"C* = {results['cstar']:.1f} m/s\n"
      f"Cf = {results['cf']:.3f}\n")

# ------------
# SAVE RESULTS
# ------------

save_dir = Path(helper_functions.find_test_folder(test_id))
file_path = save_dir / f"{test_id}_performance.txt"

with open(file_path, 'w') as f:
    print(f"Average pressures during steady state operation (OX1, OX2, Line):", file=f)
    print(f"\n{p['ox1']:.1f} & "
          f"{p['ox2']:.1f} & "
          f"{p['line']:.1f} \\\\", file=f)
    
    print(f"\nMass flow rates and oxidizer ratios (burn time, ox, fuel, prop, ratio, cd):", file=f)
    print(f"\n{burn_time:.2f} & "
          f"{results['mdot_ox'] * 1000:.0f} & "
          f"{results['mdot_fuel'] * 1000:.1f} & "
          f"{results['mdot_prop'] * 1000:.0f} & "
          f"{results['OF_ratio']:.2f} & "
          f"{results['cd']:.3f} \\\\", file=f)
    
    print(f"\nPerformance characteristics (thrust, sigma, impulse, isp, cstar):", file=f)
    print(f"\n{results['thrust']:.1f} & "
          f"{results['sigma']:.1f} & "
          f"{results['impulse']:.1f} & "
          f"{results['isp']:.1f} & "
          f"{results['cstar']:.1f} \\\\", file=f)
    