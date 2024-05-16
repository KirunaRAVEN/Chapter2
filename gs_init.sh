#!/bin/bash
# execute this bash script in a terminal with the command 'source gs_init.sh'
# alternatively, edit the .bashrc file in the home/ directory by adding the line ''
# the script can then be launched by the shortcut 'gsi'

# navigate to working directory
cd ~/Documents/ROCK_SOFTWARE/

# ping the rock three times to check packet loss
echo 'Pinging rock ...'
ping -c 3 rock-4c-plus.local | grep 'received'

# establish ssh connection and launch the serial reader code on the rock 
sshpass -p "rock" ssh rock@rock-4c-plus.local "cd scripts; echo 'rock' | sudo -S python3 serial_reader.py" &

# open new terminal and stream the data to the laptop
gnome-terminal -- sh -c "sshpass -p 'rock' ssh rock@rock-4c-plus.local 'tail -f scripts/data.csv' | tee data.csv"  

# open new terminal and run the interface software
gnome-terminal -- sh -c "python3 live_grapher_V3.py"
