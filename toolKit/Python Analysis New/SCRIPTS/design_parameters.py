import numpy as np

# Geometry
throat_area = 0.0004909             # [m²] nozzle throat area
n_injectors = 14                    # [-]
injector_diam = 1e-3                # [m]
injector_area = n_injectors * (injector_diam / 2)**2 * np.pi # total injector area

# Physical properties
ox_density = 907        # [kg/m³] oxidizer density

# Combustion parameters
port_initial = 0.032    # [m] initial port diameter
port_max = 0.07         # [m] maximum port diameter
length = 0.23           # [m] fuel grain length

a = 0.0003