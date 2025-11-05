import csv
import random
import time

FILENAME = "data.csv"
SAMPLING_HZ = 300           # simulated sampling rate
MEGA_TIME_INCREMENT = int(1e6 / SAMPLING_HZ)  # microseconds between samples
MESSAGE_PERIOD = 10_000     # every N rows one message
MESSAGE_INDEX = 1           # the message number you want (can be any 1–33)

def random_float(low, high):
    return round(random.uniform(low, high), 3)

def random_int(low, high):
    return random.randint(low, high)

def generate_row(megatime, counter):
    """Generate one 25-field row of random telemetry."""
    row = [0.0] * 25
    row[0]  = megatime
    row[1]  = random_float(5, 80)   # Line_Pres
    row[2]  = random_float(0, 60)   # Chamber_Pres
    row[3]  = random_float(10, 80)  # OxBottle1_Pres
    row[4]  = random_float(10, 80)  # OxBottle2_Pres
    row[5]  = random_float(0, 100)  # LoadCell
    row[7]  = random_float(20, 300) # NozzleTemp
    row[9]  = random_float(20, 800) # IRsensor
    row[10] = random_int(0, 1)      # Dump_Relay
    row[12] = random_int(0, 1)      # N2_Relay
    row[14] = random_int(0, 1)      # IgnRelay
    row[15] = random_int(0, 1)      # Ox1Valve_Realy
    row[19] = random_float(20, 80)  # N2Bottle_Pres
    row[20] = random_float(-15, 40) # OxBottle1_temp
    row[21] = random_float(-15, 40) # OxBottle2_temp
    row[22] = random_int(0, 1)      # OxBottle1_Relay
    row[23] = random_int(0, 1)      # OxBottle2_Relay

    # Only every MESSAGE_PERIOD rows emit a message; others are 0
    if counter % MESSAGE_PERIOD == 0:
        row[24] = MESSAGE_INDEX
    else:
        row[24] = 0

    return row

def write_fake_data():
    megatime = 0
    counter = 0

    # clear file on start
    open(FILENAME, "w").close()

    while True:
        row = generate_row(megatime, counter)
        with open(FILENAME, "a", newline="") as csvfile:
            csv.writer(csvfile).writerow(row)
        megatime += MEGA_TIME_INCREMENT
        counter += 1
        time.sleep(1 / SAMPLING_HZ)

if __name__ == "__main__":
    write_fake_data()
