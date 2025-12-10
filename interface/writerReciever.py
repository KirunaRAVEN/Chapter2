#!/bin/python3
import socket
import time
import struct
import threading

PORT = 4000
LEN_NORMAL = 48
LEN_HIGH_SPEED = 4

g_dataQueue = b''


def reciever(sock):
    """
    Recieves data from the socket and puts it into the dataqueue
    """
    while True:
        try:
            msg = sock.recv(1024)
            g_dataQueue += msg
        except:
            pass

def parseData():
    """
    Parses the data recieved and prints it to data.csv
    """
    global g_dataQueue
    print("!")

    ## initialize data
    timestamp = 0
    N20FeedingPressure1 = 0.0
    N20FeedingPressure2 = 0.0
    linePressure = 0.0
    chamberPressure = 0.0
    N2FeedingPressure = 0.0
    bottleTemperature1 = 0.0
    bottleTemperature2 = 0.0
    engineTemperature = 0.0
    dumpValveButton = 0
    heatingBlanketButton = 0
    ignitionButton = 0
    N2ValveButton = 0
    N20ValveButton = 0
    valveActive = 0
    ignitionEngagedActive = 0
    mode = 0
    subState = 0

    datafile = open("data.csv", "a")
    while True:
        if(g_dataQueue):
            packetType = g_dataQueue[0]
            match packetType:
                case 1: #normal packet
                    try:
                        data = struct.unpack_from("<l8f3i", g_dataQueue, 1)
                        # TODO: make this prettier:
                        timestamp = data[0]
                        N20FeedingPressure1 = data[1]
                        N20FeedingPressure2 = data[2]
                        linePressure = data[3]
                        chamberPressure = data[4]
                        N2FeedingPressure = data[5]
                        bottleTemperature1 = data[6]
                        bottleTemperature2 = data[7]
                        engineTemperature = data[8]
                        mode = data[9]
                        subState = data[10]

                        #TODO: manual byte unpacking or some lib, idk
                        dumpValveButton = 0
                        heatingBlanketButton1 = 0
                        heatingBlanketButton2 = 0
                        ignitionButton = 0
                        N2ValveButton = 0
                        N20ValveButton = 0
                        valveActive = 0
                        ignitionEngagedActive = 0

                        datafile.write(f"{timestamp},{N20FeedingPressure1},{N20FeedingPressure2},{linePressure},{chamberPressure},{N2FeedingPressure},{N2FeedingPressure},{bottleTemperature1},{bottleTemperature2},{engineTemperature},{mode},{subState},{dumpValveButton},{heatingBlanketButton1},{heatingBlanketButton2},{ignitionButton},{N2ValveButton},{valveActive},{ignitionEngagedActive}\n")
                        datafile.flush()
                        g_dataQueue = g_dataQueue[(1+LEN_NORMAL):]
                    except exception as e:
                        print(f"[!] {e}")
                case 2: #high-speed packet
                    pass
            time.sleep(0.1)


if __name__ == '__main__':
    listenSock = socket.socket(type=socket.SOCK_STREAM)
    print("[+] setting up incoming connection ", end="", flush=True)
    while True:
        try:
            listenSock.bind(("", 4000))
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
        print("[+] connected")
