"""
csv_edit.py

Author: Francesca Ciacci
Date: October 6, 2025
Version: 1.0.0
Description:
    This module provides utility functions for handling and calibrating 
    CSV files used in hybrid rocket test data processing.
"""

import pandas as pd
import numpy as np
import os

def add_header_to_csv(csv_path, column_names, output_path):
    """
    Add a header row to a CSV file using a provided list of column names
    Cleans any extra rows containing repeated header text

    Args:
        csv_path (str): Path to the input CSV file (without header)
        column_names (list of str): List of column names to use as header
        output_path (str, optional): Path to save the new CSV file. If None, overwrites original
    """
    if output_path is None:
        output_path = csv_path

    # Load CSV without header
    data = pd.read_csv(csv_path, header=None, sep=',', decimal='.')

    # Remove rows where first column matches header name (duplicate headers)
    first_col_name = column_names[0]
    data = data[data.iloc[:, 0] != first_col_name]

    # Assign new header
    data.columns = column_names

    # Save CSV with header
    data.to_csv(output_path, sep=',', decimal='.', index=False)


def calibrate_csv(csv_file, output_path=None):
    """
    Calibrate a CSV file by applying slope/offset corrections to specific columns
    Saves a new file with '_calibrated' in the name

    Args:
        csv_file (str): Path to the CSV file (with header)
        output_path (str, optional): Custom path for the calibrated CSV
                                     If None, saves as '<name>_calibrated.csv' in the same folder
    """
    # Read file
    data = pd.read_csv(csv_file, sep=',', decimal='.')

    # Define calibration values: (regex pattern, slope, offset)
    calibration_map = [
        ('LoadCell', 0.9931, 44.03),
        ('Bottle/HeatingBlanketTemperature', 1.04, 1.8),
        ('NozzleTemperature', 1.018, 10.95),
        ('PipingTemperature', 1.019, 15.28),
        ('PlumeTemperature', 0.9749, 4.84)
    ]

    # Apply calibration
    for pattern, slope, offset in calibration_map:
        matches = data.columns[data.columns.str.contains(pattern, case=False)]
        for col in matches:
            # Convert to numeric
            data[col] = pd.to_numeric(data[col], errors="coerce")
            data[col] = data[col] * slope + offset

    # Build output path if not given
    if output_path is None:
        folder, filename = os.path.split(csv_file)
        name, ext = os.path.splitext(filename)
        output_path = os.path.join(folder, f"{name}_calibrated{ext}")

    # Save calibrated file
    data.to_csv(output_path, sep=',', decimal='.', index=False)
    print(f"Calibrated file saved as: {output_path}")


