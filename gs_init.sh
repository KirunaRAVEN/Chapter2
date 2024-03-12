#!/bin/bash
# execute bash script with 'source gs_init.sh'
# ssh-keygen -t rsa -b 4096
# ssh-copy-id <user>@<server>
# ssh <user>@<server>

# navigate to working directory
cd Documents/ROCK_SOFTWARE/

# ping the rock to check packet loss
ping -c 1 rock-4c-plus.local | grep 'received'

# establish ssh connection 
sshpass -p "rock" ssh -o StrictHostKeyChecking=no -t rock@rock-4c-plus.local "echo 'rock' | sudo -S python3 scripts/serial_reader.py" &  

#echo pass | ssh -tt rock@rock-4c-plus.local "python3 serial_reader.py"
#ssh rock@rock-4c-plus.local 

tilix -a session-add-right -x "sleep 1; sshpass -p 'rock' ssh rock@rock-4c-plus.local 'tail -f scripts/data.csv' | tee data.csv" 
#tilix -a session-add-down -x "python3 live_grapher_V3.py"
