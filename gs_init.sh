#!/bin/bash
NAME=RavenGS
cd interface
cp data.csv data.csv.old
rm data.csv
touch data.csv
tmux kill-session -t $NAME
tmux new -s $NAME -d
tmux send-keys -t $NAME "python3 writerReciever.py $1" C-m
tmux split-window -h -t $NAME
tmux send-keys -t $NAME 'python3 run_interface.py' C-m
tmux split-window -v -t $NAME
tmux send-keys -t $NAME 'tail -f data.csv' C-m
tmux select-pane -t $NAME:0.0
tmux split-window -v -t $NAME

tmux attach -t $NAME
