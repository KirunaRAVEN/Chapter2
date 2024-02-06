chamberPressure= {
    'title': 'Chamber Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 172,
    'warningValue': 40,
    'csvIndex' : 1
}

bottlePressure = {
    'title': 'Bottle Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 100,
    'warningValue': 65,
    'csvIndex' : 2
}

linePressure = {
    'title': 'Line Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 172,
    'warningValue': 65,
    'csvIndex' : 3
}

bottleTemperature = {
    'title': 'Bottle Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': -40,
    'yUpperBound': 125,
    'warningValue': 25,
    'csvIndex' : 4
}

nozzleTemperature = {
    'title': 'Nozzle Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': 0,
    'yUpperBound': 600,
    'warningValue': 0,
    'csvIndex' : 5
}

loadCell= {
    'title': 'Load Cell Force',
    'xLabel': 'Time',
    'yLabel': 'Force', 
    'xUnit': 's', 
    'yUnit': 'N', 
    'yLowerBound': 0,
    'yUpperBound': 1112,
    'warningValue': 750,
    'csvIndex' : 6
}

ambientTemperature = {
    'title': 'Ambient Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': 0,
    'yUpperBound': 50,
    'warningValue': 0,
    'csvIndex' : 7
}

plumeTemperature = {
    'title': 'Plume Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': -50,
    'yUpperBound': 1030,
    'warningValue': 0,
    'csvIndex' : 8
}

graphData = [chamberPressure, 
             bottlePressure,
             linePressure,
             bottleTemperature,
             nozzleTemperature,
             loadCell,
             ambientTemperature,
             plumeTemperature]

displayData = [bottlePressure,
               bottleTemperature,
               loadCell]