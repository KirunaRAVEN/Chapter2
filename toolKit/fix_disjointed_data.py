import sys

with open(sys.argv[1]) as f:
    data = f.readlines()

data.pop(0) # remove header
timeOffset = 0
lastTime = 0

for line in data:
    l = line.split(",")
    if int(l[0]) < lastTime:
        timeOffset = lastTime

    l[0] = str(int(l[0]) + timeOffset)

    print(",".join(l), end="")
