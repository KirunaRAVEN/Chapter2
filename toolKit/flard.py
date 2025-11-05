#import pyduinocli
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
    parsedFetchInfos = [(x.commit.committed_date, x.commit.author.email, x.commit.message) for x in fetchInfos]

    print("most recent pushes:")
    for i in parsedFetchInfos:
        print(f"On {time.asctime(time.localtime(i[0]))}, by {i[1]} with message {i[2]}")
    """
    localCommit = r.head.object
    remoteCommit = r.remote().refs[-1].commit #has fixed offset, make it find it dynamically. TODO
    if localCommit.hexsha != remoteCommit.hexsha:
        print("[!] on outdated commit")
        # TODO: pull the most recent commit
    else:
        print("[+] everything is up to date")

    """

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
