import numpy as np
import pandas as pd

# Geometry
throat_area = 0.0004909             # [m²] nozzle throat area
n_injectors = 20                    # [-]
injector_diam = 1e-3                # [m]
injector_area = n_injectors * (injector_diam / 2)**2 * np.pi # total injector area

# Combustion parameters
port_initial = 0.032    # [m] initial port diameter
port_max = 0.07         # [m] maximum port diameter
length = 0.23           # [m] fuel grain length

factor = 67.31413678373045

a = 0.0003