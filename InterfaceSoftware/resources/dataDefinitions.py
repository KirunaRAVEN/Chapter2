chamberPressure = {
    'title': 'Chamber Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 172,
    'warningValue': 40,
    'csvIndex' : 5
}

N2O_BottlePressure = {
    'title': 'N$_2$O Bottle Pressure',
    'xLabel': 'Time',
    'yLabel': 'Pressure', 
    'xUnit': 's', 
    'yUnit': 'Bar', 
    'yLowerBound': 0,
    'yUpperBound': 100,
    'warningValue': 65,
    'csvIndex' : 6
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
    'csvIndex' : 4
}

heatingBlanketTemperature = {
    'title': 'Bottle Temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': -40,
    'yUpperBound': 125,
    'warningValue': 25,
    'csvIndex' : 8
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
    'csvIndex' : 10
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
    'csvIndex' : 7
}

ambientTemperature = {
    'title': 'Piping temperature',
    'xLabel': 'Time',
    'yLabel': 'Temperature', 
    'xUnit': 's', 
    'yUnit': '°C', 
    'yLowerBound': 0,
    'yUpperBound': 50,
    'warningValue': 0,
    'csvIndex' : 11
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
    'csvIndex' : 12
}

graphData = [chamberPressure, 
             N2O_BottlePressure,
             linePressure,
             heatingBlanketTemperature,
             nozzleTemperature,
             loadCell,
             ambientTemperature,
             plumeTemperature]

displayData = [N2O_BottlePressure,
               heatingBlanketTemperature,
               loadCell]