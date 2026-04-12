import sys

with open(sys.argv[1]) as f:
    data = f.readlines()

data.pop(0)

for line in data:
    l = line.split(",")
    if l[4][-2:] == ".0":
        l[4] = str(float(l[4])*25/(2**10))
    else:
        l[4] = str(float(l[4])/4)

    print(",".join(l), end="")
