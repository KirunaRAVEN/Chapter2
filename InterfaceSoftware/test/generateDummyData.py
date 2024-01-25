import time
import csv
import random

# create or overwrite the csv file
open('dummyData.csv', 'w')

# list for the row elements
row = []
for i in range(21):
    row.append(i)

while True:
    with open('dummyData.csv', 'a', newline='') as csv_file:
        csv_writer = csv.writer(csv_file, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        for i in range(21):
           row[i] = i * random.randrange(-5,10) 
        csv_writer.writerow(row)
        time.sleep(0.01)