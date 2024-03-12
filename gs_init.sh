#!/bin/bash
# execute bash script with 'source gs_init.sh'
# ssh-keygen -t rsa -b 4096
# ssh-copy-id <user>@<server>
# ssh <user>@<server>

# navigate to working directory
cd Documents/ROCK_SOFTWARE/

# ping the rock for 5s to check packet loss

ping -c 5 rock-4c-plus.local | grep 'received'

# establish ssh connection 
ssh rock@rock-4c-plus.local 'cd scripts/ ; python3 serial_reader.py'

tilix -a session-add-right -x "python3 generateDummyData.py" 
tilix -a session-add-down -x "python3 live_grapher_V3.py"