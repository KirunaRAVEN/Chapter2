#!/bin/bash
# execute bash script with 'source gs_init.sh'

# navigate to working directory
cd Documents/ROCK_SOFTWARE/

# ping the rock for 5s to check packet loss
function rock_ping {
    timeout 1s ping rock-4c-plus.local
    return
}

rock_ping
echo "Rock pinged"

# establish ssh connection 
function connect_ssh {
    #ssh rock@rock-4c-plus.local
    #ssh rock@rock-4c-plus.local 'cd scripts/'
    #ssh rock@rock-4c-plus.local 'python3 serial_reader.py'
    echo "Establishing SSH connection"
    return
}

function read_data {
    # ssh rock@rock-4c-plus.local 'tail -f scripts/data.csv' | tee data.csv 
    echo 'sheeshion 1'
    return
    #$SHELL
}

function graph_data {
    # python3 live_grapher_V3.py
    echo "sheeshion 2"
}

connect_ssh

tilix -a session-add-right -x "python3 generateDummyData.py" 
tilix -a session-add-down -x "python3 live_grapher_V3.py"