
import git
import os
import subprocess
import sys
import time
cli_path = "arduino-cli"
git_local_path = "./TestBenchRepo"
repo_link = "https://github.com/KirunaRAVEN/Chapter2.git"
BRANCH_NAME = "refactor-dev"

if __name__ == '__main__':

    if "-v" in sys.argv:
        out = None
    else:
        out = subprocess.DEVNULL

    #connection to arduino stuff
    # TODO

    #network stuff
    print("[+] trying connection ...")
    pingCommand = ["ping", "-c", "1", "-W", "0.1", "github.com"]
    while True:
        rv = subprocess.call(pingCommand, stdout=out, stderr=out)
        if not rv:
            print("[+] connection successfull")
            break #connection successfull
        print("[!] Connection unsuccessfull. ([s]kip/[r]etry/[c]onnect)?")
        userInput = ""
        while(userInput not in ["s", "r", "c"]):
            userInput = input()
        match userInput:
            case "s":
                print("[!] skipping network connection")
                break
            case "r":
                print("[+] retrying connection ...")
                continue
            case "c":
                print("[+] connecting to Wi-Fi")
                subprocess.call(["nmcli", "device","wifi","connect","LTU"])
                print("[+] openin browser for login")
                subprocess.call(["firefox", "https://github.com"])
            case _:
                print("[!] unknown")

    # set up git backend and check that everything is up to date
    r = git.Repo(".", search_parent_directories=True)
    if r.head.ref.name != BRANCH_NAME:
        print("[!] on invalid branch")
        # TODO: Change branch
        exit()
    else:
        print("[+] on valid branch")

    print("[+] pulling the most recent commit")
    fetchInfos = r.remotes.origin.pull()
    parsedFetchInfos = [(x.commit.committed_date, x.commit.author.name, x.commit.message, x.remote_ref_path) for x in fetchInfos]
    print("[+] most recent push:")
    for i in parsedFetchInfos:
        if i[3].strip() == BRANCH_NAME.strip():
            print(f"   [*] [{time.asctime(time.localtime(i[0]))}] ({i[1]}): '{i[2].strip()}'")

    # check that burnTime is correct
    with open("TestStandSoftware/Globals.h") as f:
        globalConstants = f.readlines()
    for line in globalConstants:
        if "const int16_t burnTime" in line:
            line = line.rstrip("\n")
            print(f"[+] current burn time: {line.split(' ')[-1].rstrip(';')} ms")
        # TODO: Update BurnTime from here?

    # flash the arduino
    compileCommand = ["arduino-cli", "compile", "--fqbn", "arduino:mbed_portenta:envie_m7", "TestStandSoftware/TestStandSoftware.ino"]
    flashCommand = ["arduino-cli", "upload", "/dev/ttyACM0", "--fqbn", "arduino:mbed_portenta:envie_m7", "TestStandSoftware/TestStandSoftware.ino"]
    if subprocess.call(compileCommand, stdout=out, stderr=out):
        print("[!] compilation failed")
    else:
        if subprocess.call(flashCommand, stdout=out, stderr=out):
            print("[!] flashing failed")
        else:
            print("[+] test stand updated")
