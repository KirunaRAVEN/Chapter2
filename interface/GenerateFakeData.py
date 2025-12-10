import csv
import random
import time

FILE_NAME = "data.csv"
SAMPLING_HZ = 300           # simulated sampling rate
MEGA_TIME_INCREMENT = int(1e6 / SAMPLING_HZ)  # microseconds between samples
MESSAGE_PERIOD = 10_000     # every N rows -> one message
MESSAGE_INDEX = 1           # the message number you want (can be any 1–33)

def randomFloat(low, high):
    return round(random.uniform(low, high), 3)

def randomInt(low, high):
    return random.randint(low, high)

def generateRow(megaTime, counter):
    """Generate one 25-field row of random telemetry."""
    row = [0.0] * 25
    row[0]  = megaTime
    row[1]  = randomFloat(5, 80)   # Line_Pres
    row[2]  = randomFloat(0, 60)   # Chamber_Pres
    row[3]  = randomFloat(10, 80)  # OxBottle1_Pres
    row[4]  = randomFloat(10, 80)  # OxBottle2_Pres
    row[5]  = randomFloat(0, 100)  # LoadCell
    row[7]  = randomFloat(20, 300) # NozzleTemp
    row[9]  = randomFloat(20, 800) # IRsensor
    row[10] = randomInt(0, 1)      # Dump_Relay
    row[12] = randomInt(0, 1)      # N2_Relay
    row[14] = randomInt(0, 1)      # IgnRelay
    row[15] = randomInt(0, 1)      # Ox1Valve_Realy
    row[16] = randomInt(0, 5)      # Software Mode
    row[17] = randomInt(0, 6)      # Software substrate
    row[19] = randomFloat(20, 80)  # N2Bottle_Pres
    row[20] = randomFloat(-15, 40) # OxBottle1_temp
    row[21] = randomFloat(-15, 40) # OxBottle2_temp
    row[22] = randomInt(0, 1)      # OxBottle1_Relay
    row[23] = randomInt(0, 1)      # OxBottle2_Relay

    # Only every MESSAGE_PERIOD rows emit a message; others are 0
    if counter % MESSAGE_PERIOD == 0:
        row[24] = MESSAGE_INDEX
    else:
        row[24] = 0

    return row

def writeFakeData():
    megaTime = 0
    counter = 0

    # clear file on start
    open(FILE_NAME, "w").close()

    while True:
        row = generateRow(megaTime, counter)
        with open(FILE_NAME, "a", newline="") as csvFile:
            csv.writer(csvFile).writerow(row)
        megaTime += MEGA_TIME_INCREMENT
        counter += 1
        time.sleep(1 / SAMPLING_HZ)

if __name__ == "__main__":
    writeFakeData()
