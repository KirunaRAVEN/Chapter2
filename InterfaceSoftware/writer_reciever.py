#!/bin/python3
import socket
import time
import subprocess

def pollRecieve(sock):
    try:
        msg = sock.recv(512).decode()
        if msg == "":
            return False
        if '\n' in msg:
            return msg.split('\n')
        else:
            return [msg]
    except:
        return True

if __name__ == '__main__':
    listenSock = socket.socket(type=socket.SOCK_STREAM)
    debugSock = socket.socket(type=socket.SOCK_STREAM)
    print("[!] setting up incoming connection ", end="", flush=True)
    while True:
        try:
            listenSock.bind(("", 6000))
            break
        except:
            print(".", end="", flush=True)
            time.sleep(0.5)
    print("!")
    listenSock.listen(10)

    print("[!] checking outgoing connection ",end="", flush=True)
    command = ["ping", "-c", "1", "-W", "0.1", "rock-4c-plus.local"]
    failcount = 0
    while True:
        rv = subprocess.call(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if not rv:
            break
        print(".", end="", flush=True)
        failcount += 1
        if failcount == 10:
            print("\n[!] connection is taking long, double check the connection", end="", flush=True)
            failcount = 0
        time.sleep(0.5)
    print("!")

    print("[!] connecting ", end="", flush=True)
    while True:
        try:
            debugSock.connect(("rock-4c-plus.local", 5011))
            break
        except:
            print(".", end="", flush=True)
            time.sleep(0.5)
    print("!")
    print("[!] connected, listening for data conn ... ")
    dataSock, addr = listenSock.accept()
    print("[!] connected")
    debugSock.settimeout(0.01)
    dataSock.settimeout(0.01)

    with open("data.csv", "a") as f:
        while True:
            msg = pollRecieve(dataSock)
            if msg == False:
                printf("[!] connection dead, reset reciever")
            if msg != True:
                for m in msg:
                    print(f"[+] {m}")
                    f.write(f"{m}\n")
            msg = pollRecieve(debugSock)
            if msg == False:
                printf("[!] connection dead, reset reciever")
            if msg != True:
                for m in msg:
                    print(f"[*] {m}")
            f.flush()

