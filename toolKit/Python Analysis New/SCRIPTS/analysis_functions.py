"""
analysis_functions.py

Author: Francesca Ciacci
Date: October 6, 2025
Version: 1.0.0
Description:
    This script provides functions for analyzing hybrid rocket test data.
"""

import numpy as np
import pandas as pd
import helper_functions
import design_parameters

def get_mass_changes(test_id, prop_mass):
    """
    Mass variations

    Args:
        test_id (str | None): Test identifier (e.g., "CFT10"). If None, 
                              values are inserted manually
        prop_mass (pd.DataFrame): DataFrame containing the propellant masses 
                                  and variations for all tests

    Returns:
        np.ndarray: Array containing [delta_oxidizer, delta_fuel, delta_total]
                    where delta_total = delta_oxidizer + delta_fuel
    """
    if test_id is None:
        # Insert manually
        dox1, dox2, dfuel = 0.382, 0.686, 0.46920
        dox = dox1 + dox2
    else:
        row = prop_mass[prop_mass['Test ID'] == test_id]
        if row.empty:
            raise ValueError(f"Test ID {test_id} not found in Excel sheet")
        dox = (row['delta Oxidizer'].iloc[0])
        dfuel = (row['delta Fuel'].iloc[0])
        # Check if oxidizer/fuel mass is missing
        if pd.isna(dox):
            print(f"⚠️ Oxidizer mass missing for test {test_id}, setting dox=0")
            dox = 0.000
        else:
            dox = float(dox)
        if pd.isna(dfuel):
            print(f"⚠️ Fuel mass missing for test {test_id}, setting dfuel=0")
            dfuel = 0.000
        else:
            dfuel = float(dfuel)
        
    return np.array([dox, dfuel, dox + dfuel])


def compute_fuel_flow(data, mfuel_i, m_dot_ox, index_pres, dfuel, port_i, port_max, length, a=0.0003, n=0.5):
    """
    Compute instantaneous fuel mass flow and remaining fuel over time

    Args:
        data (pd.DataFrame): Test data containing time in the first column
        mfuel_i (float): Initial fuel mass
        m_dot_ox (tuple): Tuple of (ox_time, ox_value) arrays representing oxidizer mass flow
        index_pres (int): Index to start calculating from the pressure signal
        dfuel (float): Total fuel consumed during the test
        port_i (float, optional): Initial port diameter [m]
        port_max (float, optional): Maximum port diameter [m]
        length (float, optional): Fuel grain length [m]
        a (float, optional): Burn rate coefficient. Default 0.0003
        n (float, optional): Burn rate exponent. Default 0.5

    Returns:
        tuple: 
            total_fuel (np.ndarray): Remaining fuel mass at each time step [kg]
            m_dot_fuel (np.ndarray): Instantaneous fuel mass flow rate [kg/s]
    """

    # Initial fuel volume based on port geometry
    fuel_volume_i = (np.pi * length) * ((port_max/2)**2 - (port_i/2)**2)
    density = mfuel_i / fuel_volume_i  # fuel density [kg/m^3]

    # Oxidizer time and values, filter for times after pressure index
    ox_time, ox_value = m_dot_ox
    mask = ox_time > data.iloc[index_pres, 0]
    ox_time_snip = ox_time[mask]
    m_ox_rel = ox_value[mask]

    # Initialize arrays
    R = np.zeros_like(ox_time_snip)           # instantaneous port radius [m]
    R[0] = port_i / 2
    m_dot_fuel = np.zeros_like(ox_time_snip)  # instantaneous fuel mass flow [kg/s]
    total_fuel = np.zeros_like(ox_time_snip)  # remaining fuel mass [kg]
    total_fuel[0] = mfuel_i

    # Loop over time to calculate fuel burning
    for i in range(1, len(ox_time_snip)-1):
        dt = float(ox_time_snip.iloc[i] - ox_time_snip.iloc[i-1])
        A_port = np.pi * R[i-1]**2               # instantaneous port area
        G_ox = float(m_ox_rel.iloc[i]) / A_port  # oxidizer mass flux
        r_dot = a * G_ox ** n                    # burn rate
        R[i] = R[i-1] + r_dot * dt               # update port radius
        A_burning = 2 * np.pi * R[i] * length    # burning surface area
        m_dot_fuel[i] = density * A_burning * r_dot  # fuel flow rate
        total_fuel[i] = total_fuel[i-1] - m_dot_fuel[i] * dt  # remaining fuel

    return total_fuel, m_dot_fuel


def compute_performance(data, column_names, index_line, index_pres, mass_change, ox_rate, fuel_rate, burn_time):
    """
    Computes performance parameters:
      - impulse
      - average thrust
      - Isp
      - C*
      - injector discharge coefficient (Cd)
      - thrust oscillation (sigma)
      - mass flow rates and OF ratio
      - average pressures (ox1, ox2, line, chamber)

    Args:
        data (pd.DataFrame): Test dataset
        column_names (list of str): List of column names
        index_line (list[int]): Indices of ignition/valve events
        index_pres (int): Start index of steady chamber pressure
        mass_change (array): Mass differences [ox, fuel, total]
        ox_rate (float): Oxidizer mass flow rate [kg/s]
        fuel_rate (float): Fuel mass flow rate [kg/s]
        burn_time (float): Burn duration [s]

    Returns:
        dict: performance results
    """
    # Get column numbers
    time_col = helper_functions.get_column_number(column_names, "ArduinoMegaTime")
    load_cell_col = helper_functions.get_column_number(column_names, "LoadCell")
    chamber_col = helper_functions.get_column_number(column_names, "ChamberPressure")
    line_col = helper_functions.get_column_number(column_names, "LinePressure")
    N2O_2_col = helper_functions.get_column_number(column_names, "N2OFeedingPressure2")
    N2O_1_col = helper_functions.get_column_number(column_names, "N2OFeedingPressure1")

    # Mass flow rates 
    prop_rate = ox_rate + fuel_rate
    # mass_flow_rate = np.array([ox_rate, fuel_rate, prop_rate])
    OF_ratio = ox_rate / fuel_rate

    # Impulse & thrust 
    impulse = np.trapz(data.iloc[index_pres:index_line[-1], chamber_col]*design_parameters.factor,
                       data.iloc[index_pres:index_line[-1], time_col])
    thrust = impulse / burn_time
    rolling_std = data.iloc[index_pres:index_line[-1], chamber_col] * design_parameters.factor
    rolling_std = rolling_std.rolling(window=100, center=True).std(ddof=0)
    sigma = np.mean(rolling_std)

    # Isp 
    isp = thrust / (mass_change[2] * 9.81)

    # C* 
    chamber_mean = data.iloc[index_pres:index_line[-1], chamber_col].mean()
    cstar = chamber_mean * 1e5 * design_parameters.throat_area / prop_rate
    cf = thrust / (chamber_mean * 1e5 * design_parameters.throat_area)

    # Pressures 
    line_pressure = data.iloc[index_pres:index_line[-1], line_col].median()
    ox1_pressure = data.iloc[index_line[0]-7000:index_line[0]-5000, N2O_2_col].median()
    ox2_pressure = data.iloc[index_line[0]-7000:index_line[0]-5000, N2O_1_col].median()

    n2o_properties = pd.read_csv('n2o_saturation_properties.csv')
    current_properties = n2o_properties[n2o_properties.iloc[:, 1] == round(ox1_pressure, 0)]
    ox_density = current_properties.iloc[0, 2]

    # Cd 
    pressure_drop = data.iloc[index_line[0]:index_line[-1], line_col] - data.iloc[index_line[0]:index_line[-1], chamber_col]
    pressure_drop = pressure_drop.mean()
    cd = ox_rate / (design_parameters.injector_area * np.sqrt(2 * design_parameters.ox_density * (pressure_drop * 1e5)))
    volumetric_flow_rate = ox_rate / ox_density * 1000

    return dict(
        impulse=impulse,
        thrust=thrust,
        isp=isp,
        cstar=cstar,
        cf=cf,
        sigma=sigma,
        cd=cd,
        OF_ratio=OF_ratio,
        mdot_ox=ox_rate,
        mdot_fuel=fuel_rate,
        mdot_prop=prop_rate,
        pressures=dict(
            ox1=ox1_pressure,
            ox2=ox2_pressure,
            line=line_pressure,
            chamber=chamber_mean,
        )
    )
