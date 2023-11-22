import datetime as dt
import time
import random

dataFile = open('dummyData.csv', 'w')

values = [0] * 8

while True:
    # get data values and convert to f-strings
    timeStamp = dt.datetime.now().time()
    str = f'{timeStamp}'
    for i in range(len(values)):
        values[i] = random.random()
        str += f',{values[i]}'
    str += '\n'

    # write the dataline to the file
    dataFile.write(str)

    # force write to disk instead of file buffer
    dataFile.flush()
    time.sleep(1)