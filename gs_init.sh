#!/bin/bash
# execute bash script with 'source gs_init.sh'

# navigate to working directory
cd ~/Documents/GroundStationSoftware/interface/
echo 'Pinging rock ...'

# ping the rock to check packet loss
ping -c 1 rock-4c-plus.local | grep 'received'

# establish ssh connection and remove old csv
sshpass -p "rock" ssh rock@rock-4c-plus.local "cd scripts; echo 'y' | rm data.csv ; echo 'rock' | sudo -S python3 serial_reader.py" &

# give time to the rock for creating the new csv
sleep 1

gnome-terminal -- sh -c "sshpass -p 'rock' ssh rock@rock-4c-plus.local 'tail -f scripts/data.csv' | tee data.csv"  

#gnome-terminal -- sh -c "sshpass -p 'rock' ssh rock@rock-4c-plus.local 'tail -f scripts/data.csv' | tee ~/Documents/GroundStationSoftware/interface/data.csv"  

sleep 1

# gnome-terminal -- sh -c "python3 live_grapher_V3.py"

gnome-terminal -- sh -c "cd ~/Documents/GroundStationSoftware/interface/ && python3 interface.py"
#gnome-terminal -- sh -c "cd ~/Documents/ROCK_SOFTWARE && python3 live_grapher_V4.py"
