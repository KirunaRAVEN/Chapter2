Guide on how to use python analysis scripts 

FOLDERS LAYOUT:
DATA --> propellant mass and n2o data (files need to be up to date, changing every test)
SCRIPTS --> all the scripts to run the analysis
Configuration --> txt files with csv configuration (column names)

Before running each script make sure to enter a valid test_id. It needs 
to be the same as the one in prop_mass.

All outputs to the scripts are saved in the same folder as the csv data 
for the corresponding test.

-------------------
SCRIPTS:

%%%%%%%%%%%%%%%%%%%
main_performance.py:

Analyzes data and computes main performance parameters of the selected test.


%%%%%%%%%%%%%%%%%%%
plot_feeding_pressure.py:

Plots feeding pressure for the selected test.


%%%%%%%%%%%%%%%%%%%
plot_thrust_pressure.py:

Plots thrust and chamber pressure for the selected (hot fire) test.


%%%%%%%%%%%%%%%%%%%
helper_functions.py:

File with functions needed to work with the csv file and elaborate data.


%%%%%%%%%%%%%%%%%%%
analysis_functions.py:

File with functions to perform analysis of preprocessed data.


%%%%%%%%%%%%%%%%%%%
csv_edit.py:

File to calibrate and save the csv file before performing the analysis.


%%%%%%%%%%%%%%%%%%%
design_parameters.py:

File containing main design parameters useful for the analysis.


%%%%%%%%%%%%%%%%%%%
colors.py:

File with color values for the plots.

