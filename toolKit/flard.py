import pyduinocli
import git
import os
cli_path = "arduino-cli"
git_local_path = "./TestBenchRepo"
repo_link = "https://github.com/MrLuckyOne/gibbgrottan.git"

def deleteDirectory(pathToDelete):
    for root, dirs, files in os.walk(pathToDelete):
        for file in files:
            file_path = os.path.join(root, file)
            os.chmod(file_path, 0o777)
            os.remove(file_path)
        for directory in dirs:
            file_path = os.path.join(root, directory)
            deleteDirectory(file_path)
            os.chmod(file_path, 0o777)
            os.rmdir(file_path)
        return 0

def getLatestRepo(linkToRepo, endDirectory):
        deleteDirectory(endDirectory)
        git.Repo.clone_from(linkToRepo, endDirectory)
        return 0

def setupArduinoCli(cliPath):
    try:
        arduino = pyduinocli.Arduino(cliPath)
        print(arduino.version())
        return 0
    except:
        print("arduino-cli not found")
        return 1


def main():
    # Set up the arduino command line interface
    setupArduinoCli(cli_path)
    getLatestRepo(repo_link, git_local_path)

main()

