"""
helper_functions.py

Author: Francesca Ciacci
Date: October 6, 2025
Version: 1.0
Description:
    This script contains utility functions used for data preprocessing, 
    file management, and signal analysis in hybrid rocket test data.
"""

import os
import re 
import numpy as np
import pandas as pd
from pathlib import Path
import csv_edit

def get_column_number(column_names, name, start_from_1=False):
    """
    Get the index of a column given its name

    Args:
        column_names (list of str): List of column names
        name (str): The name of the column to find
        start_from_1 (bool, optional): If True, numbering starts from 1 (default). 
                                       If False, numbering starts from 0

    Returns:
        int: Column number corresponding to the given column name
    """
    idx = column_names.index(name)
    return idx + 1 if start_from_1 else idx


def find_csv_file(folder_path, column_names, test_id):
    """
    Return the calibrated CSV file path in a given directory
    If no calibrated file exists, create one from the raw CSV

    Args:
        folder_path (str): Path to the folder containing CSV files
        column_names (list of str): List of column names to add if header is missing
        test_id (str): Test identifier

    Returns:
        list[str]: Paths of all calibrated CSV files in the folder
    """

    calibrated_files = []

    for filename in os.listdir(folder_path):
        if filename.endswith(".csv") and "_calibrated" not in filename.lower():
            csv_file = os.path.join(folder_path, filename)

            # Build calibrated file path
            name, ext = os.path.splitext(filename)
            calibrated_file = os.path.join(folder_path, f"{name}_calibrated{ext}")

            if os.path.exists(calibrated_file):
                print(f"Calibrated file already exists: {calibrated_file}")
            else:
                # Step 1: add header
                csv_edit.add_header_to_csv(csv_file, column_names, csv_file)

                # Step 2: calibrate
                csv_edit.calibrate_csv(csv_file)

                print(f"Created calibrated file: {calibrated_file}")

            calibrated_files.append(calibrated_file)

    # Sort calibrated files to ensure consistent ordering
    calibrated_files.sort()

    # Select file(s) based on test_id
    match = re.match(r"^([A-Z]+[0-9]+)(?:-(\d+))?$", test_id, re.IGNORECASE)
    if not match:
        raise ValueError(f"Invalid test_id format: {test_id}")

    base_id, run_number = match.groups()

    if run_number:  # Case with more than 1 data csv file in the test folder
        idx = int(run_number) - 1  # convert to 0-based index
        try:
            return calibrated_files[idx]
        except IndexError:
            raise FileNotFoundError(
                f"No CSV available for {test_id}. "
                f"Found only {len(calibrated_files)} file(s)."
            )
    else:  # Case with only 1 csv data file in the test folder
        if len(calibrated_files) != 1:
            raise FileNotFoundError(
                f"Expected exactly 1 file for {test_id}, "
                f"but found {len(calibrated_files)}"
            )
        return calibrated_files[0]


def find_test_folder(test_id):
    """
    Find the folder corresponding to a test ID by using Prop_mass.xlsx

    Args:
        test_id (str): Test identifier (e.g. "CFT10")

    Returns:
        str: Path to the test folder
    """
    root_dir = Path(__file__).resolve().parent.parent
    prop_mass = pd.read_excel(root_dir/ "DATA" / "Prop_mass.xlsx")

    row = prop_mass[prop_mass["Test ID"] == test_id]
    if row.empty: # check if the Test ID is not in Prop_mass.xlsx
        raise ValueError(f"Test ID {test_id} not found in Prop_mass.xlsx")

    date = row["Date"].dt.strftime("%Y_%m_%d").iloc[0]

    print(os.listdir(root_dir.parent))

    matches = [
        os.path.join(root_dir.parent, f)
        for f in os.listdir(root_dir.parent)
        if f.startswith(date) #and os.path.isdir(os.path.join(root_dir.parent, f))
    ]
    if not matches: # check if no matching folder is found
        raise FileNotFoundError(f"No folder found for {date}")
    return matches[0]


def find_indices(data, column_names):
    """
    Find ignition and valve closing indices

    Args:
        data (pd.DataFrame): Experimental dataset
        column_names (list of str): List of column names

    Returns:
        tuple[int, int, pd.Index]: 
            - ignition index (int)
            - valve closing index (int)
            - all ignition transition indices (pd.Index)
    """
    ign_col = data.iloc[:, get_column_number(column_names, "ValveSwState")].astype(int)
    
    n_rows = len(data)
    print(f"find_indices len(data): {len(data)}")

    # Identify 0 → 1 transitions with at least 100 samples quiet period before
    quiet_period = 100
    ignition_candidates = ign_col[(ign_col == 1) &
                                 (ign_col.shift(1) == 0) &
                                 (ign_col.shift(1).rolling(quiet_period).sum() == 0)].index

    # Choose the last ignition transition, fallback to substate if not found
    index_ign = None
    if not ignition_candidates.empty:
        index_ign = ignition_candidates[-1]
    else:
        print("Couldn't find ignition index, trying substate column...")
        substate_col = data.iloc[:, get_column_number(column_names, "CurrentSwSubstate")]
        print(substate_col)
        substate_candidates = substate_col[(substate_col == 1) & (substate_col.shift(1).fillna(0) == 2)].index
        if not substate_candidates.empty:
            index_ign = substate_candidates[-1]
        else:
            raise ValueError("EMERGENCY: NO IGNITION DETECTED!")

    # Ensure index_ign is within bounds
    index_ign = min(index_ign, n_rows - 1)

    # Find first valve closing transition 1 → 0 after ignition
    start = 0 if index_ign > len(ign_col) else index_ign
    valve_col = data.iloc[start:, get_column_number(column_names, "ValveSwState")].astype(int)
    shifted_valve = valve_col.shift(1)
    valve_candidates = valve_col[(valve_col == 0) & (shifted_valve == 1)].index
    if valve_candidates.empty:
        index_valve = None  
        print("⚠️ Warning: No valve transition detected")
    else:
        index_valve = valve_candidates[0]

    if index_ign > len(ign_col):
        index_valve = 20000 + index_valve - index_ign
        index_ign = 20000

    return index_ign, index_valve, ignition_candidates


def read_csv(test_id, column_names, all=False, rounding=False):
    """
    Load and preprocess the CSV for a test

    Args:
        test_id (str | None): Test identifier, or None for manual data
        column_names (list of str): List of column names
        all (bool): If True, keep all rows without trimming
        rounding (bool): If True, round time column (s) to 2 decimals

    Returns:
        pd.DataFrame: Preprocessed dataset
    """
    # Load CSV
    if test_id is None:
        csv_path = find_csv_file("DATA/", column_names, test_id) # get file from DATA folder
    else:
        folder = find_test_folder(test_id)
        csv_path = find_csv_file(folder, column_names, test_id)

    data_frame = pd.read_csv(csv_path)

    print(f"len data_frame: {len(data_frame)}")

    # Find ignition and valve indices
    ign_idx, valve_idx, _ = find_indices(data_frame, column_names)

    ign_offset = ign_idx - max(ign_idx - 20000, 0)

    # Trim around ignition and valve, exactly like the old version
    if not all:
        data_frame = data_frame.iloc[max(ign_idx - 20000, 0) : min(valve_idx + 30000, len(data_frame))]

    print(f"len data_frame (after trimming): {len(data_frame)}")

    # Remove baseline offset from ChamberPressure and LoadCell
    for sensor in ["ChamberPressure", "LoadCell"]:
        col_idx = get_column_number(column_names, sensor)
        offset = data_frame.iloc[3500:4000, col_idx].mean()
        data_frame.iloc[:, col_idx] -= offset

    # Normalize time column using ignition time as offset
    time_idx = get_column_number(column_names, "ArduinoMegaTime")
    time_diff = np.diff(data_frame.iloc[0:10, time_idx]).mean()

    data_frame.iloc[:, time_idx] = (data_frame.iloc[:, time_idx] - data_frame.iloc[ign_offset, time_idx]) / 1000
    data_frame_out = data_frame

    return data_frame_out


def get_pressure_index(data,column_names):
    """
    Find index where chamber pressure rises

    Args:
        data (pd.DataFrame): Test data
        column_names (list of str): List of column names

    Returns:
        int: index where chamber pressure starts rising
    """
    valve_idx = find_indices(data,column_names)[1]

    pres_col = get_column_number(column_names,"ChamberPressure")
    pres_diff = np.diff(data.iloc[:, pres_col])

    mask = (pres_diff > 0.8) & (data.iloc[:-1, pres_col] > 4)
    mask_end = (pres_diff  < -1) & (data.iloc[:-1, pres_col] < data.iloc[valve_idx:valve_idx + 1000, pres_col].mean() / 2)

    first = np.nonzero(mask)[0][0]

    return first


def get_line_indices(data, column_names):
    """
    Find start and end indices for line pressure window

    Args:
        data (pd.DataFrame): Test data
        column_names (list of str): List of column names

    Returns:
        list: [start_index, end_index]
    """
    _, valve_idx, _ = find_indices(data, column_names)
    line_col = get_column_number(column_names, "LinePressure")

    # Compute absolute difference between consecutive line pressure values
    diff_line = np.abs(np.diff(data.iloc[:, line_col]))

    mask = (diff_line > 0.4) & (data.iloc[:-1, line_col] > 0.75*min(data.iloc[valve_idx:, line_col].max(), data.iloc[:, line_col].max()))
    indices = np.where(mask)[0]

    if indices.size == 0:
        return [None, None]

    first = indices[0]  # first index where conditions are met
    # Find last index after valve if possible; otherwise take last in mask
    last = next((x for x in np.nonzero(mask)[0] if x > valve_idx), np.nonzero(mask)[0][-1])

    return [first, last]


def mass_flow_rate(data, test_id, prop_mass, column_names, valve_idx=None):
    """
    Compute oxidizer mass flow rate using line and chamber pressure

    Args:
        data (pd.DataFrame): Experimental dataset
        test_id (str): Test identifier
        prop_mass (pd.DataFrame): DataFrame containing the propellant masses 
                                  and variations for all tests
        column_names (list of str): List of column names
        valve_idx (int | None): Optional index of valve closing (from find_indices)

    Returns:
        tuple:
        - (time_slice, mass_flow) (tuple[np.ndarray, np.ndarray]): 
            Time vector (s) and corresponding instantaneous oxidizer mass flow rate (kg/s)  
        - stable_ox_flow (float): 
            Estimated stable oxidizer mass flow rate (kg/s), obtained as the mode of the distribution
    """
    # Oxidizer mass variation for this test
    dox = float(prop_mass.loc[prop_mass["Test ID"] == test_id, "delta Oxidizer"].iloc[0])

    # Extract pressure columns
    line_col = get_column_number(column_names, "LinePressure")
    press_col = get_column_number(column_names, "ChamberPressure")
    p_line = data.iloc[:, line_col]
    p_chamber = data.iloc[:, press_col].rolling(window=100, center=True).mean()

    # Δp signal 
    # sqrt_dp = np.sqrt(np.maximum(p_line - p_chamber, 0)) avoid negatives
    sqrt_dp = np.sqrt(p_line)

    # Time
    time_col = get_column_number(column_names, "ArduinoMegaTime")
    time = data.iloc[:, time_col]

    # Define useful test window
    index_start = sqrt_dp[(~np.isnan(sqrt_dp)) & (time >= 0)].index

    # Search for end of test within 20 s and line pressure > 2
    index_end = sqrt_dp[(~np.isnan(sqrt_dp)) & (time < 20) & (p_line > 2)].index

    # Fallback if no start/end found
    if len(index_start) == 0:
        print(f"⚠️ Warning: could not find start index for test {test_id}, using first valid index.")
        index_start = [0]
    if len(index_end) == 0:
        print(f"⚠️ Warning: could not find end index for test {test_id}, using last valid index.")
        index_end = [len(sqrt_dp) - 1]

    # Slice useful window
    time_slice = time[index_start[0]:index_end[-1]]
    sqrt_dp_slice = sqrt_dp[index_start[0]:index_end[-1]]

    # Remove NaN
    mask = ~np.isnan(sqrt_dp_slice)

    total_mass = np.trapz(sqrt_dp_slice[mask], time_slice[mask])
    if total_mass <= 1e-9:
        print(f"⚠️ Test {test_id}: total_mass ~ 0, setting oxidizer flow = 0")
        return pd.Series(np.zeros_like(time_slice), index=time_slice), 0.0

    # Normalize with oxidizer mass
    constant = dox / total_mass
    mass_flow = constant * sqrt_dp_slice[mask]

    # Stable flow estimation = mode of distribution
    counts, bin_edges = np.histogram(mass_flow, bins=1000)
    max_bin_index = np.argmax(counts)
    stable_ox_flow = np.mean([bin_edges[max_bin_index], bin_edges[max_bin_index + 1]])

    return (time_slice[mask], mass_flow), stable_ox_flow
