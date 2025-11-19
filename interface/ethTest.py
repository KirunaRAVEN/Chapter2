#!/bin/python3
import socket
import threading
import time

recvPackets = 0
recvData = 0

def reciever(conn):
    """
    get incoming data and log metadata
    """
    global recvPackets, recvData
    while True:
        try:
            data = conn.recv(2048)
            if data:
                recvPackets += 1
                recvData += len(data)
        except:
            continue


def printStats():
    """
    print metadata every second
    """
    global recvPackets, recvData
    while True:
        if recvPackets:
            print(f"[*] {recvPackets} \tpackets/s, {round((recvData/1000)*8,1)} kbps/s")
            recvPackets = recvData = 0
        time.sleep(1)




if __name__ == '__main__':
    threading.Thread(target=printStats).start()

    host = ""
    port = 4000
    serverSocket = socket.socket()
    serverSocket.bind((host,port))
    print("[+] server initialized")
    serverSocket.listen(0)

    while True:
        conn, addr = serverSocket.accept()
        print("[+] connection aquired")
        threading.Thread(target=reciever, args=(conn,)).start()

