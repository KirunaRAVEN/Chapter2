chamberPressure= {
    'title': 'Chamber Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 172,
    'warningValue': 40
}

bottlePressure = {
    'title': 'Bottle Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 100,
    'warningValue': 65
}

linePressure = {
    'title': 'Line Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 172,
    'warningValue': 65
}

bottleTemperature = {
    'title': 'Bottle Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': -40,
    'yUpperBound': 125,
    'warningValue': 25
}

nozzleTemperature = {
    'title': 'Nozzle Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': 0,
    'yUpperBound': 600,
    'warningValue': None
}

loadCell= {
    'title': 'Load Cell Force',
    'xLabel': 'Time',
    'yLabel': 'Force', 
    'xUnit': 's', 
    'yUnit': 'N', 
    'yLowerBound': 0,
    'yUpperBound': 1112,
    'warningValue': None
}

ambientTemperature = {
    'title': 'Ambient Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': 0,
    'yUpperBound': 50,
    'warningValue': None
}

plumeTemperature = {
    'title': 'Plume Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': -50,
    'yUpperBound': 1030,
    'warningValue': None
}

data = [chamberPressure, 
        bottlePressure,
        linePressure,
        bottleTemperature,
        nozzleTemperature,
        loadCell,
        ambientTemperature,
        plumeTemperature]