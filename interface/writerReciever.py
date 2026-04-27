#!/bin/python3
import socket
import time
import struct
import threading
import sys
import random

PORT = 4000
LEN_NORMAL = 52
LEN_HIGH_SPEED = 4

g_dataQueue = b''

g_data = {
"timestamp":0,
"N20FeedingPressure1":0.0,
"N20FeedingPressure2":0.0,
"linePressure":0.0,
"chamberPressure":0.0,
"N2FeedingPressure":0.0,
"bottleTemperature1":0.0,
"bottleTemperature2":0.0,
"plumeTemperature":0.0,
"pipingTemperature":0.0,
"chamberTemperature":0.0,
"loadCell":0.0,
"mode":0,
"subState":0,
"message":0,
"dumpValveButton":0,
"heatingBlanketButton1":0,
"heatingBlanketButton2":0,
"ignitionButton":0,
"N2ValveButton":0,
"N20ValveButton":0,
"valveActive":0,
"ignitionEngagedActive":0,
}

def _readBitFromByte(byte, bit):
    """
    Read a single bit from a byte
    """
    if byte & 1 << bit: # somehow it is reversed when it is sent, idk.
        return 1
    else:
        return 0

def reciever(sock):
    """
    Recieves data from the socket and puts it into the dataqueue.
    Designed as a thread.
    """
    print("[+] connected")
    global g_dataQueue
    while True:
        try:
            msg = sock.recv(1024)
            g_dataQueue += msg
        except Exception as e:
            print(e)

def printTelemetry(datafile):
    datafile.write(f"{g_data['timestamp']},{g_data['N20FeedingPressure1']},{g_data['N20FeedingPressure2']},{g_data['linePressure']},{g_data['chamberPressure']},{g_data['N2FeedingPressure']},{g_data['bottleTemperature1']},{g_data['bottleTemperature2']},{g_data['plumeTemperature']},{g_data['pipingTemperature']},{g_data['chamberTemperature']},{g_data['loadCell']},{g_data['mode']},{g_data['subState']},{g_data['message']},{g_data['dumpValveButton']},{g_data['heatingBlanketButton1']},{g_data['heatingBlanketButton2']},{g_data['ignitionButton']},{g_data['N2ValveButton']},{g_data['N20ValveButton']},{g_data['valveActive']},{g_data['ignitionEngagedActive']}\n")
    datafile.flush()

def parseData():
    """
    Parses the data recieved and prints it to data.csv.
    Designed as a thread.
    """
    global g_dataQueue, g_data
    print("!")

    ## initialize data

    datafile = open("data.csv", "a")
    while True:
        if(g_dataQueue):
            packetType = g_dataQueue[0]
            match packetType:
                case 1: #normal packet
                    try:
                        data = struct.unpack_from("<l11f4b", g_dataQueue, 1)
                        # TODO: make this prettier:
                        g_data["timestamp"] = data[0]
                        g_data["N20FeedingPressure1"] = data[1]
                        g_data["N20FeedingPressure2"] = data[2]
                        g_data["linePressure"] = data[3]
                        g_data["chamberPressure"] = data[4]
                        g_data["N2FeedingPressure"] = data[5]
                        g_data["bottleTemperature1"] = data[6]
                        g_data["bottleTemperature2"] = data[7]
                        g_data["plumeTemperature"] = data[8]
                        g_data["pipingTemperature"] = data[9]
                        g_data["chamberTemperature"] = data[10]
                        g_data["loadCell"] = data[11]
                        g_data["mode"] = data[12]
                        g_data["subState"] = data[13]
                        g_data["message"] = data[14]

                        g_data["dumpValveButton"] = _readBitFromByte(data[15], 0)
                        g_data["heatingBlanketButton1"] = _readBitFromByte(data[15], 1)
                        g_data["heatingBlanketButton2"] = _readBitFromByte(data[15], 2)
                        g_data["ignitionButton"] = _readBitFromByte(data[15], 3)
                        g_data["N2ValveButton"] = _readBitFromByte(data[15], 4)
                        g_data["N20ValveButton"] = _readBitFromByte(data[15], 5)
                        g_data["valveActive"] = _readBitFromByte(data[15], 6)
                        g_data["ignitionEngagedActive"] = _readBitFromByte(data[15], 7)

                        printTelemetry(datafile)
                        g_dataQueue = g_dataQueue[(1+LEN_NORMAL):]
                    except Exception as e:
                        print(f"[!] {e}")
                case 2: #high-speed packet
                    try:
                        data = struct.unpack_from("<f", g_dataQueue, 1)
                        g_data["chamberPressure"] = data[0]
                        printTelemetry(datafile)
                        g_dataQueue = g_dataQueue[(1+LEN_HIGH_SPEED):]

                    except Exception as e:
                        print(f"[!] {e}")

def fakeData():
    """
    generate fake data for testing purposes
    """
    datafile = open("data.csv", "a")
    g_data["N20FeedingPressure1"] = 67.0
    g_data["N20FeedingPressure2"] = 45.0
    g_data["loadCell"] = 50
    pressure = 40
    while True:
        g_data["timestamp"] += 100
        g_data["N20FeedingPressure1"] = pressure+ 5*(random.random()-0.5)
        g_data["N20FeedingPressure2"] += random.random()-0.5
        g_data["loadCell"] += random.random()-0.5

        if random.randint(0,10) == 1:
            g_data["N20ValveButton"] = 1
        else:
            g_data["N20ValveButton"] = 0
        if random.randint(0,10) == 1:
            g_data["N2ValveButton"] = 1
        else:
            g_data["N2ValveButton"] = 0
        if random.randint(0,10) == 1:
            g_data["dumpValveButton"] = 1
        else:
            g_data["dumpValveButton"] = 0

        printTelemetry(datafile)
        time.sleep(0.1)


if __name__ == '__main__':
    if "-d" in sys.argv:
        print("[+] setting up fake data")
        fakeData()
    else:
        listenSock = socket.socket(type=socket.SOCK_STREAM)
        print("[+] setting up incoming connection ", end="", flush=True)
        while True:
            try:
                listenSock.bind(("", PORT))
                break
            except:
                print(".", end="", flush=True)
                time.sleep(0.5)
        print("!")
        listenSock.listen(10)

    print("[+] setting up parser ...", end="", flush=True)
    threading.Thread(target=parseData).start()


    while True:
        print("[+] listening for data conn ... ")
        dataSock, addr = listenSock.accept()
        threading.Thread(target=reciever, args=(dataSock,)).start()
