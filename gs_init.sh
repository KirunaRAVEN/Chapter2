#!/bin/bash
# execute bash script with 'source gs_init.sh'

# navigate to working directory
cd ~/Documents/ROCK_SOFTWARE/
echo 'Pinging rock ...'

# ping the rock to check packet loss
ping -c 1 rock-4c-plus.local | grep 'received'

# establish ssh connection 
sshpass -p "rock" ssh rock@rock-4c-plus.local "cd scripts; echo 'rock' | sudo -S python3 serial_reader.py" &

gnome-terminal -- sh -c "sshpass -p 'rock' ssh rock@rock-4c-plus.local 'tail -f scripts/data.csv' | tee data.csv"  

gnome-terminal -- sh -c "python3 live_grapher_V3.py"
