#!/bin/python3
import socket
import time



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
    listenSock.bind(("", 6000))
    listenSock.listen(10)

    print("[!] connecting ", end="")
    while True:
        try:
            debugSock.connect(("rock-4c-plus.local", 5011))
            break
        except:
            print(".", end="", flush=True)
            time.sleep(1)
    print("")
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
                    f.write(f"{m}\n")
        #time.sleep(0.1)
